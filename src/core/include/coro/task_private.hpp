/*
Copyright 2022 Roy Awesome's Open Engine (RAOE)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <coroutine>
#include <memory>
#include <optional>
#include "types.hpp"
#include "coro/task_shared.hpp"
#include "debug.hpp"

namespace raoe::coro
{
    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumableType> class task;
    inline namespace _
    {
        struct task_private_base
        {
            task_private_base(std::coroutine_handle<> in_coro_handle)
                : coro_handle(in_coro_handle)
            {
                raoe::debug::debug_break_if(!!in_coro_handle);
            }
            ~task_private_base()
            {
                terminate();
            }

            explicit operator stop_context() const { return {&is_stop_requested}; } 
            bool stop_requested() const { return is_stop_requested; }

            void request_stop()
            {
                is_stop_requested = true;
                for(auto& stop_task : stop_tasks)
                {
                    if(auto t = stop_task.lock())
                    {
                        t->request_stop();
                    }
                }
                stop_tasks.clear();
            }

            template<typename ReturnValue, ETaskRef RefType, ETaskResumable ResumeType>
            void add_stop_task(task<ReturnValue, RefType, ResumeType>& in_task)
            {
                if(is_stop_requested)
                {
                    in_task.request_stop();
                }
                else if(in_task.is_valid())
                {
                    stop_tasks.push_back(in_task.private_task());
                }
            }

            template<typename ReturnValue, ETaskRef RefType, ETaskResumable ResumeType>
            void remove_stop_task(task<ReturnValue, RefType, ResumeType>& in_task)
            {
                if(in_task.is_valid())
                {
                    for(size_t i = 0; i < stop_tasks.size(); i++)
                    {
                        if(stop_tasks[i].lock() == in_task.private_task())
                        {
                            stop_tasks[i] = stop_tasks.back();
                            stop_tasks.pop_back();
                            return;
                        }
                    }
                }
            }

            ETaskStatus resume()
            {
                //PRECONDITION: internal_state != EInternalState::Resuming

                if(internal_state == EInternalState::Destroyed)
                {
                    return ETaskStatus::Done;
                }

                internal_state = EInternalState::Resuming;

                if(subtask_internal)
                {
                    if(stop_requested())
                    {
                        subtask_internal->is_stop_requested = true;
                    }

                    if(subtask_internal->resume() != ETaskStatus::Done)
                    {
                        internal_state = EInternalState::Idle;
                        return ETaskStatus::Suspended;
                    }
                    subtask_internal = nullptr;
                }

                if(can_resume())
                {
                    ready_func = nullptr;
                    coro_handle.resume();
                }

                const ETaskStatus status = coro_handle.done() ? ETaskStatus::Done : ETaskStatus::Suspended;
                if(status == ETaskStatus::Done)
                {
                    is_done = true;
                }
                internal_state = EInternalState::Idle;
                return status;
            }

            void set_subtask(std::shared_ptr<task_private_base> in_subtask)
            {
                subtask_internal = in_subtask;
            }

        private:
            template<typename ReturnType> friend class promise_base;
            template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumableType> friend class task;
            template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType, typename promise_type> friend struct task_awaiter_base;


            void terminate()
            {
                if(internal_state == EInternalState::Idle)
                {
                    is_done = true;

                    if(subtask_internal)
                    {
                        subtask_internal->terminate();
                    }

                    coro_handle.destroy();
                    coro_handle = nullptr;
                    ready_func = nullptr;
                    internal_state = EInternalState::Destroyed;
                }
            }

            void set_ready_func(const TaskReadyFunc& in_ready_func)
            {
                ready_func = in_ready_func;
            }
            TaskReadyFunc ready_func;

            bool can_resume() const
            {
                if(done())
                {
                    return false;
                }
                if(subtask_internal)
                {
                    return subtask_internal->can_resume();
                }
                const bool is_ready = !ready_func || ready_func();
                return is_ready;
            }

            bool done() const { return is_done; }
            bool is_done = false;

            enum class EInternalState
            {
                Idle,
                Resuming,
                Destroyed,
            };
            
            EInternalState internal_state = EInternalState::Idle;

            std::coroutine_handle<> coro_handle;
            std::shared_ptr<task_private_base> subtask_internal;

            bool is_stop_requested = false;
            std::vector<std::weak_ptr<task_private_base>> stop_tasks;

            int32 ref_count = 0;
            void add_ref()
            {
                ++ref_count;
            }
            void remove_ref()
            {
                --ref_count;
                if(ref_count <= 0)
                {
                    terminate();
                }
            }
        };  


    }

    template<typename ReturnType>
    class task_private : public task_private_base
    {
    public:
        using promise_type = promise<ReturnType>;
        using super = _::task_private_base;

         task_private(std::coroutine_handle<promise_type> in_handle)
            : super(in_handle)
        {
            auto& promise_ptr = in_handle.promise();
            promise_ptr.set_internal_task(this);
        }

        void set_return_value(const ReturnType& in_retval)
        {
            ReturnType retval_cpy = in_retval;
            set_return_value(std::move(retval_cpy));
        }

        void set_return_value(ReturnType&& in_retval)
        {
            //PRECONDITION: retval_state != ERetValState::Set
            //PRECONDITION: retval_state != ERetValState::Taken
            //PRECONDITION: retval_state != ERetValState::Orphaned
            if(retval_state == ERetValState::Unset)
            {
                retval = std::move(in_retval);
                retval_state = ERetValState::Set;
                return;
            }
        }

        std::optional<ReturnType> take_return_value()
        {
            //PRECONDITION: retval_state != ERetValState::Taken
            //PRECONDITION: retval_state != ERetValState::Orphaned
            if(retval_state == ERetValState::Set)
            {
                retval_state = ERetValState::Taken;
                return std::move(retval);
            }
            return {};
        }

        void on_task_promise_destroyed()
        {
            if(retval_state == ERetValState::Unset)
            {
                retval_state = ERetValState::Orphaned;
            }
        }

    private:
        enum class ERetValState
        {
            Unset,
            Set,
            Taken,
            Orphaned,
        };

        ERetValState retval_state = ERetValState::Unset;
        std::optional<ReturnType> retval;
    };

    template<>
    class task_private<void> : public _::task_private_base
    {
    public: 
        using promise_type = promise<void>;
        using super = _::task_private_base;

        task_private(std::coroutine_handle<promise_type> in_handle)
            : super(in_handle)
        {
            auto& promise_ptr = in_handle.promise();
            promise_ptr.set_internal_task(this);
        }

        void take_return_value() { }
        void on_task_promise_destroyed() { }
    };

}
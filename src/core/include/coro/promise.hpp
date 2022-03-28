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
#include "types.hpp"
#include "coro/task_shared.hpp"
#include "coro/awaiters.hpp"

namespace raoe::coro
{
    template<typename ReturnType>
    class promise;  
    template<typename ReturnType>
    class task_private;

    inline namespace _
    {      
        template <typename ReturnType>
        struct promise_base
        {
            using promise_type = promise<ReturnType>;
            using internal_task = task_private<ReturnType>;

            ~promise_base()
            {
                if(task)
                {
                    task->on_task_promise_destroyed();
                }
            }

            auto initial_suspend() noexcept
            {
                return std::suspend_always();
            }

            auto final_suspend() noexcept
            {
                return std::suspend_always();
            }

            void set_ready_func(const TaskReadyFunc& in_func)
            {
                if(task)
                {
                    task->set_ready_func(in_func);
                }
            }

            auto get_return_object()
            {
                return std::coroutine_handle<promise_type>::from_promise(*static_cast<promise_type*>(this));
            }

            static std::shared_ptr<internal_task> get_return_object_on_allocation_failure()
            {
                return {};
            }

            void unhandled_exception() noexcept
            {
                //TODO: Exception Handling 
            }

            void set_internal_task(internal_task* in_internal_task)
            {
                task = in_internal_task;
            }

            internal_task* get_internal_task()
            {
                return task;
            }

            const internal_task* get_internal_task() const
            {
                return task;
            }

            auto await_transform(std::suspend_always in_awaiter)
            {
                return in_awaiter;
            }

            auto await_transform(std::suspend_never in_awaiter)
            {
                return in_awaiter;
            }

            auto await_transform(get_stop_context in_awaiter)
            {
                struct get_stop_context_awaiter : public std::suspend_never
                {
                    get_stop_context_awaiter(stop_context in_stop_context)
                        : stop_context(in_stop_context)
                    {

                    }
                    auto await_resume() noexcept
                    {
                        return stop_context;
                    }

                    stop_context stop_context;
                };

                return get_stop_context_awaiter(static_cast<stop_context>(task));
            }

            auto await_transform(const TaskReadyFunc& task_ready)
            {
                bool is_ready = task_ready();
                if(!is_ready)
                {
                    task->set_ready_func(task_ready);
                }
                return suspend_if(!is_ready);
            }

            template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType,
                typename std::enable_if_t<ResumeType == ETaskResumable::Yes>* = nullptr>
            auto await_transform(task<ReturnType, RefType, ResumeType>&& in_task)
            {
                return task_awaiter<ReturnType, RefType, ResumeType, promise_type>(std::move(in_task));
            }

            template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType,
                typename std::enable_if_t<ResumeType == ETaskResumable::No>* = nullptr>
            auto await_transform(task<ReturnType, RefType, ResumeType> in_task)
            {
                return task_awaiter<ReturnType, RefType, ResumeType, promise_type>(std::move(in_task));
            }
            template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType,
                typename std::enable_if_t<ResumeType == ETaskResumable::No>* = nullptr>
            auto await_transform(const task<ReturnType, RefType, ResumeType>& in_task)
            {
                static_assert(raoe::static_false<ReturnType>::value, "Cannot await a non-copyable (resumable) task by copy.  try co_await std::move(task), co_await weak_task_handle(task), or co_await task.wait_until_done()");
                return task_awaiter<ReturnType, RefType, ResumeType, promise_type>(std::move(in_task));
            }
        private:
            internal_task* task = nullptr;
        };
    }
   
    template<typename ReturnType>
    struct promise : public promise_base<ReturnType>
    {   
        void return_value(const ReturnType& retval)
        {
            this->task->set_return_value(retval);
        }
        void return_value(ReturnType&& retval)
        {
            this->task->set_return_value(std::move(retval));
        }
    };

    template<>
    struct promise<void> : public promise_base<void>
    {
        void return_void() {}
    };
}
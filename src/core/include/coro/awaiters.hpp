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
#include "coro/task_shared.hpp"

namespace raoe::coro
{
    struct suspend_if
    {
        suspend_if(bool in_suspend)
            : suspend(in_suspend)
        {

        }

        bool await_ready() noexcept { return !suspend; }
        void await_suspend(std::coroutine_handle<>) noexcept { }
        void await_resume() noexcept {}
    private:
        bool suspend;
    };

    inline namespace _
    {
        template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType, typename promise_type>
        struct task_awaiter_base
        {
            using task_type = task<ReturnType, RefType, ResumeType>;
            task_awaiter_base(const task_type& in_task) {}
            task_awaiter_base(task_type&& in_task)
                : task(std::move(in_task))
            {
                //PRECONDITION: task.is_valid();
            }

            bool await_ready() noexcept
            {
                return task.is_done();
            }

            template<ETaskResumable UResumable = ResumeType, typename std::enable_if_t<UResumable == ETaskResumable::Yes>* = nullptr>
            bool await_suspend(std::coroutine_handle<promise_type> in_coro) noexcept
            {
                auto& promise = in_coro.promise();
                auto task_private = promise.get_internal_task();
                auto subtask_private = task.private_task();

                if(task_private->stop_requested())
                {
                    subtask_private->request_stop();
                }

                task_private->set_subtask(std::static_pointer_cast<_::task_private_base>(subtask_private));

                if(task.resume() == ETaskStatus::Done)
                {
                    task_private->set_subtask(nullptr);
                    return false;
                }

                return true;
            }

            template<ETaskResumable UResumable = ResumeType, typename std::enable_if_t<UResumable == ETaskResumable::No>* = nullptr>
            bool await_suspend(std::coroutine_handle<promise_type> in_coro) noexcept
            {
                auto& promise = in_coro.promise();
                if(!task.is_done())
                {
                    promise.set_ready_func([this] { return task.is_done(); });
                    return true;
                }

                return false;
            }            


        protected:
            auto get_internal_task() const
            {
                return task.get_internal_task();
            }
            task_type task;
        };
    }

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType, typename promise_type>
    struct task_awaiter : public task_awaiter_base<ReturnType, RefType, ResumeType, promise_type>
    {
        using task_awaiter_base<ReturnType, RefType, ResumeType, promise_type>::task_awaiter_base;

        template<typename U = ReturnType, typename std::enable_if_t<!std::is_void_v<U>>* = nullptr>
        auto await_resume()
        {
            //PRECONDITION: task has return value;
            auto retval = this->task.take_return_value();
            return std::move(retval.value());
        }
        template<typename U = ReturnType, typename std::enable_if_t<std::is_void_v<U>>* = nullptr>
        auto await_resume()
        {}
    };
}
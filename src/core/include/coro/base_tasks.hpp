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
#include "coro/task.hpp"
#include "types.hpp"
#include "time.hpp"

namespace raoe::coro
{       
    inline task<> wait_until(TaskReadyFunc ready_func)
    {
        co_await ready_func;
    }

    inline task<> wait_while(TaskReadyFunc ready_func)
    {
        co_await [&ready_func]() {return !ready_func(); };
    }

    inline task<> wait_forever()
    {
        return wait_until([]() { return false; });
    }

    inline task<> wait_count(int32 count)
    {
        for(int i = 0; i < count; i++)
        {
            co_await std::suspend_always();
        }
    }

    
    inline task<raoe::core::time::time_s> wait_seconds(raoe::core::time::time_s time, raoe::core::time::time_func time_func)
    {
        //PRECONDITION: in_time_func is true;

        auto start_time = time_func();
        auto is_time_up = [time_func, time, start_time]() -> bool {
            return raoe::core::time::time_since(start_time, time_func) >= time;
        };

        co_await is_time_up;
        co_return time_func() - start_time - time;
    }

    

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto add_stop_task(task<ReturnType, RefType, ResumeType>& in_task)
    {
        return await::add_stop_task_awaiter(in_task);
    }

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto remove_stop_task(task<ReturnType, RefType, ResumeType>& in_task)
    {
        return await::remove_stop_task_awaiter(in_task);
    }

    inline namespace _
    {    
        inline task<bool> cancel_if_impl(task<> in_task, TaskCancelFunc in_cancel_fn)
        {
            co_await add_stop_task(in_task);

            while(true)
            {
                if(in_cancel_fn && in_cancel_fn())
                {
                    co_return false;
                }
                auto task_status = in_task.resume();
                if(task_status == ETaskStatus::Done)
                {
                    co_return true;
                }

                co_await std::suspend_always();
            }
            co_return false;
        }
    }

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto cancel_task_if(task<ReturnType, RefType, ResumeType>&& in_task, TaskCancelFunc in_cancel_func)
    {
        static_assert(RefType == ETaskRef::Strong && ResumeType == ETaskResumable::Yes, "Cannot call cancel_if on weak_task, task_handle, or weak_task_handle");
        return cancel_if_impl(std::move(in_task), in_cancel_func);
    }

    template<typename ReturnType>
    auto timeout(task<ReturnType>&& task, raoe::core::time::time_s time, raoe::core::time::time_func time_func)
    {
        auto is_time_up = [time_func, start_time = time_func(), time] () -> bool {
            return raoe::core::time::time_since(start_time, time_func) >= time;
        };
        return cancel_task_if(std::move(task), is_time_up);
    }

    inline namespace _ 
    {
        task<bool> stop_if_impl(task<> in_task, TaskCancelFunc cancel_func, 
            std::optional<raoe::core::time::time_s> timeout = {}, raoe::core::time::time_func time_func = nullptr)
        {
            co_await add_stop_task(in_task);

            while(true)
            {
                if(!in_task.is_stop_requested() && cancel_func && cancel_func())
                {
                    in_task.request_stop();
                    if(timeout)
                    {
                        co_return co_await raoe::coro::timeout(std::move(in_task), timeout.value(), time_func);
                    }
                }
                auto task_status = in_task.resume();
                if(task_status == ETaskStatus::Done)
                {
                    co_return true;
                }
                co_await std::suspend_always();
            }
            co_return false;
        }

        template<typename ReturnType>
        task<std::optional<ReturnType>> stop_if_impl(task<ReturnType> task, TaskCancelFunc cancel_func,
            std::optional<raoe::core::time::time_s> timeout = {}, raoe::core::time::time_func time_func = nullptr)
        {            
            co_await add_stop_task(task);

            while(true)
            {
                if(!task.is_stop_requested() && cancel_func && cancel_func())
                {
                    task.request_stop();
                    if(timeout.has_value())
                    {
                        co_return co_await timeout(std::move(task), timeout.value(), time_func);
                    }
                }
                auto task_status = task.resume();
                if(task_status == ETaskStatus::Done)
                {
                    co_return task.take_return_value();
                }
                co_await std::suspend_always();
            }
        }
    }
    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto stop_task_if(task<ReturnType, RefType, ResumeType>&& in_task, TaskCancelFunc cancel_func)
    {
        return stop_if_impl(std::move(in_task), cancel_func);
    }
    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto stop_task_if(task<ReturnType, RefType, ResumeType>&& in_task, TaskCancelFunc cancel_func,
        raoe::core::time::time_s time, raoe::core::time::time_func time_func)
    {
        return stop_if_impl(std::move(in_task), cancel_func, time, time_func);
    }
}
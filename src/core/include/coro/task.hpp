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
#include <type_traits>
#include "coro/task_shared.hpp"
#include "coro/task_private.hpp"
#include "time.hpp"

namespace raoe::coro
{   

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto cancel_task_if(task<ReturnType, RefType, ResumeType>&& in_task, TaskCancelFunc in_cancel_func);

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto stop_task_if(task<ReturnType, RefType, ResumeType>&& in_task, TaskCancelFunc cancel_func);

    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType>
    auto stop_task_if(task<ReturnType, RefType, ResumeType>&& in_task, TaskCancelFunc cancel_func,
        raoe::core::time::time_s time, raoe::core::time::time_func time_func);

    template<typename ReturnType = void, ETaskRef RefType = ETaskRef::Strong, ETaskResumable ResumableType = ETaskResumable::Yes>
    class task
    {
    public:
        using task_private = task_private<ReturnType>;
        using promise_type = promise<ReturnType>;
        static_assert(RefType == ETaskRef::Strong || std::is_void_v<ReturnType>, "Illegal Task Type (cannot have a weak ref with a non-void return type)");

        //Invalid handle constructors
        task() { }
        task(nullptr_t) {}

        task(std::shared_ptr<task_private> in_internal_task)
            : _private_task(in_internal_task)
        {
            add_ref();
        }

        task(std::coroutine_handle<promise_type> in_corohandle)
            : _private_task(std::make_shared<task_private>(in_corohandle))
        {
            add_ref();
        }
        task(const task& other_task)
            : task(other_task._private_task)
        {
            static_assert(is_copyable(), "Cannot copy-construct task or weak_task.  try task_handle or weak_task_handle");
            add_ref();
        }

        task(task&& other_task)
            : _private_task(std::move(other_task._private_task))
        {
        }

        task& operator=(nullptr_t) noexcept
        {
            remove_ref();
            private_task = nullptr;
            return *this;
        }

        task& operator=(const task& other_task)
        {
            static_assert(is_copyable(), "Cannot copy task or weak_task.  try task_handle or weak_task_handle");
            remove_ref();
            private_task = other_task.private_task;
            add_ref();
            return *this;
        }

        task&& operator=(task&& other_task)
        {
            terminate_if_resumable();
            remove_ref();
            private_task = std::move(other_task._private_task);

            return *this;
        }

        ~task()
        {
            remove_ref();
            terminate_if_resumable();
        }

        bool is_valid() const { return _private_task.get(); }
        operator bool() const{ return is_valid(); }
        bool is_done() const { return is_valid() ? private_task()->done() : true; }
        bool is_stop_requested() const { return is_valid() ? private_task()->stop_requested() : true; }
        static constexpr bool is_resumable() { return ResumableType == ETaskResumable::Yes; }
        static constexpr bool is_strong() { return RefType == ETaskRef::Strong; }
        static constexpr bool is_copyable() { return !is_resumable(); }

        //gracefully requests that the coro stop.
        void request_stop()
        {
            if(is_valid())
            {
                private_task()->request_stop();
            }
        }
        //Immediately terminates the coroutine and all of it's local variables
        void terminate()
        {
            if(is_valid())
            {
                _private_task->terminate();
            }
        }

        template<typename U = ReturnType, typename std::enable_if_t<!std::is_void_v<U>>* = nullptr>
        std::optional<ReturnType> take_return_value()
        {
            //PRECONDITION valid()
            if(is_valid())
            {
                return private_task()->take_return_value();
            }
            return {};
        }

        ETaskStatus resume()
        {
            static_assert(is_resumable(), "Cannot call resume on a non-resumable task");
            return is_valid() ? _private_task->resume() : ETaskStatus::Done;
        }

        //TASK CONVERSIONS

        template<typename OtherReturnType>
        operator task<OtherReturnType>() const &
        {
            constexpr bool legal_return_type_conv = std::is_void_v<OtherReturnType> || std::is_same_v<ReturnType, OtherReturnType>;
            constexpr bool legal_type_conv = is_strong() && is_resumable();
            static_assert(legal_type_conv, "Cannot promote weak task, task handle, or weak task handle to a task");
            static_assert(!legal_type_conv || legal_return_type_conv, "mismatched return type conversion");
            static_assert(!legal_type_conv || !legal_return_type_conv, "cannot copy task -> task because it's non-copyable (try std::move(task))");

            return {};
        }
        template<typename OtherReturnType>
        operator task<OtherReturnType>() const &&
        {
            constexpr bool legal_return_type_conv = std::is_void_v<OtherReturnType> || std::is_same_v<ReturnType, OtherReturnType>;
            constexpr bool legal_type_conv = is_strong() && is_resumable();
            static_assert(legal_type_conv, "Cannot promote weak task, task handle, or weak task handle to a task");
            static_assert(!legal_type_conv || legal_return_type_conv, "mismatched return type conversion");

            return move_to_task<OtherReturnType, RefType, ResumableType>();
        }

        operator weak_task() const &
        {
            static_assert(is_resumable(), "Cannot convert task_handle -> weak_task (invalid resumablility conversion");
            static_assert(!is_resumable(), "Cannot convert task -> weak_task because it's non-copyable (try std::move(task))" );
            return {};
        }

        operator weak_task() const &&
        {
            static_assert(is_resumable(), "cannot convert task_handle -> weak_task (invalid resumablility conversion)");
            return move_to_task<void, ETaskRef::Weak, ETaskResumable::Yes>();
        }

        operator task_handle<ReturnType>() const
        {
            static_assert(is_strong(), "cannot convert a weak_task or weak_task_handle -> task_handle (invalid reference strength conversion)");
            return copy_to_task<ReturnType, ETaskRef::Strong, ETaskResumable::No>();
        }

        template<typename OtherReturnType>
        operator task_handle<OtherReturnType>() const
        {
            constexpr bool legal_return_type_conv = std::is_void_v<OtherReturnType> || std::is_same_v<ReturnType, OtherReturnType>;
            static_assert(is_strong(), "Cannot convert weak_task or weak_task_handle -> task_handle (invalid ref strength conversion)");
            static_assert(!is_strong() || legal_return_type_conv, "invalid return type conversion");

            return copy_to_task<OtherReturnType, ETaskRef::Strong, ETaskResumable::No>();
        }

        operator weak_task_handle() const
        {
            return copy_to_task<void, ETaskRef::Weak, ETaskResumable::No>();
        }

        auto cancel_if(TaskCancelFunc cancel_func) &&
        {
            return cancel_task_if(std::move(*this), cancel_func);
        }
        auto cancel_if_stop_requested() &&
        {
            return std::move(*this).cancel_if([]{return is_stop_requested(); });
        }
        auto cancel_if(TaskCancelFunc cancel_func) &
        {
            static_assert(static_false<ReturnType>::value, "Cannot call cancel_if() on a lvalue (try std::move(task).cancel_if())");
            return cancel_task_if(std::move(*this), cancel_func);
        }
        auto cancel_if_stop_requested() & 
        {
            static_assert(static_false<ReturnType>::value, "Cannot call cancel_if_stop_requested() on a lvalue (try std::move(task).cancel_if())");            
            return std::move(*this).cancel_if([]{return is_stop_requested(); });
        }

        auto stop_if(TaskCancelFunc cancel_func) &&
        {
            return stop_task_if(std::move(*this), cancel_func);
        }

        auto stop_if(TaskCancelFunc cancel_func) &
        {
            static_assert(static_false<ReturnType>::value, "Cannot call stop_if on lvalue (try std::move(task).stop_if())");
            return stop_task_if(std::move(*this), cancel_func);
        }

        auto stop_if(TaskCancelFunc cancel_func, raoe::core::time::time_s time, raoe::core::time::time_func time_func) &&
        {
            return stop_task_if(std::move(*this), cancel_func, time, time_func);
        }

        auto stop_if(TaskCancelFunc cancel_func, raoe::core::time::time_s time, raoe::core::time::time_func time_func) &
        {
            static_assert(static_false<ReturnType>::value, "Cannot call stop_if on lvalue (try std::move(task).stop_if())");
            return stop_task_if(std::move(*this), cancel_func, time, time_func);
        }



        friend class _::task_private_base;
        template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType, typename awaiter_promise_type> friend struct await::_::task_awaiter_base;
    private:
        std::shared_ptr<_::task_private_base> _private_task;

        std::shared_ptr<task_private> private_task() const { return std::static_pointer_cast<task_private>(_private_task); }

        void terminate_if_resumable()
        {
            if(is_resumable() && is_valid())
            {
                terminate();
            }
        }
        void add_ref()
        {
            if(is_valid() && is_strong())
            {                
                _private_task->add_ref();                
            }
        }
        void remove_ref()
        {
            if(is_valid() && is_strong())
            {                
                _private_task->remove_ref();
            }
        }


        template<typename NewReturnType, ETaskRef NewRefType, ETaskResumable NewResumable>
        task<NewReturnType, NewRefType, NewResumable> copy_to_task() const
        {
            task<NewReturnType, NewRefType, NewResumable> ret;
            ret._private_task = _private_task;
            ret.add_ref();
            return ret;
        }

        template<typename NewReturnType, ETaskRef NewRefType, ETaskResumable NewResumable>
        task<NewReturnType, NewRefType, NewResumable> move_to_task() const
        {
            task<NewReturnType, NewRefType, NewResumable> ret;
            ret._private_task = _private_task;
            ret.add_ref();
            remove_ref();
            _private_task = nullptr;
            return ret;
        }
    };



}
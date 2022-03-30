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

#include <functional>

namespace raoe::coro
{
    enum class ETaskRef
    {
        Strong,
        Weak,
    };

    enum class ETaskResumable
    {
        Yes,
        No,
    };

    enum class ETaskStatus
    {
        Suspended,
        Done,
    };

   
    inline namespace _
    {
        class task_private_base;
        
    }
    namespace await
    {
        inline namespace _
        {
            template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumeType, typename promise_type> struct task_awaiter_base;
        }        
    }

    template<typename ReturnType> struct promise;
    template<typename ReturnType, ETaskRef RefType, ETaskResumable ResumableType> class task;    
    template<typename ReturnType = void>
    using task_handle = task<ReturnType, ETaskRef::Strong, ETaskResumable::No>;
    using weak_task = task<void, ETaskRef::Weak, ETaskResumable::Yes>;
    using weak_task_handle = task<void, ETaskRef::Weak, ETaskResumable::No>;

    using TaskReadyFunc = std::function<bool()>;
    using TaskCancelFunc = std::function<bool()>;

    struct stop_context
    {
        operator bool() const { return *stopped_ptr; }
    private:
        friend class _::task_private_base;
        stop_context(const bool* in_stopped_ptr)
            : stopped_ptr(in_stopped_ptr)
        {            
        }

        const bool* stopped_ptr = nullptr;
    };
    
    struct get_stop_context {};

}
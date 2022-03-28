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
}
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

#include <gtest/gtest.h>

#include "coro/coro.hpp"

TEST(CoroTest, InitialTest)
{
    raoe::coro::promise<int> p;

    raoe::coro::task<void> task_v;
    raoe::coro::task<int> task_t;
}

TEST(CoroTest, Counter)
{
    auto l = []() -> raoe::coro::task<> {
        for(int i = 0; i < 10; i++)
        {
            std::cout<< i << std::endl;
            co_await std::suspend_always();
        }
    };

    auto task = l();
    while(!task.is_done())
    {
        task.resume();
    }
}

TEST(CoroTest, Cancel)
{
     auto l = []() -> raoe::coro::task<> {
        for(int i = 0; i < 10; i++)
        {
            std::cout<< i << std::endl;
            co_await std::suspend_always();
        }
    };

    int32 i = 0;
    auto task = l().cancel_if([&i] {return i >= 5; });

    while(!task.is_done())
    {
        task.resume();
        i++;        
    }
}

TEST(CoroTest, StopIf)
{
    auto l = []() -> raoe::coro::task<> {
        for(int i = 0; i < 10; i++)
        {
            std::cout<< i;
            co_await std::suspend_always();
        }
    };

    int32 i = 0;
    auto task = l().stop_if([&i] {return i >= 5; });

    while(!task.is_done())
    {
        task.resume();
        i++;    
        std::cout << " Stop Requested: " << task.is_stop_requested() << std::endl;    
    }
}

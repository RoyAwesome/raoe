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
#include <gmock/gmock.h>

#include "spdlog/spdlog.h"
#include "lazy.hpp"

raoe::lazy<int> f(int x)
{
    co_return x;
}

raoe::lazy<int> sum(int x)
{
    if(x <= 0) 
    {
        co_return co_await f(0);
    }
    else 
    {
        co_return co_await f(x) + co_await sum(x-1);
    }
}

TEST(lazy, ReturnIntegerInput)
{
    EXPECT_EQ(f(42).sync_await(), 42);
}

TEST(lazy, SumCoro)
{
    EXPECT_EQ(sum(42).sync_await(), 42 * 43 / 2);
}

raoe::lazy<void> basic_handle_test(int value)
{
    int s = co_await sum(value);

    spdlog::info("Sum: {} ", s);
}

TEST(lazy, basic_coro)
{
    EXPECT_NO_THROW(basic_handle_test(42).sync_await());
}
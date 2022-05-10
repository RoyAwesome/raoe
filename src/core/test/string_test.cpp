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

#include "core.hpp"


using namespace std::literals::string_view_literals;

TEST(StringViewOperations, trim_r)
{
    EXPECT_EQ(raoe::string::trim_r("asdf    "sv), "asdf"sv);
}

TEST(StringViewOperations, trim_l)
{
    EXPECT_EQ(raoe::string::trim_l("    asdf"sv), "asdf"sv);
}

TEST(StringViewOperations, trim)
{
    EXPECT_EQ(raoe::string::trim("    asdf       "sv), "asdf"sv);
}

TEST(StringViewOperations, split)
{
    std::string_view arr = "hello my name is jeff"sv;
    std::array<std::string_view, 5> container;

    raoe::string::split(arr, " "sv, container.begin());

    EXPECT_EQ(container[0], "hello"sv);
    EXPECT_EQ(container[1], "my"sv);
    EXPECT_EQ(container[2], "name"sv);
    EXPECT_EQ(container[3], "is"sv);
    EXPECT_EQ(container[4], "jeff"sv);
}

TEST(StringViewOperations, token)
{
    EXPECT_EQ(raoe::string::token("these words of power"sv, " "sv), "these"sv);
}
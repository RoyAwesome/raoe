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

#include "parse.hpp"

using namespace std::literals::string_view_literals;

//Line tests


//One Argument tests a simple string.  The input string should be the only element in the output array
TEST(LineParse, TestOneArgument)
{
    std::string_view input_string = "one"sv;

    auto results = raoe::core::parse::_::parse_split(input_string);
    auto matching_vector = std::vector<std::string_view>{"one"sv};
    EXPECT_EQ(results, matching_vector);
}

//Take in a string with only whitespace, ensure that the return value is both elements
TEST(LineParse, TwoArguments_OnlyWhitespace)
{
    std::string_view input_string = "one two"sv;
    auto results = raoe::core::parse::_::parse_split(input_string);
    auto matching_vector = std::vector<std::string_view>{"one"sv, "two"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, TenArguments_OnlyWhitespace)
{
    std::string_view input_string = "one two three four five 6 7 eight 9 ten"sv;
    auto results = raoe::core::parse::_::parse_split(input_string);
    auto matching_vector = std::vector<std::string_view>{"one"sv, "two"sv, "three"sv, "four"sv, "five"sv, "6"sv, "7"sv, "eight"sv, "9"sv, "ten"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, TwoArguments_WhitespaceStart_OnlyWhitespace)
{
    std::string_view input_string = " one two"sv;
    auto results = raoe::core::parse::_::parse_split(input_string);
    auto matching_vector = std::vector<std::string_view>{"one"sv, "two"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, TwoArguments_LotsOfWhitespaceBewtween_OnlyWhitespace)
{
    std::string_view input_string = "one       two"sv;
    auto results = raoe::core::parse::_::parse_split(input_string);

    auto matching_vector = std::vector<std::string_view>{"one"sv, "two"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, TwoArguments_WhitespaceStart_WhitespaceEnd_OnlyWhitespace)
{
    std::string_view input_string = " one two "sv;
    auto results = raoe::core::parse::_::parse_split(input_string);
        auto matching_vector = std::vector<std::string_view>{"one"sv, "two"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, OneArgument_Quotes)
{
    std::string_view input_string = "\"one\""sv;
    auto results = raoe::core::parse::_::parse_split(input_string);

    auto matching_vector = std::vector<std::string_view>{"one"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, OneArgument_InQuotes_WhitespaceInQuotes)
{
    std::string_view input_string = "\"one two\""sv;
    auto results = raoe::core::parse::_::parse_split(input_string);
    auto matching_vector = std::vector<std::string_view>{"one two"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, Two_InQuotes_WhitespaceInQuotes_SecondArgument_WithWhitespace)
{
    std::string_view input_string = "\"one two\" three"sv;
    auto results = raoe::core::parse::_::parse_split(input_string);   

    auto matching_vector = std::vector<std::string_view>{"one two"sv, "three"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, Two_InQuotes_EscapedQuoteInQuotes)
{
    std::string_view input_string = "\"one \\\"two\""sv;
    auto results = raoe::core::parse::_::parse_split(input_string);

    auto matching_vector = std::vector<std::string_view>{"one \\\"two"sv};
    EXPECT_EQ(results, matching_vector);
}

TEST(LineParse, ThreeArguments_FirstAndThirdInQuotes_WhitespaceBetween)
{
    std::string_view input_string = "\"one two\" three \"four five\""sv;
    auto results = raoe::core::parse::_::parse_split(input_string);

    auto matching_vector = std::vector<std::string_view>{"one two"sv, "three"sv, "four five"sv};
    EXPECT_EQ(results, matching_vector);
}


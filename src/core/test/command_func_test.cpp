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
#include "types.hpp"
#include "spdlog/fmt/bundled/ranges.h"

#include "parse.hpp"

using namespace std::literals::string_view_literals;



void foo(int i, float f, std::string s)
{
    std::cout << fmt::format("Foo Called: {} {} {}", i, f, s) << std::endl;
}


class console_command_base
{
    console_command_base() = delete;
protected:
    console_command_base(std::string in_name, int num_args)
    : m_name(in_name)
    , m_num_arguments(num_args)
    {

    }
public:
    std::string_view name() const { return m_name; }
    int num_args() const { return m_num_arguments; }

    virtual bool execute(std::string_view command_line) const = 0;
private:   
    std::string m_name;
    int m_num_arguments;
};


template<typename... Args>
class console_command : public console_command_base
{
public:
    console_command(std::string name, void(*in_func)(Args...))
        : console_command_base(name, raoe::core::count_args(in_func))
        , func(in_func)
    {

    }
    virtual bool execute(std::string_view command_line) const
    {
        std::tuple<Args...> parsed_arguments = raoe::core::parse::parse_tuple<Args...>(command_line);
       
        std::apply(func, parsed_arguments);
        return true;
    }
private:
    std::function<void(Args...)> func;
};

template<typename... Args>
std::unique_ptr<console_command_base> make_console_command(std::string_view command_name, void(*func)(Args...))
{
    return std::unique_ptr<console_command_base>(new console_command<Args...>(std::string(command_name), func));
}

TEST(CommandFuncTest, countargs)
{
    EXPECT_EQ(raoe::core::count_args(foo), 3);
}

TEST(CommandFuncTest, console_command_struct)
{
    std::unique_ptr<console_command_base> command = make_console_command("foo", foo);
    EXPECT_EQ(command->num_args(), 3);

    EXPECT_EQ(command->execute("foo 12 3.14 booyah"), true);

}
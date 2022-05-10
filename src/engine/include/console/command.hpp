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

#include <string>
#include <functional>
#include <vector>
#include "console.hpp"
#include "from_string.hpp"
#include "parse.hpp"

namespace RAOE::Console
{
    class BaseCommand : public IConsoleElement
    {        
        BaseCommand() = delete;
    protected:
        BaseCommand(
            std::string_view in_command_name, 
            std::string_view in_description, 
            EConsoleFlags in_flags = EConsoleFlags::None
        )
        : m_name(in_command_name)
        , m_description(in_description)
        , m_flags(in_flags)
        {            
        }

    public:
        virtual std::string_view name() const override { return m_name; }
        virtual std::string_view description() const override { return m_description; }
        virtual EConsoleFlags flags() const override { return m_flags; }
        virtual EExecuteError execute(std::string_view command_line) const = 0;

        std::string m_name;
        std::string m_description;
        EConsoleFlags m_flags;
    };

    class ConsoleCommandNoArgs : public BaseCommand
    {
    public:
        ConsoleCommandNoArgs() = delete; //Cannot construct a default console command

        ConsoleCommandNoArgs(
            std::string_view in_command_name, 
            std::string_view in_description, 
            std::function<void()> in_functor, 
            EConsoleFlags in_flags = EConsoleFlags::None
        )
            : BaseCommand(in_command_name, in_description, in_flags)
            , functor(in_functor)
        {
        }

        virtual EExecuteError execute(std::string_view command_line) const
        {
            functor();
            return EExecuteError::Success;
        }
    private:
        std::function<void()> functor;
    };

    class ConsoleCommandAllArgs : public BaseCommand
    {
    public:
        ConsoleCommandAllArgs() = delete; //Cannot construct a default console command

        ConsoleCommandAllArgs(
            std::string_view in_command_name, 
            std::string_view in_description, 
            std::function<void(std::string_view)> in_functor, 
            EConsoleFlags in_flags = EConsoleFlags::None
        )
            : BaseCommand(in_command_name, in_description, in_flags)
            , functor(in_functor)
        {
        }

        virtual EExecuteError execute(std::string_view command_line) const
        {
            functor(command_line);
            return EExecuteError::Success;
        }
    private:
        std::function<void(std::string_view)> functor;
    };

    template<typename... Args>
    class ConsoleCommandWithArgs : public BaseCommand
    {
    public:
        ConsoleCommandWithArgs() = delete; //Cannot construct a default console command

        ConsoleCommandWithArgs(
            std::string_view in_command_name, 
            std::string_view in_description, 
            std::function<void(Args...)> in_functor, 
            EConsoleFlags in_flags = EConsoleFlags::None
        )
            : BaseCommand(in_command_name, in_description, in_flags)
            , functor(in_functor)
        {
        }

        virtual EExecuteError execute(std::string_view command_line) const
        {
            std::tuple<Args...> parsed_arguments = raoe::core::parse::parse_tuple<Args...>(command_line);       
            std::apply(functor, parsed_arguments);
            return EExecuteError::Success;
        }
    private:
        std::function<void(Args...)> functor;
    };

    struct AutoRegisterConsoleCommand : public AutoRegisterConsoleElement
    {
        AutoRegisterConsoleCommand(BaseCommand* in_command)
            : AutoRegisterConsoleElement(in_command)
        {
        }

        BaseCommand* GetConsoleCommand() { return static_cast<BaseCommand*>(console_element()); }
    };

    template<typename... Args>
    AutoRegisterConsoleCommand CreateConsoleCommandWithArgs(std::string_view name,
        std::string_view description,
        void(*functor)(Args...), 
        EConsoleFlags flags = EConsoleFlags::None
    )
    {
        BaseCommand* cmd = static_cast<BaseCommand*>(CommandRegistry::Get().register_console_element([=]() -> IConsoleElement*
            {
                return new ConsoleCommandWithArgs<Args...>(
                    name,
                    description,
                    functor,
                    flags
                );
            }           
        ));

        return AutoRegisterConsoleCommand(cmd);
    }

   
    inline AutoRegisterConsoleCommand CreateConsoleCommand(std::string_view name,
        std::string_view description,
        void(*functor)(std::string_view), 
        EConsoleFlags flags = EConsoleFlags::None
    )
    {
        BaseCommand* cmd = static_cast<BaseCommand*>(CommandRegistry::Get().register_console_element([=]() -> IConsoleElement*
            {
                return new ConsoleCommandAllArgs(
                    name,
                    description,
                    functor,
                    flags
                );
            }           
        ));

        return AutoRegisterConsoleCommand(cmd);
    }


    inline AutoRegisterConsoleCommand CreateConsoleCommand(std::string_view name,
        std::string_view description,
        std::function<void()> functor, 
        EConsoleFlags flags = EConsoleFlags::None
    )
    {
        BaseCommand* cmd = static_cast<BaseCommand*>(CommandRegistry::Get().register_console_element([=]() -> IConsoleElement*
            {
                return new ConsoleCommandNoArgs(
                    name,
                    description,
                    functor,
                    flags
                );
            }           
        ));

        return AutoRegisterConsoleCommand(cmd);
    }
}
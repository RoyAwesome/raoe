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

namespace RAOE::Console
{
    using CommandFunctor = void(const CommandArgs&);

    struct Command : public IConsoleElement
    {        
        Command(
            std::string_view in_command_name, 
            std::string_view in_description, 
            std::function<CommandFunctor> in_functor, 
            EConsoleFlags in_flags = EConsoleFlags::None
        )
        : m_name(in_command_name)
        , m_description(in_description)
        , m_functor(in_functor)
        , m_flags(in_flags)
        {            
        }

        virtual std::string_view name() const override { return m_name; }
        virtual std::string_view description() const override { return m_description; }
        virtual EConsoleFlags flags() const override { return m_flags; }
        virtual EExecuteError execute(const CommandArgs& args) const
        {
            m_functor(args);
            return EExecuteError::Success;
        }

        std::string m_name;
        std::string m_description;
        std::function<CommandFunctor> m_functor;
        EConsoleFlags m_flags;
    };

    struct AutoRegisterConsoleCommand : public AutoRegisterConsoleElement
    {
        AutoRegisterConsoleCommand(Command* in_command)
            : AutoRegisterConsoleElement(in_command)
        {
        }

        Command* GetConsoleCommand() { return static_cast<Command*>(console_element()); }
    };

    AutoRegisterConsoleCommand CreateConsoleCommand(std::string_view name,
        std::string_view description,
        std::function<CommandFunctor> functor, 
        EConsoleFlags flags = EConsoleFlags::None
    );

}
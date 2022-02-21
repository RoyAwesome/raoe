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
     class CommandProcessor;

    struct ConsoleHandle
    {
        ConsoleHandle(IConsoleElement* in_console_element)
            : element(in_console_element)
        {
        }
    private:
        IConsoleElement* element;
    };

    
    struct CommandArgs
    {
        std::weak_ptr<CommandProcessor> command_processor;
        std::vector<std::string> args;
        std::string args_as_string;
    };
    using CommandFunctor = void(const CommandArgs&);

    struct Command : public IConsoleElement
    {
        
        Command(std::string_view command_name, std::string_view description, std::function<CommandFunctor>&& functor, EConsoleFlags flags = EConsoleFlags::None);

        std::string name;
        std::string description;
        std::function<CommandFunctor> functor;
        EConsoleFlags flags;
    };

}
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

#include "console/console.hpp"
#include "console/command.hpp"

#include <optional>
#include "string.hpp"

namespace RAOE::Console
{
    static std::optional<CommandRegistry>& registry_singleton()
    {
        static std::optional<CommandRegistry> singleton(std::in_place);
        return singleton;
    }
    CommandRegistry& CommandRegistry::Get()
    {
        return registry_singleton().value();
    }
  
    IConsoleElement* CommandRegistry::register_console_element(std::function<IConsoleElement*()> factory)    
    {
        std::unique_ptr<IConsoleElement>& element = element_registry.emplace_back(std::unique_ptr<IConsoleElement>(factory()));
        return element.get();
    }

    std::tuple<std::string_view, std::string_view> split_command_args(std::string_view command_line)
    {
        const std::string_view start = raoe::string::token(command_line, " ");
        return { start, command_line.substr(start.length())};
    }

    EConsoleError execute(std::string_view command_line)    
    {
        //split this command into it's name and args
        auto [command, args] = split_command_args(command_line);
     
        IConsoleElement* found_element = nullptr;

        //Find the console command this matches
        for(auto& element : RAOE::Console::CommandRegistry::Get().elements())
        {
            if(element && element->name().compare(command) == 0)
            {
                found_element = element.get();
                break;                
            }
        }

        if(found_element)
        {
            if(IConsoleElement::EExecuteError error = found_element->execute(args); error != IConsoleElement::EExecuteError::Success )
            {
                return EConsoleError::Incorrect_Arguments;
            }
           
            return EConsoleError::None;
        }

        return EConsoleError::Command_Not_Found;
    }
}
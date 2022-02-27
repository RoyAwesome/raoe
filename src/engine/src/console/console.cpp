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

    AutoRegisterConsoleCommand CreateConsoleCommand(std::string_view name,
        std::string_view description,
        std::function<CommandFunctor> functor, 
        EConsoleFlags flags
    )
    {
        Command* cmd = static_cast<Command*>(CommandRegistry::Get().register_console_element([=]() -> IConsoleElement*
            {
                return new Command(
                    name,
                    description,
                    functor,
                    flags
                );
            }           
        ));

        return AutoRegisterConsoleCommand(cmd);
    }

    IConsoleElement* CommandRegistry::register_console_element(std::function<IConsoleElement*()> factory)    
    {
        std::unique_ptr<IConsoleElement>& element = element_registry.emplace_back(std::unique_ptr<IConsoleElement>(factory()));
        return element.get();
    }

    EConsoleError execute(std::string_view command_line)    
    {
        //Parse this command into the it's constituent parts
        std::vector<std::string> elements;
        raoe::core::split(std::string(command_line), ' ', std::back_inserter(elements));

        CommandArgs args;

        IConsoleElement* found_element = nullptr;

        //Find the console command this matches
        for(auto& element : RAOE::Console::CommandRegistry::Get().elements())
        {
            if(element && element->name().compare(elements[0]) == 0)
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
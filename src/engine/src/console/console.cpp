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
}
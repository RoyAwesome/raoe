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

#include <optional>

namespace RAOE::Console
{
/*
    Command::Command(
        std::string_view in_command_name, 
        std::string_view in_description, 
        std::function<CommandFunctor>&& in_functor, 
        ECommandFlags in_flags
    )    
    : name(in_command_name)
    , description(in_description)
    , functor(in_functor)
    , flags(in_flags)
    {
        CommandRegistry::Get().register_command(*this);
    }

    static std::optional<CommandRegistry>& command_registry_singleton()
    {
        static std::optional<CommandRegistry> singleton(std::in_place);
        return singleton;
    }

    CommandRegistry& CommandRegistry::Get()    
    {
        return command_registry_singleton().value();
    }

    void CommandRegistry::register_command(Command& command)
    {
        command_registry.push_back(std::move(command));
    }
*/
}
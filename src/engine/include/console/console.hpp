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

namespace RAOE::Console
{
    enum class EConsoleFlags
    {
        None            = 0,
        Cheat           = 1 << 1, //This object is a cheat, and must have cheat privledges to execute
        Replicated      = 1 << 2, //This object is replicated, and all objects in the network execute it
        Authority       = 1 << 3, //this object is only able to be executed on authority (cvars can only be set by auth, commands can only start on auth)

    };

    class IConsoleElement
    {
        virtual std::string_view name() const = 0;
        virtual std::string_view description() const = 0;
        virtual EConsoleFlags flags() const = 0;
    };

   
   

    struct AutoRegisterConsoleElement
    {
       

       private:
       IConsoleElement* constructed_element;
    };

    class CommandRegistry
    {
    public:
        static CommandRegistry& Get();
        friend struct Command;



        const std::vector<std::unique_ptr<IConsoleElement>>& commands() const { return element_registry; }    
    private:        
        std::vector<std::unique_ptr<IConsoleElement>> element_registry;
    };
          
}
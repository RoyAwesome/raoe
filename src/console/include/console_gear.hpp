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

#include "cogs/gear.hpp"
#include "display_console.hpp"

namespace RAOE::Gears
{
    struct ConsoleGear : public RAOE::Cogs::Gear
    {      
        ConsoleGear(RAOE::Cogs::BaseCog&, std::string_view);

        RAOE::Console::DisplayConsole* display_console() { return console_ptr.get(); }
        
    private:
        std::unique_ptr<RAOE::Console::DisplayConsole> console_ptr;
    };
}
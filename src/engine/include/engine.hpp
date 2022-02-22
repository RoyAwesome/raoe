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

#include <functional>
#include "core.hpp"
#include "cogs/gear.hpp"

#pragma once

int main(int, char**);

namespace RAOE
{
    class Engine
    {     
    protected:
        //Main calls Init and Run (but nobody else can).  So it's friend.  
        friend int ::main(int, char**);
        friend class CogManager;

        static Engine& Init(int32 argc, char* argv[]);
        bool Run();
        void Shutdown();
    };

    namespace _
    {
        struct EngineCog;
    }
    namespace Gears
    {
        /* Engine Gear
            This is a special gear that represents the game engine.  It it initalized first, and is also the last to be destroyed
            It can be accessed by "Engine::Gear"
        */
        extern const std::string EngineGearName;

        class Engine : public RAOE::Cogs::Gear
        {   
        public:         
            friend struct RAOE::_::EngineCog;

            void request_exit() { exit_requested = true; }
            bool should_exit() const { return exit_requested; }

            void register_tickfunc(std::function<void()>&& tickfunc)
            {
                tick_funcs.emplace_back(std::move(tickfunc));
            }

            const std::vector<std::function<void()>>& tickfuncs() { return tick_funcs; }
        private:
            Engine()
            {

            }        

            bool exit_requested = false;

            std::vector<std::function<void()>> tick_funcs;
        };       
    }
}
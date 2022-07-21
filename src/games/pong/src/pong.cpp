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


#include "engine.hpp"
#include "cogs/gear.hpp"
#include "framework.hpp"
#include "resource/resources.hpp"
#include "lazy.hpp"
#include "services/task_service.hpp"

namespace RAOE::Pong
{
    const RAOE::Resource::Tag PongGameTag = RAOE::Resource::Tag("pong:game");

    struct Game : RAOE::Framework::IGame
    {
        Game(RAOE::Engine& in_engine)
            : RAOE::Framework::IGame(in_engine)
        {
        }

        void begin() override
        {
            
        }
    };

    struct Gear : RAOE::Cogs::Gear
    {
        Gear(RAOE::Cogs::BaseCog& in_cog, std::string_view in_name)
            : RAOE::Cogs::Gear(in_cog, in_name) 
        {
            startup_task(RAOE::Resource::emplace_owned_resource<Game>(engine(), PongGameTag, RAOE::Framework::Tags::GameType));
        }

        void activated() override
        {
          
        }
    };

}

RAOE_DEFINE_GEAR(PongGear, RAOE::Pong::Gear)
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

#include "engine_cog.hpp"
#include "flecs_cog.hpp"
#include "sdl_module.hpp"

namespace RAOE::Cogs
{
    using IEngineCog = RAOE::IEngineCog;

    struct SDLFLECSCog : public IEngineCog
    {
        virtual void activated()
        {
            if(auto flecs_cog = CogManager::Get().get_cog<RAOE::Cogs::FlecsCog>().lock())
            {
                flecs_cog->ecs_world_client->import<RAOE::ECS::SDL::Module>();
            }
        }
    };

    REGISTER_COG(sdl_flecs_cog, SDLFLECSCog)
}
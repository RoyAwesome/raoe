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
#include "flecs_gear.hpp"
#include "client_app_module.hpp"
#include "glm/glm.hpp"
#include "frontend.hpp"
#include "frontend_internal.hpp"

namespace RAOE::Frontend
{
    struct Gear : public RAOE::Cogs::Gear
    {
        Gear(RAOE::Cogs::BaseCog& in_cog, std::string_view in_name)
            : RAOE::Cogs::Gear(in_cog, in_name) 
        {}

        void activated() override
        {
            if(auto gear_service = engine().get_service<RAOE::Service::GearService>().lock())
            {
                if(auto flecs_gear = gear_service->get_gear<RAOE::Gears::FlecsGear>().lock() )
                {
                    flecs_gear->ecs_world_client->entity().set<RAOE::ECS::ClientApp::Canvas>({"RAOE", glm::ivec2(1600, 900), glm::i8vec4(0, 0, 0, 0)}); //NOLINT complains about the resolution.
                    flecs_gear->ecs_world_client->import<RAOE::Frontend::Module>();
                    flecs_gear->ecs_world_client->entity().set<RAOE::Frontend::FrontenedPanel>({});
                }
            }
        }
    };

}


RAOE_DEFINE_GEAR(FrontendGear, RAOE::Frontend::Gear)
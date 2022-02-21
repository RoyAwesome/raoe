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

#include "cogs/cog.hpp"
#include "flecs_gear.hpp"
#include "imgui_module.hpp"

namespace RAOE::Gears
{
    struct ImguiSdlFlecsGear : public RAOE::Cogs::Gear
    {
        virtual void activated()
        {
            FlecsGear* flecs_gear = static_cast<FlecsGear*>(RAOE::Cogs::Registry::Get().get_gear(RAOE::Gears::FlecsGearName));
            if(flecs_gear)
            {
                flecs_gear->ecs_world_client->import<RAOE::ECS::Imgui::Module>();
            }
        }
    };
}
RAOE_DEFINE_GEAR(ImguiGear, RAOE::Gears::ImguiSdlFlecsGear)
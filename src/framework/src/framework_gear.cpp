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

#include "framework.hpp"
#include "cogs/cog.hpp"
#include "cogs/gear.hpp"
#include "resource/resources.hpp"

namespace RAOE::Framework::Tags
{
    const RAOE::Resource::Tag GameType = RAOE::Resource::Tag("raoe:type/game");
}

namespace RAOE::Gears
{
    struct FrameworkGear : public RAOE::Cogs::Gear
    {
        FrameworkGear(RAOE::Cogs::BaseCog& in_cog, std::string_view in_name)
            : RAOE::Cogs::Gear(in_cog, in_name)
        {            
        }

        void activated() override
        {
            if(auto resource_service = engine().get_service<RAOE::Resource::Service>().lock())
            {
                resource_service->init_resource_type(RAOE::Framework::Tags::GameType);
            }
        }
    };
}

RAOE_DEFINE_GEAR(FrameworkGear, RAOE::Gears::FrameworkGear)
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
#include "engine_cog.hpp"

#include "flecs.h"

namespace RAOE::Cogs
{
    struct FlecsCog : public IEngineCog
    {
        FlecsCog();
      
        virtual void activated() override;
        virtual void deactivated() override;

        std::unique_ptr<flecs::world> ecs_world_client;
        //std::unique_ptr<flecs::world> ecs_world_server; //TODO: Server World
    };
}
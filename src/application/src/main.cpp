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

#include "engine_cog.hpp"
#include "flecs_cog.hpp"
#include "client_app_module.hpp"

#ifndef RAOE_STATIC_COGS
#define RAOE_STATIC_COGS 1 //TODO: get cmake to produce this.  this should be default 0
#endif

#if RAOE_STATIC_COGS
#include "static_cogs.inl"
#endif

int main(int argc, char* argv[])
{
#if RAOE_STATIC_COGS
    RAOE::_GENERATED::LoadStaticCogs();
#endif

    RAOE::Engine& engine = RAOE::Engine::Init(argc, argv);

    //HACKHACK - Make a window here (TODO: Find a better place to do this)
    if(auto flecs_cog = RAOE::CogManager::Get().get_cog<RAOE::Cogs::FlecsCog>().lock() )
    {
        flecs_cog->ecs_world_client->entity().set<RAOE::ECS::ClientApp::Canvas>({"RAOE", glm::ivec2(640, 480), glm::i8vec4(0, 0, 0, 0)});
    }
    else
    {
        spdlog::warn("Unable to find flecs cog");
    }

    while(engine.Run()) {}
    engine.Shutdown();

    return 0;
}
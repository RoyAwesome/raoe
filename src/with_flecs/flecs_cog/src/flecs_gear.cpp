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

#include "flecs_gear.hpp"
#include "engine.hpp"
#include "client_app_module.hpp"
#include "cogs/cog.hpp"
#include "services/task_service.hpp"
#include "lazy.hpp"

namespace RAOE::Gears
{
    const std::string FlecsGearName("Flecs::ECS::FlecsGear");

    void LogECS(int32 level,  const char *file,  int32 line, const char *msg)
    {
        spdlog::level::level_enum lvl = spdlog::level::trace;
        if(level > 0)
        {
            lvl = spdlog::level::debug;
        }
        else if(level == -2)
        {
            lvl = spdlog::level::warn;
        }
        else if(level == -3)
        {
            lvl = spdlog::level::err;
        }
        else if(level == -4)
        {
            lvl = spdlog::level::critical;
        }

        std::string indentation;
        for(int32 i  = 0; i < ecs_os_api.log_indent_; i++)
        {
            indentation += "\t -";
        }

        spdlog::log(lvl, "|{} ({}:{}) {} ", indentation, file, line, msg);
    }

    void InitFLECSSystem()
    {
        ecs_os_set_api_defaults();
        ecs_os_api_t api = ecs_os_api;
    
        api.log_ = LogECS;
        api.flags_ |= EcsOsApiLogWithColors;
        ecs_os_set_api(&api);

        ecs_log_set_level(0);
        spdlog::set_level(spdlog::level::trace);
    }

    raoe::lazy<> tick_ecs(Engine& engine, const std::unique_ptr<flecs::world>& world_ptr)
    {
        while(!world_ptr)
        {
            if(!world_ptr->progress())
            {
                engine.request_exit();
            }      
            co_await std::suspend_always();      
        }
    }

    FlecsGear::FlecsGear(RAOE::Cogs::BaseCog& in_cog, std::string_view name)   
        : RAOE::Cogs::Gear(in_cog, name)
        , ecs_world_client(std::make_unique<flecs::world>())
    {
        ecs_world_client->set_context(&engine());
        InitFLECSSystem();      
    }  

    void FlecsGear::activated()    
    {        
        if(ecs_world_client)
        {
            ecs_world_client->import<RAOE::ECS::ClientApp::Module>();
        }   
        RAOE::enqueue_task(engine(), tick_ecs(engine(), ecs_world_client));
    }

    void FlecsGear::deactivated()    
    {
    
    }

    const std::unique_ptr<flecs::world>& client_world(RAOE::Engine& engine)    
    {
        if(auto gear_service = engine.get_service<RAOE::Service::GearService>().lock())
        {
            if(auto flecs_gear = gear_service->get_gear<FlecsGear>().lock())
            {
                return flecs_gear->ecs_world_client;
            }
        }        

        static std::unique_ptr<flecs::world> none_world;
        return none_world;    
    }    
}

RAOE_DEFINE_GEAR(FlecsGear, RAOE::Gears::FlecsGear)
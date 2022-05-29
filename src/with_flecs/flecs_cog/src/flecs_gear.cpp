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
#include "services/tick_service.hpp"

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

        std::string indentation = "";
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
        api.log_with_color_ = true;
        ecs_os_set_api(&api);

        ecs_log_set_level(0);
        spdlog::set_level(spdlog::level::trace);
    }


    FlecsGear::FlecsGear(RAOE::Cogs::BaseCog& in_cog)   
        : RAOE::Cogs::Gear(in_cog)
        , ecs_world_client(std::make_unique<flecs::world>())
    {
        ecs_world_client->set_context(&engine());
        InitFLECSSystem();    
    }

    void FlecsGear::activated()    
    {
        if(auto tick_service = engine().get_service<RAOE::Service::TickService>().lock())
        {
            tick_service->register_tickfunc([this]()
            {
                if(ecs_world_client)
                {
                    if(!ecs_world_client->progress())
                    {
                        engine().get_service<RAOE::Service::TickService>().lock()->request_exit();
                    }
                }
            });
        }
       
      

        if(ecs_world_client)
        {
            ecs_world_client->import<RAOE::ECS::ClientApp::Module>();
        }    
    }

    void FlecsGear::deactivated()    
    {
    
    }    
}

RAOE_DEFINE_GEAR(FlecsGear, RAOE::Gears::FlecsGear)
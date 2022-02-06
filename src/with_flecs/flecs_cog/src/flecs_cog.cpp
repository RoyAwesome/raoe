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

#include "flecs_cog.hpp"
#include "engine.hpp"
#include "client_app_module.hpp"

namespace RAOE::Cogs
{

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


    FlecsCog::FlecsCog()   
        : IEngineCog() 
        , ecs_world_client(std::make_unique<flecs::world>())
    {
        InitFLECSSystem();    
    }

    void FlecsCog::activated()    
    {
        register_tickfunc([this](){
            if(ecs_world_client)
            {
                if(!ecs_world_client->progress())
                {
                    if(std::shared_ptr<RAOE::Cogs::Engine> engine_cog = CogManager::Get().get_cog<RAOE::Cogs::Engine>().lock())
                    {
                        engine_cog->engine_ptr->request_exit();
                    }                   
                }
            }
        });  

        if(ecs_world_client)
        {
            ecs_world_client->import<RAOE::ECS::ClientApp::Module>();
        }    
    }

    void FlecsCog::deactivated()    
    {
    
    }

    REGISTER_COG(FlecsCog)

}
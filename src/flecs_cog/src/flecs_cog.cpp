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

namespace RAOE::Cogs
{

    FlecsCog::FlecsCog()   
        : IEngineCog() 
        , ecs_world_client(std::make_unique<flecs::world>())
    {
        
    
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
    }

    void FlecsCog::deactivated()    
    {
    
    }

}
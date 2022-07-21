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
#include "engine.hpp"
#include "cogs/cog.hpp"
#include "cogs/gear.hpp"
#include "cogs/gear_service.hpp"
#include "resource/resources.hpp"

namespace RAOE::Gears
{
    struct FrameworkGear : public RAOE::Cogs::Gear
    {
        FrameworkGear(RAOE::Cogs::BaseCog& in_cog, std::string_view in_name)
            : RAOE::Cogs::Gear(in_cog, in_name)
        {   
            startup_task(RAOE::Resource::init_resource_type(engine(), RAOE::Framework::Tags::GameType));
        }

        void activated() override
        {
        }

        std::shared_ptr<RAOE::Resource::Handle> m_active_game;
    };
}

namespace RAOE::Framework::Tags
{
    const RAOE::Resource::Tag GameType = RAOE::Resource::Tag("raoe:type/game");
}

namespace RAOE::Framework
{
    raoe::lazy<> make_active_game(RAOE::Engine& engine, std::shared_ptr<RAOE::Resource::Handle> game_handle)
    {
        //TODO: Loading screen
        //std::shared_ptr<loading_screen_handle> loading_screen = co_await raoe::engine::loading_screen(); //something like this

        //unload the current game
        co_await deactivate_game(engine);

        //wait for the game to be loaded
        co_await RAOE::Resource::is_handle_loaded(game_handle);
        //make game active
        if(auto task_service = engine.get_service<RAOE::Service::TaskService>().lock())
        {
            if(auto game = game_handle->get<IGame>().lock())
            {
                game->begin(); //tell the game we are about to begin, and generate the tasks to enqueue

                //Enqueue the startup tasks
                task_service->enqueue_startup_tasks(*game);
            }    

            //make the game active
            if(auto gear_service = engine.get_service<RAOE::Service::GearService>().lock())
            {
                if(auto framework_gear = gear_service->get_gear<RAOE::Gears::FrameworkGear>().lock())
                {
                    framework_gear->m_active_game = game_handle;
                }
            }   
        }      
    }

    raoe::lazy<> deactivate_game(RAOE::Engine& engine)
    {
        //Unload the game
        if(auto task_service = engine.get_service<RAOE::Service::TaskService>().lock())
        {
            
            if(auto gear_service = engine.get_service<RAOE::Service::GearService>().lock())
            {
                if(auto framework_gear = gear_service->get_gear<RAOE::Gears::FrameworkGear>().lock())
                {
                    if(framework_gear->m_active_game)
                    {
                        if(auto game = framework_gear->m_active_game->get<IGame>().lock())
                        {
                            game->end();
                            task_service->enqueue_shutdown_tasks(*game);
                            //TODO: Await all these shutdown tasks
                        }   

                        framework_gear->m_active_game.reset();                    
                    }
                }
            }            
        }

        co_return;
    }

    bool has_active_game(RAOE::Engine& engine)    
    {
        if(auto gear_service = engine.get_service<RAOE::Service::GearService>().lock())
        {
            if(auto framework_gear = gear_service->get_gear<RAOE::Gears::FrameworkGear>().lock())
            {
                return !!(framework_gear->m_active_game);
            }
        }
        return false;    
    }

    std::shared_ptr<IGame> get_active_game(RAOE::Engine& engine)    
    {
        if(auto gear_service = engine.get_service<RAOE::Service::GearService>().lock())
        {
            if(auto framework_gear = gear_service->get_gear<RAOE::Gears::FrameworkGear>().lock())
            {
                return framework_gear->m_active_game->get<IGame>().lock();
            }
        }

        return {};
    }
}



RAOE_DEFINE_GEAR(FrameworkGear, RAOE::Gears::FrameworkGear)
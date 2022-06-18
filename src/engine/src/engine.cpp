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
#include <cassert>
#include "cogs/cog.hpp"

#include "console/console.hpp"
#include "console/command.hpp"

#include "services/tick_service.hpp"
#include "cogs/cog_service.hpp"
#include "cogs/gear_service.hpp"
#include "resource/service.hpp"

namespace RAOE
{ 
    /*
        Special Engine Cog
        The Engine is not a cog like other parts of the engine.  It manages the cogs, and does not generate one itself

        So, we need to manually create a cog for the engine to host the engine's gears.  
    */
    struct EngineCog : public RAOE::Cogs::BaseCog
    {
        EngineCog(RAOE::Engine& in_engine)
            : RAOE::Cogs::BaseCog(in_engine, "raoe:cog")
        {    

        }

        virtual void register_gears() override
        {
            
        }
        [[nodiscard]] bool is_engine_cog() const override { return true; }
    };



    namespace TransitionFunc
    {
        void no_op(RAOE::Cogs::BaseCog&) { };
        void register_gears(RAOE::Cogs::BaseCog& cog) 
        { 
            cog.register_gears();
        };

        void activate_gears(RAOE::Cogs::BaseCog& for_cog) 
        { 
            if(std::shared_ptr<RAOE::Service::GearService> gear_service = for_cog.engine().get_service<RAOE::Service::GearService>().lock())
            {
                for(const auto& [_, gear_ptr] : gear_service->all_gears())
                {
                    if(gear_ptr && &gear_ptr->cog() == &for_cog)
                    {
                        gear_ptr->activated();
                    }
                } 
            }          
        };

        void ShutdownGears(RAOE::Cogs::BaseCog& for_cog) 
        { 
            if(std::shared_ptr<RAOE::Service::GearService> gear_service = for_cog.engine().get_service<RAOE::Service::GearService>().lock())
            {
                for(const auto& [_, gear_ptr] : gear_service->all_gears())
                {
                    if(gear_ptr && &gear_ptr->cog() == &for_cog)
                    {
                        gear_ptr->deactivated();
                    }
                }   
            }
        };

        void LockCogForShutdown(RAOE::Cogs::BaseCog&) 
        { 
        };
    }

    using AutoRegisterConsoleCommand = RAOE::Console::AutoRegisterConsoleCommand;
    static AutoRegisterConsoleCommand quit_command = RAOE::Console::CreateConsoleCommand(
        "quit",
        "Exits the game",
        +[](Engine& engine) {
            if(auto tick_service = engine.get_service<RAOE::Service::TickService>().lock())
            {
                tick_service->request_exit();
            }
        }
    );

    static AutoRegisterConsoleCommand log_command = RAOE::Console::CreateConsoleCommand(
        "log",
        "prints the arguments to the log",
        +[](std::string_view args) {
            spdlog::info(args);
        }
    );

    void foo(int32 a, int32 b, std::string c) { //NOLINT
             spdlog::info("fooo {} {} {}", a, b, c);
        }

    static AutoRegisterConsoleCommand test_comamnd = RAOE::Console::CreateConsoleCommand(
        "test_args",
        "prints the arguments to the log",
        foo
    );




    Engine::Engine(int argv, char** argc)
        : Engine()
    {
        init_service<RAOE::Service::TickService>();
        init_service<RAOE::Service::GearService>();
        init_service<RAOE::Resource::Service>();        
    }

    Engine::Engine()    
    {
        init_service<RAOE::Service::CogService>();

        if(auto cog_service = get_service<RAOE::Service::CogService>().lock())
        {
            //At this point, there are no cogs in the registry.  We must create the special engine cog here with it's own gears
            //and activate them. 
            cog_service->register_static_cog<EngineCog>();
            cog_service->transition_cog<EngineCog>(RAOE::Cogs::ECogStatus::PreActivate, TransitionFunc::register_gears);
            cog_service->transition_cog<EngineCog>(RAOE::Cogs::ECogStatus::Activated, TransitionFunc::activate_gears);
        }
    
    }

    Engine::~Engine()
    {      
        if(std::shared_ptr<RAOE::Service::CogService> cog_service = get_service<RAOE::Service::CogService>().lock())
        {
            cog_service->transition_cog<EngineCog>(RAOE::Cogs::ECogStatus::PreShutdown, TransitionFunc::ShutdownGears);
            cog_service->transition_cog<EngineCog>(RAOE::Cogs::ECogStatus::Shutdown, TransitionFunc::no_op);
        }
    }

    void Engine::Startup()    
    {  
        spdlog::info("Initializing Roy Awesome's Open Engine (RAOE)");

        
        if(std::shared_ptr<RAOE::Service::CogService> cog_service = get_service<RAOE::Service::CogService>().lock())
        {
            cog_service->transition_cogs(RAOE::Cogs::ECogStatus::PreActivate, TransitionFunc::register_gears);
            cog_service->transition_cogs(RAOE::Cogs::ECogStatus::Activated, TransitionFunc::activate_gears);
        }
        else
        {
            spdlog::error("Unable to start up engine, cog service doesn't exist");
            return;
        }     

        spdlog::info("Registered Commands: ");
        for(auto& cmd : Console::CommandRegistry::Get().elements())
        {
            spdlog::info("\t{} - {}", cmd->name(), cmd->description());
        }
    }

    bool Engine::Run()    
    {
        std::shared_ptr<RAOE::Service::TickService> tick_service = get_service<RAOE::Service::TickService>().lock();
        if(!tick_service)
        {
            spdlog::error("Engine::Run() - Unable to tick, cannot find the tick service!");
            return false;
        }

        tick_service->run_frame();

        return tick_service->should_exit();
    }

    void Engine::Shutdown()    
    {
        spdlog::info("Shutting down cogs for clean shutdown");
      
        if(std::shared_ptr<RAOE::Service::CogService> cog_service = get_service<RAOE::Service::CogService>().lock())
        {
            cog_service->transition_cogs(RAOE::Cogs::ECogStatus::PreShutdown, TransitionFunc::ShutdownGears);
            cog_service->transition_cogs(RAOE::Cogs::ECogStatus::Shutdown, TransitionFunc::LockCogForShutdown);
        }
        else
        {
            spdlog::error("Unable to shut down engine cleanly, cog service doesn't exist");
            return;
        }
    }
}
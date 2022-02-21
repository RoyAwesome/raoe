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

namespace RAOE
{
    const std::string EngineCogName("Global::Engine");
    namespace Gears
    {
        const std::string EngineGearName("Engine::Gear");
    }
    /*
        Special Engine Cog
        The Engine is not a cog like other parts of the engine.  It manages the cogs, and does not generate one itself

        So, we need to manually create a cog for the engine to host the engine's gears.  
    */
    namespace _
    {
        struct EngineCog : public RAOE::Cogs::BaseCog
        {
            friend class Engine;
            EngineCog()
                : RAOE::Cogs::BaseCog(std::string(EngineCogName))
            {    

            }

            virtual void register_gears() override
            {
                register_gear(Gears::EngineGearName, [](){ return new RAOE::Gears::Engine(); });
            }

            virtual bool is_engine_cog() override { return true; }
            std::unique_ptr<Engine> engine_ptr;
        };

    }


    namespace TransitionFunc
    {
        static auto NoOp = [](RAOE::Cogs::BaseCog&) { };
        static auto ConstructGears = [](RAOE::Cogs::BaseCog& cog) { 
            cog.register_gears();
            cog.construct_gears();
        };

        static auto ActivateGears = [](RAOE::Cogs::BaseCog& for_cog) { 
            for(auto& [name, value] : RAOE::Cogs::Registry::Get().gears())
            {
                auto& [gear, cog] = value;
                if(cog == &for_cog)
                {
                    gear->activated();
                }
            }
        };

        static auto ShutdownGears = [](RAOE::Cogs::BaseCog& for_cog) { 
             for(auto& [name, value] : RAOE::Cogs::Registry::Get().gears())
            {
                auto& [gear, cog] = value;
                if(cog == &for_cog)
                {
                    gear->deactivated();
                }
            }
        };

        static auto LockCogForShutdown  = [](RAOE::Cogs::BaseCog&) { 

        };

        static auto ShutdownEngine = [](RAOE::Cogs::BaseCog& cog) { 
            _::EngineCog& engine_cog = static_cast<_::EngineCog&>(cog);
            engine_cog.engine_ptr.reset();
        };
    }

    using AutoRegisterConsoleCommand = RAOE::Console::AutoRegisterConsoleCommand;
    using CommandArgs = RAOE::Console::CommandArgs;
    static AutoRegisterConsoleCommand quit_command = RAOE::Console::CreateConsoleCommand(
        "quit",
        "Exits the game",
        [](const CommandArgs& args) {
            Gears::Engine* engine_gear = static_cast<Gears::Engine*>(RAOE::Cogs::Registry::Get().get_gear(Gears::EngineGearName));
            if(engine_gear != nullptr)
            {
                engine_gear->request_exit();
            }
        }
    );

    static AutoRegisterConsoleCommand log_command = RAOE::Console::CreateConsoleCommand(
        "log",
        "prints the arguments to the log",
        [](const CommandArgs& args) {
            spdlog::info(args.args_as_string);
        }
    );

    Engine& Engine::Init(int32 argc, char* argv[])
    {
        spdlog::info("Initializing Roy Awesome's Open Engine (RAOE)");

        //At this point, there are no cogs in the registry.  We must create the special engine cog here with it's own gears
        //and activate them.  
        _::EngineCog& EngineCog = static_cast<_::EngineCog&>(RAOE::Cogs::Registry::Get().construct_cog<_::EngineCog>(EngineCogName));
        EngineCog.engine_ptr = std::make_unique<Engine>();
        RAOE::Cogs::Registry::Get().transition_cog(EngineCogName, RAOE::Cogs::ECogStatus::PreActivate, TransitionFunc::ConstructGears);
        RAOE::Cogs::Registry::Get().transition_cog(EngineCogName, RAOE::Cogs::ECogStatus::Activated, TransitionFunc::ActivateGears);

        //Construct the statically linked cogs
        RAOE::Cogs::Registry::Get().create_statically_linked_cogs();
        RAOE::Cogs::Registry::Get().transition_cogs(RAOE::Cogs::ECogStatus::PreActivate, TransitionFunc::ConstructGears);
        RAOE::Cogs::Registry::Get().transition_cogs(RAOE::Cogs::ECogStatus::Activated, TransitionFunc::ActivateGears);

        spdlog::info("Registered Commands: ");
        for(auto& cmd : Console::CommandRegistry::Get().elements())
        {
            spdlog::info("\t{} - {}", cmd->name(), cmd->description());
        }

        return *EngineCog.engine_ptr.get();
    }

    bool Engine::Run()    
    {
        Gears::Engine* engine_gear = static_cast<Gears::Engine*>(RAOE::Cogs::Registry::Get().get_gear(Gears::EngineGearName));
        if(engine_gear == nullptr)
        {
            spdlog::error("Engine::Run() - Unable to tick, cannot find {}}!", Gears::EngineGearName);
            return false;
        }

        for(auto& tickfunc : engine_gear->tickfuncs())
        {
            tickfunc();
        }
        return !engine_gear->should_exit();
    }

    void Engine::Shutdown()    
    {
        spdlog::info("Shutting down cogs for clean shutdown");

        RAOE::Cogs::Registry::Get().transition_cogs(RAOE::Cogs::ECogStatus::PreShutdown, TransitionFunc::ShutdownGears);
        
        RAOE::Cogs::Registry::Get().transition_cogs(RAOE::Cogs::ECogStatus::Shutdown, TransitionFunc::LockCogForShutdown);

        RAOE::Cogs::Registry::Get().transition_cog(EngineCogName, RAOE::Cogs::ECogStatus::PreShutdown, TransitionFunc::ShutdownGears);
        RAOE::Cogs::Registry::Get().transition_cog(EngineCogName, RAOE::Cogs::ECogStatus::Shutdown, TransitionFunc::ShutdownEngine);
    }


}
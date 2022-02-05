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
#include <cassert>

namespace RAOE
{
    using EngineCog = RAOE::Cogs::Engine;
    REGISTER_COG(EngineCog)


    Engine& Engine::Init(int32 argc, char* argv[])
    {
        spdlog::info("Initializing Roy Awesome's Open Engine (RAOE)");

        if(std::shared_ptr<EngineCog> engine_cog = CogManager::Get().get_cog<EngineCog>().lock())
        {
            engine_cog->engine_ptr = std::make_unique<Engine>();
            //TODO: Forward the command line args

            CogManager::Get().activate_cog<EngineCog>();
            
            return *engine_cog->engine_ptr.get();
        }
        spdlog::error("Unable to construct the engine cog.  This is bad and a crash");
        Voidcraft::Debug::Break();
        assert(0);
        __assume(0);
    }



}
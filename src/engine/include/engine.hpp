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

#include <functional>
#include <vc/Core.hpp>
#include "engine_cog.hpp"

#pragma once

int main(int, char**);

namespace RAOE
{
    class Engine
    {     
    protected:
        //Main calls Init and Run (but nobody else can).  So it's friend.  
        friend int ::main(int, char**);
        friend class CogManager;

        static Engine& Init(int32 argc, char* argv[]);
        bool Run();
        void Shutdown();
    public:
      
        void request_exit() { exit_requested = true; }
        bool should_exit() const { return exit_requested; }

    private:
        bool exit_requested = false;
    };

    namespace Cogs
    {
        /* Engine Cog
            This is a special cog that contains the engine object.  When the Engine is created, the Engine will create this cog and activate it before any other cog
            It is also the last cog to shutdown
        */
        struct Engine : public RAOE::IEngineCog
        {
            virtual void activated() override {}
            virtual void deactivated() override {}

            std::unique_ptr<RAOE::Engine> engine_ptr;
        };       
    }
}
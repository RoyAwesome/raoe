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

#include "version.hpp"
#include "spdlog/spdlog.h"
#include <memory>
#include "flecs.h"

#include "sdl_module.hpp"
#include "imgui_module.hpp"
#include "application_module.hpp"

using int32 = int;

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

int main(int argc, char* argv[])
{

    spdlog::info("Garrett's Open Tiberium Sun Remake (GOTSR) version {} (TibSun: {} RA2: {})", GOTSR_VERSION, TS_VERSION, RA2_VERSION);

    InitFLECSSystem();

    std::unique_ptr<flecs::world> world = std::make_unique<flecs::world>(argc, argv);
    world->import<RAOE::ECS::App::Module>();
    world->import<RAOE::ECS::SDL::Module>();
    world->import<RAOE::ECS::Imgui::Module>();

    world->entity()

    while(world->progress()) { }

    return 0;
}
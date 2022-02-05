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
#include "imgui_module.hpp"

#include "flecs.h"
#include "sdl_components.hpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "imcmd_command_palette.h"

namespace OTSR::ECS::Imgui
{
    using SDLSystem = OTSR::SDL::System;
    using Events = OTSR::SDL::Events;
   
    struct ImCmdInfo
    {
        bool should_show_command_palette;
    };

    void NewFrame(flecs::entity e, const SDLSystem& sdl_system)
    {      
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdl_system.window_handle.get());
        ImGui::NewFrame();   

        ImCmdInfo* info = e.world().module<Module>().get_mut<ImCmdInfo>();
        if(info)
        {
            if(info->should_show_command_palette)
            {
                ImCmd::CommandPaletteWindow("Command Palette", &info->should_show_command_palette);
            }
        }
    }

    void UpdateIO(flecs::entity e, const Events& events)
    {
        for(const SDL_Event& event : events.events_this_frame)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDL_KeyCode::SDLK_p && !!(event.key.keysym.mod & (KMOD_CTRL | KMOD_SHIFT)))
            {   
                ImCmdInfo* info = e.world().module<Module>().get_mut<ImCmdInfo>();
                if(info)
                {
                    info->should_show_command_palette = !info->should_show_command_palette;
                }
            }
        }
    }

    void DrawFrame(flecs::entity e, const SDLSystem& sdl_system)
    {
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    }  

    

   

    Module::Module(flecs::world& world)    
    {  
        //Setup the Dear ImGui context (Taken from the imgui SDL Renderer example)
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        world.module<Module>().set<ImCmdInfo>({false});

        ImCmd::CreateContext();

        ImCmd::Command quit_command;
        quit_command.Name = "Quit";
        quit_command.InitialCallback = [&world]() {
            world.quit();
        };

        ImCmd::AddCommand(std::move(quit_command));
        

        //Init the style
        ImGui::StyleColorsDark(); //TODO: Make this configurable by command line?


        //Observe for the Window and Renderer creation events.  When that happens, we'll init the rest of the imgui/sdl binding
        world.observer<SDLSystem>()
            .event(flecs::OnSet)
            .each([](flecs::entity e, const SDLSystem& sdl_system){
                if(sdl_system.window_handle)
                {   
                    ImGui_ImplSDL2_InitForSDLRenderer(sdl_system.window_handle.get(), sdl_system.renderer_handle.get());
                    ImGui_ImplSDLRenderer_Init(sdl_system.renderer_handle.get());
                }
               
            });

        world.system<Events>()
            .kind(flecs::PreUpdate)
            .no_staging()
            .each(&UpdateIO);

        world.system<SDLSystem>()
            .kind(flecs::PreUpdate)
            .no_staging()
            .each(&NewFrame);
    
        world.system<SDLSystem>()
            .kind(flecs::OnStore)
            .each(&DrawFrame);      
    }
}
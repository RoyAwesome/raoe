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

#include "sdl_module.hpp"
#include "sdl_components.hpp"
#include "client_app_module.hpp"

#include "flecs.h"

namespace RAOE::ECS::SDL
{
    using System = RAOE::SDL::System;
    using Events = RAOE::SDL::Events;
    using Canvas = RAOE::ECS::ClientApp::Canvas;

    void PollWindow(flecs::entity e, Events& events)
    {
       events.events_this_frame.clear();
           
        SDL_Event event;
        while(SDL_PollEvent(&event) != 0)
        {
            if(event.type == SDL_QUIT)
            {
                e.world().quit();
            }
            else
            {
                events.events_this_frame.push_back(event);
            }
        }
    }

    void ClearRenderer(flecs::entity e, const System& sdl_system)
    {
        if(sdl_system.renderer_handle)
        {
            SDL_SetRenderDrawColor(sdl_system.renderer_handle.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(sdl_system.renderer_handle.get());
        }
    }

    void PresentRender(flecs::entity e, const System& sdl_system)
    {
        if(sdl_system.renderer_handle)
        {
            SDL_RenderPresent(sdl_system.renderer_handle.get());
        }
    }

    Module::Module(flecs::world& world)
    {
        //Register the Window and renderer components with the flecs world so we can use them
        world.component<System>();
        world.component<Events>();      

        //Register the systems
        world.system<Events>()
            .kind(flecs::PreFrame) //PreFrame is before any other stages execute
            .no_staging()
            .each(&PollWindow);

        world.system<System>()
            .kind(flecs::PreStore) //PreStore is before rendering
            .each(&ClearRenderer);
        
        world.system<System>()
            .kind(flecs::PostFrame) //PostFrame is the last thing to happen in a frame.  This will be where we present the rendered frame
            .each(&PresentRender);

        world.observer<Canvas>()
            .event(flecs::OnSet)
            .each([](flecs::entity e, const Canvas& canvas)
            {
                e.add<System>();
                System* sdl_system = e.get_mut<System>();
                sdl_system->system_handle = sdl2::make_sdl(SDL_INIT_EVERYTHING);
                sdl_system->window_handle = sdl2::make_window(canvas.title, 100, 100, canvas.window_size.x, canvas.window_size.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                sdl_system->renderer_handle = sdl2::make_renderer(sdl_system->window_handle, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED );
                e.modified<System>();
                
                e.add<Events>();

            });
    }

}
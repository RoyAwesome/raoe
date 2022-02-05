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

#pragma once
#include "sdl_helper.hpp"
#include <vector>

namespace OTSR::SDL
{    
    struct System
    {
        System()
            : system_handle(nullptr, nullptr)
            , window_handle(nullptr, nullptr)
            , renderer_handle(nullptr, nullptr)
        {

        }
        System(sdl2::sdl_system_ptr&& system_ptr, sdl2::sdl_window_ptr&& window_ptr,  sdl2::sdl_renderer_ptr&& renderer_ptr)
            : system_handle(std::move(system_ptr))
            , window_handle(std::move(window_ptr))
            , renderer_handle(std::move(renderer_ptr))
        {
            
        }

        sdl2::sdl_system_ptr system_handle;
        sdl2::sdl_window_ptr window_handle;
        sdl2::sdl_renderer_ptr renderer_handle;
    };   

    struct Events
    {      
        std::vector<SDL_Event> events_this_frame;
    };
}
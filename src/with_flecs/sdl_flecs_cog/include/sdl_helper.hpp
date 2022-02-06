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

// Very useful sdl helper functions from Eric Scott Barr for managing sdl resources with std::unique_ptr:
// https://eb2.co/blog/2014/04/c-plus-plus-14-and-sdl2-managing-resources/

#include "SDL.h"
#include <memory>
#include <string>

namespace sdl2 
{
    inline namespace _
    {
        template<typename Creator, typename Destructor, typename... Arguments>
        auto make_resource(Creator c, Destructor d, Arguments&&... args)
        {
            auto r = c(std::forward<Arguments>(args)...);
            return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
        }
    }
   

    using sdl_window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
    inline sdl_window_ptr make_window(std::string_view title, int x, int y, int w, int h, Uint32 flags)
    {
       return make_resource(SDL_CreateWindow, SDL_DestroyWindow, std::string(title).c_str(), x, y, w, h, flags);
    }

    //Wrapper for SDL's System return value.
    //This is so we can store the value into a unique_ptr and use a deleter to tear down SDL like any other resource
    inline namespace _
    {
        using SDL_System = int;

        inline SDL_System* SDL_CreateSDL(Uint32 flags)
        {
            auto init_status = new SDL_System;
            *init_status = SDL_Init(flags);
            return init_status;
        }

        inline void SDL_DestroySDL(SDL_System* init_status)
        {
            delete init_status;
            SDL_Quit();
        }
    }

    using sdl_system_ptr = std::unique_ptr<SDL_System, decltype(&SDL_DestroySDL)>;
    inline sdl_system_ptr make_sdl(Uint32 flags)
    {
        return make_resource(SDL_CreateSDL, SDL_DestroySDL, flags);
    }

    using sdl_renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
    inline sdl_renderer_ptr make_renderer(const sdl_window_ptr& window, int index, Uint32 flags)
    {
        return make_resource(SDL_CreateRenderer, SDL_DestroyRenderer, window.get(), index, flags);
    }
   

}
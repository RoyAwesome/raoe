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

#include "frontend.hpp"
#include "engine.hpp"
#include "resource/resources.hpp"
#include "flecs.h"
#include "flecs_gear.hpp"
#include "imgui.h"
#include "frontend_internal.hpp"
#include "framework.hpp"

namespace RAOE::Frontend
{
    //imgui uses a lot of vararg functions.  
    //NOLINTBEGIN(cppcoreguidelines-pro-type-vararg)
    void draw_frontend(flecs::entity e, FrontenedPanel& panel)
    {
        RAOE::Engine& engine = *static_cast<RAOE::Engine*>(e.world().get_context());

        ImGui::Begin("Frontend");
        {
            ImGui::Text("This is a temporary Frontend.  I will eventually make this feel like a game console, switching out cartridges.");
            ImGui::Text("But for now, it's just a simple list of games");
            ImGui::Separator();

            ImGui::Text("Games: ");
            if(auto resource_service = engine.get_service<RAOE::Resource::Service>().lock())
            {
                for(const auto& [tag, handle] : resource_service->handle_map())
                {
                    if(auto locked_handle = handle.lock())
                    {
                        if(locked_handle->resource_type() == RAOE::Framework::Tags::GameType)
                        {                            
                            ImGui::Text("%s", std::string(tag).c_str());
                            ImGui::SameLine();
                            if(ImGui::Button("Start Game"))
                            {
                                //Transition to the game
                            }                        
                        }
                    }
                }
            }
           
            
        }
        ImGui::End();
    }


    Module::Module(flecs::world& world)
    {
        world.component<FrontenedPanel>();    
        world.system<FrontenedPanel>()
            .kind(flecs::OnUpdate)
            .each(&draw_frontend);  
    }


    //NOLINTEND(cppcoreguidelines-pro-type-vararg)
}




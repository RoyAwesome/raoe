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

#include "lazy.hpp"
#include <coroutine>
#include "engine.hpp"
#include "resource/tag.hpp"
#include "resource/service.hpp"

namespace RAOE::Resource
{
    template<typename ResourceType>
    raoe::lazy<> emplace_owned_resource(RAOE::Engine& engine, RAOE::Resource::Tag resource_tag, RAOE::Resource::Tag type_tag) //NOLINT
    {
        if(auto resource_service = engine.get_service<RAOE::Resource::Service>().lock())
        {
            auto game_handle = resource_service->get_resource(type_tag);        
            co_await RAOE::Resource::is_handle_loaded(game_handle);

            resource_service->emplaced_owned_resource(resource_tag, std::make_shared<ResourceType>(engine), type_tag);
        }
    }

    inline raoe::lazy<> init_resource_type(RAOE::Engine& engine, RAOE::Resource::Tag resource_tag)
    {
        if(auto resource_service = engine.get_service<RAOE::Resource::Service>().lock())
        {
            resource_service->init_resource_type(resource_tag);
        }
        co_return;
    }

}
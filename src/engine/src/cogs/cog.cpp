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

#include "cogs/cog.hpp"
#include <optional>

namespace RAOE::Cogs
{
    static std::optional<Registry>& registry_singleton()
    {
        static std::optional<Registry> singleton(std::in_place);
        return singleton;
    }

    Registry& Registry::Get()
    {
        return registry_singleton().value();
    }

    BaseCog& Registry::construct_cog(std::string_view name, std::function<BaseCog*()> factory)    
    {
        auto pair = registered_cogs.emplace(std::string(name), std::unique_ptr<BaseCog>(factory()));
        return *(pair.first->second.get());
    }

    void Registry::create_statically_linked_cogs()    
    {
        for(const StaticRegisteredCogFactory& static_cog : static_registered_cogs)
        {
            construct_cog(static_cog.name, static_cog.factory_func);
        }
        static_registered_cogs.clear();
    }

    void Registry::transition_cogs(ECogStatus transition_to, std::function<void(BaseCog&)> transition_func)    
    {
        //Run the transition funcs
        for(const auto& [key, value] : registered_cogs)
        {
            if(!value->is_engine_cog() && value->status() < transition_to)
            {
                 transition_func(*value.get());
            }           
        }
        //then set the value
        for(const auto& [key, value] : registered_cogs)
        {
            if(!value->is_engine_cog() && value->status() < transition_to)
            {
                spdlog::info("Transitioning Cog {} from {} to {}", value->name(), value->status(), transition_to);
                value->m_status = transition_to;
                
            }            
        }
    }

    void Registry::transition_cog(std::string cog_name, ECogStatus transition_to, std::function<void(BaseCog&)> transition_func)
    {
        BaseCog* cog = get_cog(cog_name);
        if(!cog)
        {
            return;
        }
        transition_func(*cog);
        spdlog::info("Transitioning Cog {} from {} to {}", cog->name(), cog->status(), transition_to);
        cog->m_status = transition_to;
    }

    BaseCog* Registry::get_cog(std::string_view cog_name)
    {
        if(registered_cogs.contains(std::string(cog_name)))
        {
            return registered_cogs[std::string(cog_name)].get();
        }

        return nullptr;
    }

    Gear* Registry::get_gear(std::string_view gear_name)    
    {   
        if(registered_gears.contains(std::string(gear_name)))
        {
            auto& [gear, cog] = registered_gears[std::string(gear_name)];
            return gear.get();
        }
        return nullptr;
    }

    Gear& Registry::construct_gear(std::string_view name, std::function<Gear*()> factory, BaseCog* owning_cog)
    {
        std::string gear_name(name);
        if(owning_cog && !owning_cog->is_engine_cog())
        {
            gear_name = std::string(owning_cog->name()) + "::" + gear_name;
        }

        spdlog::info("Constructing Gear {}", name);
        auto pair = registered_gears.emplace(gear_name, std::tuple<std::unique_ptr<Gear>, BaseCog*>{ std::unique_ptr<Gear>(factory()), owning_cog } );

        auto& [gear, cog] = pair.first->second;
        return *(gear.get());
    }

    void BaseCog::construct_gears()    
    {
        for(const auto& gear_factory : gear_factories)
        {
            Registry::Get().construct_gear(gear_factory.name, gear_factory.factory_func, this);
        }

        gear_factories.clear();
    }
}
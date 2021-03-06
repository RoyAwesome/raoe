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

#include "core.hpp"
#include "services/iservice.hpp"
#include "cogs/cog.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <concepts>
#include "container/subclass_map.hpp"

namespace RAOE
{
    class Engine;
}

namespace RAOE::Service
{
    class CogService : public IService
    {
        using ECogStatus = RAOE::Cogs::ECogStatus;
        using BaseCog = RAOE::Cogs::BaseCog;
    public:
        friend class RAOE::Engine;

        CogService(Engine& in_engine)
            : IService(in_engine)
        {
        }
        
        template<RAOE::Cogs::is_cog T>
        std::weak_ptr<T> register_static_cog()
        {
           return std::dynamic_pointer_cast<T>(find_or_create_cog<T>().lock());
        }

        template<RAOE::Cogs::is_cog T>
        void transition_cog(ECogStatus transition_to, std::function<void(BaseCog&)> transition_func)
        {
            if(auto cog = m_cogs.find<T>().lock())
            {
                transition_func(*(cog.get()));
                spdlog::info("Transitioning Cog {} from {} to {}", cog->name(), cog->status(), transition_to);
                cog->m_status = transition_to;
            }    
        }

        void transition_cogs(ECogStatus transition_to, std::function<void(BaseCog&)> transition_func);
    private:
        template<RAOE::Cogs::is_cog T>
        std::weak_ptr<RAOE::Cogs::BaseCog> find_or_create_cog()
        {           
            if(m_cogs.contains<T>())
            {
                return m_cogs.find<T>();
            }

            auto ptr = m_cogs.insert<T>(engine());
            register_cog_resource(ptr);
            return ptr;
        }

        void register_cog_resource(std::weak_ptr<BaseCog> cog_ptr);

        raoe::container::subclass_map<RAOE::Cogs::BaseCog> m_cogs;
    };
}

#define __GENERATED_REGISTER_COG(CogName, CogTypename, CogQualifiedName, CogFunctionName) \
    extern "C" void CogFunctionName(RAOE::Engine& engine) { if(auto service = engine.get_service<RAOE::Service::CogService>().lock()) service->register_static_cog<CogTypename>(); }
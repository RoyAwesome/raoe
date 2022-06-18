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
#include <unordered_map>
#include <string>
#include <memory>
#include <concepts>
#include "resource/iresource.hpp"
#include "resource/tag.hpp"
#include "engine_fwd.hpp"

namespace RAOE
{
    class Engine;
    namespace Service
    {
        class CogService;
        class GearService;
    }

}

namespace RAOE::Cogs
{       
    enum class ECogStatus : uint8
    {
        Created,
        PreActivate,
        Activated,
        PreShutdown,
        Shutdown,
    };

    class BaseCog : public RAOE::Resource::IResource
    {
        friend class RAOE::Service::CogService;
    public:
        BaseCog() = delete; //Cogs must be constructed by the CogService
        BaseCog(const BaseCog&) = delete; //Cogs cannot be copied
        BaseCog& operator=(const BaseCog&) = delete;

        BaseCog(BaseCog&&) = delete; //Cogs cannot be moved        
        BaseCog& operator=(BaseCog&&) = delete;
        virtual ~BaseCog() = default;
    protected:
        explicit BaseCog(RAOE::Engine& in_engine, std::string_view in_name)
            : m_engine(in_engine)
            , m_status(ECogStatus::Created)
            , m_tag(in_name)
        {
        }     
       
    public:
        [[nodiscard]] RAOE::Engine& engine() const { return m_engine; }
        [[nodiscard]] ECogStatus status() const { return m_status; }
        [[nodiscard]] std::string_view name() const { return m_tag.prefix(); }
        virtual void register_gears() = 0;   

        [[nodiscard]] virtual bool is_engine_cog() const { return false; }

        //BEGIN: IResource Interface
        [[nodiscard]] RAOE::Resource::IResource::ELoadStatus loadstatus() const override { return RAOE::Resource::IResource::ELoadStatus::Loaded; }
        //END: IResource Interface
        
        [[nodiscard]] const RAOE::Resource::Tag& tag() const { return m_tag; }
    protected:


    private:
        RAOE::Engine& m_engine;
        ECogStatus m_status;
        RAOE::Resource::Tag m_tag;      
    };

    template<typename T> 
    concept is_cog = std::derived_from<T, RAOE::Cogs::BaseCog> && has_engine_ref<T>;
}




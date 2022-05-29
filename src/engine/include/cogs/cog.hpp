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
        BaseCog(BaseCog&&) = delete; //Cogs cannot be moved
        virtual ~BaseCog() = default;
    protected:
        explicit BaseCog(RAOE::Engine& in_engine, std::string&& in_name)
            : m_engine(in_engine)
            , m_status(ECogStatus::Created)
            , m_name(in_name)
        {
        }     
       
    public:
        RAOE::Engine& engine() { return m_engine; }
        ECogStatus status() { return m_status; }
        std::string_view name() { return m_name; }
        virtual void register_gears() = 0;   

        virtual bool is_engine_cog() { return false; }

        //BEGIN: IResource Interface
        virtual RAOE::Resource::IResource::ELoadStatus loadstatus() const override { return RAOE::Resource::IResource::ELoadStatus::Loaded; }
        //END: IResource Interface
        const RAOE::Resource::Tag& tag() const;
    protected:


    private:
        RAOE::Engine& m_engine;
        ECogStatus m_status;
        std::string m_name;           
    };

    template<typename T> 
    concept is_cog = std::derived_from<T, RAOE::Cogs::BaseCog> && has_engine_ref<T>;
}




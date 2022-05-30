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
#include "cogs/cog.hpp"
#include "resource/iresource.hpp"
#include "resource/tag.hpp"

namespace RAOE
{
    class Engine;
}

namespace RAOE::Cogs
{  
    class Gear : public RAOE::Resource::IResource
    {
        Gear() = delete; //use the engine ctor
        Gear(const Gear&) = delete; //cant copy gears
        Gear(Gear&&) = delete; //can't move gears
    protected:
        Gear(RAOE::Cogs::BaseCog& in_cog, std::string_view name)
            : m_cog(in_cog)
            , m_tag(in_cog.tag().prefix(), "gear/" + std::string(name))
        {

        }

    public:
        virtual void activated() {}
        virtual void deactivated() {}

        RAOE::Engine& engine() const { return m_cog.engine(); }
        const RAOE::Cogs::BaseCog& cog() const { return m_cog; }

        //BEGIN: IResource Interface
        virtual RAOE::Resource::IResource::ELoadStatus loadstatus() const override { return RAOE::Resource::IResource::ELoadStatus::Loaded; }
        //END: IResource Interface
        
        const RAOE::Resource::Tag& tag() const { return m_tag; }
    private:
        RAOE::Cogs::BaseCog& m_cog;
        RAOE::Resource::Tag m_tag;
    };

    template<typename T>
    concept is_gear = std::constructible_from<T, RAOE::Cogs::BaseCog&, std::string_view> && std::derived_from<T, RAOE::Cogs::Gear>;
}

#include "cogs/gear_service.hpp"
#include "engine.hpp"

#define RAOE_GEAR_GENERATE_FUNC_NAME(GearName) __GENERATED_REGISTER_##GearName
#define RAOE_GEAR_GENERATE_FUNC_DECL(GearName) extern void RAOE_GEAR_GENERATE_FUNC_NAME(GearName)(RAOE::Cogs::BaseCog& owning_cog)
#define RAOE_GEAR_GENERATE_FUNC_CALL(GearName) RAOE_GEAR_GENERATE_FUNC_NAME(GearName)(*this)

#define RAOE_DEFINE_GEAR(GearName, GearClass) \
    RAOE_GEAR_GENERATE_FUNC_DECL(GearName) \
    { \
        owning_cog.engine().get_service<RAOE::Service::GearService>().lock()->register_gear<GearClass>(owning_cog, #GearName); \
    }


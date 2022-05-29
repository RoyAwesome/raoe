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
#include "cogs/gear.hpp"
#include "services/iservice.hpp"
#include "container/subclass_map.hpp"

namespace RAOE
{
    class Engine;
    namespace Cogs
    {
        class BaseCog;
    }
}

namespace RAOE::Service
{
    class GearService : public IService
    {
    public:
        friend class RAOE::Engine;

        GearService(Engine& in_engine)
            : IService(in_engine)
        {
        }

        template<RAOE::Cogs::is_gear T>
        std::weak_ptr<T> get_gear()
        {
            return m_gears.find<T>();
        }

        template<RAOE::Cogs::is_gear T>
        std::weak_ptr<T> register_gear(RAOE::Cogs::BaseCog& owning_cog)
        {
            return m_gears.insert<T>(owning_cog);
        }

        const raoe::container::subclass_map<RAOE::Cogs::Gear>& all_gears() const { return m_gears; }
    private:

        raoe::container::subclass_map<RAOE::Cogs::Gear> m_gears;
    };
}
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

#include "vc/Core.hpp"
#include "cogs/gear.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <concepts>

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

    class BaseCog
    {
    protected:
        explicit BaseCog(std::string&& in_name)
            : m_status(ECogStatus::Created)
            , m_name(in_name)
        {
            
        }     
        virtual void register_gears() = 0;
        void register_gear(std::shared_ptr<Gear> gear_ptr);

    public:
        ECogStatus status() { return m_status; }
        std::string_view name() { return m_name; }
    private:
        ECogStatus m_status;
        std::string m_name;    
    };

    class Registry
    {
    public:
        static Registry& Get();

        template<class T>
            requires std::derived_from<T, BaseCog> && std::default_initializable<T>
        BaseCog& register_cog(std::string_view name)
        {
            auto pair = registered_cogs.emplace(std::string(name), std::unique_ptr<BaseCog>(new T()));
            return *(pair.first->second.get()); 
        }
    private:
        std::unordered_map<std::string, std::unique_ptr<BaseCog>> registered_cogs;
    };

}

#ifndef STRINGIZE
#define STRINGIZE(A) #A
#define STRINGIZE_DEFINED_HERE
#endif

#define __GENERATED_REGISTER_COG(CogName, CogTypename, CogFunctionName) \
    static RAOE::Cogs::BaseCog& __REGISTERED_COG_##CogName = RAOE::Cogs::Registry::Get().register_cog<CogTypename>(STRINGIZE(CogName)); \
    extern "C" const RAOE::Cogs::BaseCog& CogFunctionName() { return __REGISTERED_COG_##CogName; }

#ifdef STRINGIZE_DEFINED_HERE
#undef STRINGIZE
#undef STRINGIZE_DEFINED_HERE
#endif
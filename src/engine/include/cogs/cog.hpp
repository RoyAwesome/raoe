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
    inline namespace _
    {
        template<typename T>
        struct NamedObjectFactory
        {
            std::string name;
            std::function<T*()> factory_func;
        };
    }
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
        friend class Registry;
    protected:
        explicit BaseCog(std::string&& in_name)
            : m_status(ECogStatus::Created)
            , m_name(in_name)
        {
        }     

        template<class T>
            requires std::derived_from<T, Gear> && std::default_initializable<T>
        void register_gear(std::string_view name)
        {
            gear_factories.emplace_back(GearFactory{std::string(name), [](){ return new T(); }});
        }

        void register_gear(std::string_view name, std::function<Gear*()> gear_factory)
        {
            gear_factories.emplace_back(GearFactory{std::string(name), gear_factory});
        };
             
    public:
        ECogStatus status() { return m_status; }
        std::string_view name() { return m_name; }
        virtual void register_gears() = 0;   
        void construct_gears();

        virtual bool is_engine_cog() { return false; }
    private:
        ECogStatus m_status;
        std::string m_name;   

        using GearFactory = NamedObjectFactory<Gear>;
        std::vector<GearFactory> gear_factories;
        
    };

    class Registry
    {
    public:
        using CogRegistryType = std::unordered_map<std::string, std::unique_ptr<BaseCog>>;
        using GearRegistryType = std::unordered_map<std::string, std::tuple<std::unique_ptr<Gear>, BaseCog*>>;

    public:
        static Registry& Get();

        template<class T>
            requires std::derived_from<T, BaseCog> && std::default_initializable<T>
        BaseCog& construct_cog(std::string_view name)
        {
           return construct_cog(name, [](){return new T(); });
        }

        BaseCog& construct_cog(std::string_view name, std::function<BaseCog*()> factory);
        void create_statically_linked_cogs();

        template<class T>
            requires std::derived_from<T, BaseCog> && std::default_initializable<T>
        std::string_view register_cog_static_linked(std::string_view name)
        {
            static_registered_cogs.emplace_back(StaticRegisteredCogFactory{std::string(name), [](){ return new T(); } });
            return name;
        }
        void transition_cogs(ECogStatus transition_to, std::function<void(BaseCog&)> transition_func);

        void transition_cog(std::string cog_name, ECogStatus transition_to, std::function<void(BaseCog&)> transition_func);

        BaseCog* get_cog(std::string_view cog_name);

        Gear* get_gear(std::string_view gear_name);

        Gear& construct_gear(std::string_view name, std::function<Gear*()> factory, BaseCog* owning_cog = nullptr);

        template<class T>
            requires std::derived_from<T, Gear> && std::default_initializable<T>
        Gear& construct_gear(std::string_view name, BaseCog* owning_cog = nullptr)
        {
            return construct_gear(name, [](){ return new T(); }, owning_cog);
        }


        const CogRegistryType& cogs() { return registered_cogs; }
        const GearRegistryType& gears() { return registered_gears; }

    private:
        CogRegistryType registered_cogs;      
        GearRegistryType registered_gears;

        using StaticRegisteredCogFactory = NamedObjectFactory<BaseCog>;      
        std::vector<StaticRegisteredCogFactory> static_registered_cogs;
    };

}

#define __GENERATED_REGISTER_COG(CogName, CogTypename, CogQualifiedName, CogFunctionName) \
    static std::string_view __REGISTERED_COG_##CogName = RAOE::Cogs::Registry::Get().register_cog_static_linked<CogTypename>(CogQualifiedName); \
    extern "C" const char CogFunctionName() { return __REGISTERED_COG_##CogName.front(); }
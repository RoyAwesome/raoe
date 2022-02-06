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
#include <unordered_map>
#include <string>
#include <memory>
#include <concepts>

namespace RAOE
{
    enum class ECogStatus : uint8
    {
        CREATED,
        PREACTIVATE,
        ACTIVATED,
        PRESHUTDOWN,
        SHUTDOWN,
        SHUTDOWN_ENGINE,
    };

    class IEngineCog : public std::enable_shared_from_this<IEngineCog>
    {
    public:
        virtual void activated() = 0;
        virtual void deactivated() {}

        ECogStatus status = ECogStatus::CREATED;   
        std::string name;

        void register_tickfunc(std::function<void()>&& tickfunc);   
    };

    class CogManager
    {
    public:
        using RegistryType = std::unordered_map<std::string, std::shared_ptr<IEngineCog>>;
        friend class Engine;
        template<class T>
            requires std::derived_from<T, IEngineCog>
        friend class StaticLinkedCogFactory;

        static CogManager& Get();

        template<class T>
            requires std::derived_from<T, IEngineCog>
        std::weak_ptr<T> get_cog() const
        {
            return std::static_pointer_cast<T>(get_cog(Voidcraft::Core::NameOf<T>()).lock());
        }

         template<class T>
            requires std::derived_from<T, IEngineCog> && std::default_initializable<T>
        void register_cog()
        {
            register_cog(Voidcraft::Core::NameOf<T>(), std::make_shared<T>());
        }   

        template<class T>
            requires std::derived_from<T, IEngineCog>
        void activate_cog()
        {
            activate_cog(Voidcraft::Core::NameOf<T>());
        } 
        
        //Returns a cog (or null if the cog doesn't exist) 
        std::weak_ptr<IEngineCog> get_cog(std::string_view module_name) const;
    private:
        void register_cog(std::string_view name, std::shared_ptr<IEngineCog>&& module);
       

        void activate_cog(std::string_view name) const;
        void activate_cog(std::weak_ptr<IEngineCog> cog) const;

        void shutdown_cog(std::weak_ptr<IEngineCog> cog, bool engine_shutdown = false) const;

       
 
        RegistryType registry;

        using CogCtorFunc = std::function<std::shared_ptr<IEngineCog>()>;
        struct PendingStaticLinkedCog
        {
            std::string class_name;
            CogCtorFunc factory;
        };

        void register_static_linked_cog(std::string_view cog_definition, std::string_view cog_classname, CogCtorFunc&& cog_func);
        std::unordered_map<std::string, PendingStaticLinkedCog> pending_static_linked_cogs;
    };

    template<class T>
        requires std::derived_from<T, IEngineCog>
    struct StaticLinkedCogFactory
    {
        using CogType = T;

        StaticLinkedCogFactory(std::string_view cog_name)
        {
            CogManager::Get().register_static_linked_cog(
                cog_name, 
                Voidcraft::Core::NameOf<CogType>(), 
                [](){ return std::make_shared<CogType>(); }
            );      
        }   

        int touch()
        {
            return 0;
        }     
    };

    #define REGISTER_COG(cog_name, clazz)                                                                       \
    namespace zzz_RegisterCog__ ## cog_name ## __internal {                                                     \
        static RAOE::StaticLinkedCogFactory<clazz> zzz_registered__##clazz(#cog_name);                          \
    }                                                                                                           \
    extern "C" void __GENERATED__##cog_name() { zzz_RegisterCog__ ## cog_name ## __internal::zzz_registered__##clazz.touch(); }
}
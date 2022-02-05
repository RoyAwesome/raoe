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

#include "engine_cog.hpp"
#include "engine_ticker.hpp"

namespace RAOE
{

    CogManager& CogManager::Get()    
    {
        static CogManager mm;
        return mm;
    }

    std::weak_ptr<IEngineCog> CogManager::get_cog(std::string_view cog_name) const    
    {
        if(registry.contains(std::string(cog_name)))
        {
            return registry.at(std::string(cog_name));
        }
        return std::weak_ptr<IEngineCog>();    
    } 

    void CogManager::register_cog(std::string_view name, std::shared_ptr<IEngineCog>&& cog)    
    {
        cog->name = std::string(name);
        registry.emplace(std::string(name), cog);
    }

    void CogManager::activate_cog(std::string_view name) const
    {
       activate_cog(get_cog(name));
    }

    void CogManager::activate_cog(std::weak_ptr<IEngineCog> cog) const
    {
        if(std::shared_ptr<IEngineCog> cog_ptr = cog.lock())
        {
            if(cog_ptr->status == ECogStatus::SHUTDOWN_ENGINE)
            {
                spdlog::error("Attempting to shutdown a cog that was shutdown at engine shutdown.  We wont be re-activating it");
                Voidcraft::Debug::Break();
                return;
            }

            cog_ptr->status = ECogStatus::PREACTIVATE;
            cog_ptr->activated();
            cog_ptr->status = ECogStatus::ACTIVATED;
        }
    }

    void CogManager::shutdown_cog(std::weak_ptr<IEngineCog> cog, bool engine_shutdown) const    
    {
        if(std::shared_ptr<IEngineCog> cog_ptr = cog.lock())
        {
            cog_ptr->status = ECogStatus::PRESHUTDOWN;
            cog_ptr->deactivated();
            cog_ptr->status = engine_shutdown ? ECogStatus::SHUTDOWN_ENGINE : ECogStatus::SHUTDOWN;
        }    
    }

    void IEngineCog::register_tickfunc(std::function<void()>&& tickfunc)    
    {
        if(auto ticker_ptr = CogManager::Get().get_cog<RAOE::_::Ticker>().lock())
        {
            ticker_ptr->tick_funcs.emplace(name, std::move(tickfunc));
        }
    }
    
}
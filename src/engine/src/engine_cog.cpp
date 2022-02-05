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

namespace RAOE
{

    CogManager& CogManager::Get()    
    {
        static CogManager mm;
        return mm;
    }

    std::weak_ptr<IEngineCog> CogManager::get_cog(std::string_view module_name) const    
    {
        if(registry.contains(std::string(module_name)))
        {
            return registry.at(std::string(module_name));
        }
        return std::weak_ptr<IEngineCog>();    
    } 

    void CogManager::register_cog(std::string_view name, std::shared_ptr<IEngineCog>&& module)    
    {
        registry.emplace(std::string(name), module);
    }

    void CogManager::activate_cog(std::string_view name) const
    {
        if(std::shared_ptr<IEngineCog> cog_ptr = get_cog(name).lock())
        {
            cog_ptr->status = ECogStatus::PREACTIVATE;
            cog_ptr->activated();
            cog_ptr->status = ECogStatus::ACTIVATED;
        }
    }
    
}
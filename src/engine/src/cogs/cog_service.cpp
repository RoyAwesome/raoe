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

#include "cogs/cog_service.hpp"

namespace RAOE::Service
{

    void CogService::transition_cogs(ECogStatus transition_to, std::function<void(BaseCog&)> transition_func)    
    {   
        //Run the transition funcs
        for(const auto& [_, base_cog] : m_cogs)
        {
            if(base_cog && !base_cog->is_engine_cog() && base_cog->status() < transition_to)
            {
                 transition_func(*base_cog.get());
            }           
        }
        //then set the value
        for(const auto& [_, base_cog] : m_cogs)
        {
            if(base_cog && !base_cog->is_engine_cog() && base_cog->status() < transition_to)
            {
                spdlog::info("Transitioning Cog {} from {} to {}", base_cog->name(), base_cog->status(), transition_to);
                base_cog->m_status = transition_to;                
            }            
        }
    }

}
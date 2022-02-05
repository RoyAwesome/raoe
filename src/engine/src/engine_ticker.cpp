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

#include "engine_ticker.hpp"
#include "engine_cog.hpp"

namespace RAOE::_
{

    void Ticker::run_tick()    
    {
        if(status != ECogStatus::ACTIVATED)
        {
            return;
        }

        for(const auto& [cog_name, tick_func] : tick_funcs)
        {
            if(auto cog_ptr = CogManager::Get().get_cog(cog_name).lock())
            {
                if(cog_ptr->status == ECogStatus::ACTIVATED)
                {
                    tick_func();
                }
            }
        }
    
    }

    REGISTER_COG(Ticker)

}
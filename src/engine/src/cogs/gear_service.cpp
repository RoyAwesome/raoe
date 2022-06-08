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


#include "cogs/gear_service.hpp"
#include "resource/resource_service.hpp"

namespace RAOE::Service
{

    void GearService::internal_register_gear(const std::shared_ptr<RAOE::Cogs::Gear>& in_gear)    
    {   
        using ResourceService = RAOE::Resource::Service;
        if(std::shared_ptr<ResourceService> resource_service = engine().get_service<ResourceService>().lock())
        {
            resource_service->emplace_resource(in_gear->tag(), in_gear, RAOE::Resource::TypeTags::Gear);
        }    
    }

}
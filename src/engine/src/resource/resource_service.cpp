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


#include "resource/resource_service.hpp"
#include "resource/handle.hpp"

namespace RAOE::Resource
{

    std::shared_ptr<Handle> Service::get_resource(const Tag& tag)
    {
        return find_or_create_handle(tag);
    }

    std::shared_ptr<Handle> Service::load_resource(const Tag& tag)    
    {   
        auto handle = find_or_create_handle(tag);
        if(!handle->loaded())
        {
            handle->load_resource_synchronously();
        }
        return handle;
    }

    std::shared_ptr<Handle> Service::find_or_create_handle(const Tag& tag)    
    {   
        auto found_handle = handle_map.find(tag);
        if(found_handle != handle_map.end())
        {
            auto weak_handle = found_handle->second;
            if(std::shared_ptr<Handle> strong_handle = weak_handle.lock())
            {
                return strong_handle;
            }
        }        

        std::shared_ptr<Handle> handle = std::shared_ptr<Handle>(new Handle(engine(), tag));
        handle_map.insert_or_assign(tag, handle);
        return handle;
    }


}
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

#include "core.hpp"
#include "resource/resource_service.hpp"
#include "resource/handle.hpp"
#include "resource/iresource.hpp"

namespace RAOE::Resource
{

    std::shared_ptr<Handle> Service::get_resource(const Tag& tag)
    {
        return find_or_create_handle(tag);
    }

    std::weak_ptr<Handle> Service::get_resource_weak(const Tag& tag)    
    {
        auto found_handle = handle_map.find(tag);
        return found_handle != handle_map.find(tag) ? found_handle->second : std::weak_ptr<Handle>();
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

    std::shared_ptr<Handle> Service::emplace_resource(const Tag& tag, std::unique_ptr<IResource>&& resource)    
    {
        auto handle = find_or_create_handle(tag);
        //if we have a valid resource at this handle, wipe it out, as we will be emplacing a new resource here.
        if(handle->m_resource) 
        {
            handle->m_resource.reset();
        }
        handle->m_resource = std::forward<std::unique_ptr<IResource>>(resource);

        //pin the emplaced resource
        m_pinned_resources.insert_or_assign(tag, handle);

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

    void Service::pin_resource(const Tag& tag)    
    {   
        auto found_handle = handle_map.find(tag);
        if(found_handle != handle_map.end())
        {
            if(auto shared_handle = found_handle->second.lock())
            {
                m_pinned_resources.insert_or_assign(tag, found_handle->second.lock());
                return;
            }
           
        }
        spdlog::error("Service::pin_resource: unable to pin resource {}, handle not valid.", tag);
    }

}

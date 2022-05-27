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
#include "core.hpp"
#include "engine.hpp"
#include "resource/handle.hpp"
#include "resource/iresource.hpp"

#include "console/command.hpp"

namespace RAOE::Resource
{

    std::shared_ptr<Handle> Service::get_resource(const Tag& tag)
    {
        return find_or_create_handle(tag);
    }

    std::weak_ptr<Handle> Service::get_resource_weak(const Tag& tag)    
    {
        auto found_handle = m_handle_map.find(tag);
        return found_handle != m_handle_map.find(tag) ? found_handle->second : std::weak_ptr<Handle>();
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

    std::shared_ptr<Handle> Service::emplace_resource(const Tag& tag, std::unique_ptr<IResource>& resource)    
    {
        auto handle = find_or_create_handle(tag);
        //if we have a valid resource at this handle, wipe it out, as we will be emplacing a new resource here.
        if(handle->m_resource) 
        {
            handle->m_resource.reset();
        }
        handle->m_resource = resource;
        //Pin the resource
        pin_resource(handle.get());

        return handle;
    }

    std::shared_ptr<Handle> Service::find_or_create_handle(const Tag& tag)    
    {   
        auto found_handle = m_handle_map.find(tag);
        if(found_handle != m_handle_map.end())
        {
            auto weak_handle = found_handle->second;
            if(std::shared_ptr<Handle> strong_handle = weak_handle.lock())
            {
                return strong_handle;
            }
        }        

        std::shared_ptr<Handle> handle = std::shared_ptr<Handle>(new Handle(*this, tag));
        m_handle_map.insert_or_assign(tag, handle);
        return handle;
    }
   
    void Service::pin_resource(Handle* handle)    
    {   
        if(handle)
        {
            auto found_handle = m_handle_map.find(handle->tag());
            if(found_handle != m_handle_map.end())
            {
                if(auto shared_handle = found_handle->second.lock())
                {
                    m_pinned_resources.insert_or_assign(handle->tag(), found_handle->second.lock());
                    return;
                }            
            }
        }        
        spdlog::error("Service::pin_resource: unable to pin resource {}, handle not valid.", handle->tag());
    }


    void Service::on_handle_destroyed(Handle* destroying_handle)    
    {        
    
    }

    void print_handle_information(Engine& engine)
    {
        Service* resource_service = engine.get_service<Service>();
        spdlog::info("Resource Service Info");
        for(const auto& [tag, handle] : resource_service->m_handle_map)
        {
            if(auto strong_handle = handle.lock())
            {
                spdlog::info("\t{} - (loaded? {} pinned? {})", tag, strong_handle->loaded(), resource_service->m_pinned_resources.contains(tag));
            }            
        }
        spdlog::info("End Resource Service Info");
    }
    using AutoRegisterConsoleCommand = RAOE::Console::AutoRegisterConsoleCommand;
    static AutoRegisterConsoleCommand print_handle_info_command = RAOE::Console::CreateConsoleCommand(
        "print_resource_info",
        "Prints resources to the console, in the format [Tag] - [LoadStatus]",
        print_handle_information
    );

    

}

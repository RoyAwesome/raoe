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

#include "resource/service.hpp"
#include "core.hpp"
#include "engine.hpp"
#include "resource/handle.hpp"
#include "resource/iresource.hpp"
#include "resource/type.hpp"

#include "console/command.hpp"

#include "resource/assets/text_asset.hpp"

namespace RAOE::Resource
{
    namespace TypeTags
    {
        const Tag Type("raoe:type/type");
        const Tag Unknown("raoe:type/unknown");
        const Tag Cog("raoe:type/cog");
        const Tag Gear("raoe:type/gear");
        const Tag Loader("raoe:type/loader");
    }
    namespace Asset::Tags
    {
        const Tag TextLoader("raoe:loader/text");
        const Tag TextAsset("raoe:type/text");
    }


    Service::Service(RAOE::Engine& in_engine)
        : IService(in_engine)
    {
        //Create the basic types that are always available to the resource system
        create_resource_type(TypeTags::Type);
        create_resource_type(TypeTags::Unknown);
        create_resource_type(TypeTags::Cog);
        create_resource_type(TypeTags::Gear);
        create_resource_type(TypeTags::Loader);

        //emplace the basic loaders that are always available
        std::shared_ptr<Handle> loader = emplaced_owned_resource(Asset::Tags::TextLoader, std::static_pointer_cast<IResource>(std::make_shared<Asset::TextAssetLoader>()), TypeTags::Loader);
        loader->pin();
        
        if(auto text_asset = create_resource_type(Asset::Tags::TextAsset).lock())
        {
            text_asset->get<Type>().lock()->add_loader(loader->get<ILoader>());
        }

    }

    std::shared_ptr<Handle> Service::get_resource(const Tag& tag)
    {
        return find_or_create_handle(tag);
    }

    std::weak_ptr<Handle> Service::get_resource_weak(const Tag& tag) const    
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

    std::shared_ptr<Handle> Service::emplace_resource(const Tag& tag, const std::weak_ptr<IResource>& resource, const Tag& resource_type)    
    {
        auto handle = find_or_create_handle(tag);  
        if(m_owned_resources.contains(resource_type))
        {
            handle->m_resource_type = resource_type;   
        }
        else
        {
            spdlog::warn("Tried to emplace resource with unknown type {}.  Defaulting to raoe:type/unknown", resource_type);
        }
  
        handle->m_resource = resource;
        //Pin the resource
        pin_resource(handle.get());

        return handle;
    }

    std::shared_ptr<Handle> Service::emplaced_owned_resource(const Tag& tag, const std::shared_ptr<IResource>& resource, const Tag& resource_type)    
    {  
        m_owned_resources.insert_or_assign(tag, resource);
        return emplace_resource(tag, resource, resource_type);    
    }

    std::weak_ptr<Handle> Service::create_resource_type(const Tag& tag)    
    {  
        auto handle = find_or_create_handle(tag);
        std::shared_ptr<IResource> resource = std::make_shared<Type>(tag);
        m_owned_resources.insert_or_assign(tag, resource);
        handle->m_resource_type = TypeTags::Type;
        handle->m_resource = resource;
        handle->pin();  

        return handle;
    }

    const Type& Service::unknown_type() const    
    {   
        if(auto resource = get_resource_weak(TypeTags::Unknown).lock())
        {
            return *resource->get<Type>().lock();
        }
       
        std::terminate();//crash
    
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

        std::shared_ptr<Handle> handle = std::shared_ptr<Handle>(new Handle(*this, tag, Tag("raoe:type/unknown")));
        m_handle_map.insert_or_assign(tag, handle);
        return handle;
    }
   
    void Service::pin_resource(Handle* handle)    
    {   
        if(handle != nullptr)
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

    void Service::manage_resource(const Tag& tag, std::shared_ptr<IResource> resource, const Tag& resource_type)  
    {   
        auto handle = find_or_create_handle(tag);
        m_owned_resources.insert_or_assign(tag, resource);
        handle->m_resource_type = resource_type;
        handle->m_resource = resource;
        handle->pin();            
    }

    void print_handle_information(Engine& engine)
    {
        if(std::shared_ptr<Service> resource_service = engine.get_service<Service>().lock())
        {
            spdlog::info("   | {:<25}|{:<25}|{:^8}|{:^8}|", "Name", "Type", "Loaded", "Pinned");
            for(const auto& [tag, handle] : resource_service->m_handle_map)
            {
                if(auto strong_handle = handle.lock())
                {
                    spdlog::info("   | {:<25}|{:<25}|{:^8}|{:^8}|", tag, strong_handle->resource_type(), strong_handle->loaded(), resource_service->m_pinned_resources.contains(tag));
                }            
            }
        }      
        else
        {            
            spdlog::error("print_handle_information: Unable to find resource service");
        }  
    }
    using AutoRegisterConsoleCommand = RAOE::Console::AutoRegisterConsoleCommand;
    static const AutoRegisterConsoleCommand print_handle_info_command = RAOE::Console::CreateConsoleCommand(
        "print_resource_info",
        "Prints resources to the console, in the format [Tag] - [LoadStatus]",
        print_handle_information
    );
}

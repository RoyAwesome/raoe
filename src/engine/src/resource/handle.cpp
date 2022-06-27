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


#include "resource/handle.hpp"
#include "resource/iresource.hpp"
#include "resource/service.hpp"
#include "engine.hpp"
#include "resource/type.hpp"

namespace RAOE::Resource
{
    bool Handle::loaded() const
    {
        if(auto hard_resource = m_resource.lock())
        {
            return hard_resource->loadstatus() == IResource::ELoadStatus::Loaded;
        }
        return false;
    }

    void Handle::pin()    
    {
        service()->pin_resource(this);
    }

    void Handle::load_resource_synchronously()    
    {   
        
    
    }

    std::shared_ptr<Handle> Handle::type_handle() const
    {
        return service()->get_resource(resource_type());
    }

    std::shared_ptr<Type> Handle::type() const    
    {
        if(auto locked_handle = type_handle(); locked_handle->loaded())
        {
            return locked_handle->get<Type>().lock();
        }
        return {};    
    }

    const Type& Handle::type_ref() const    
    {
        if(auto locked_handle = type_handle())
        {
            if(locked_handle->loaded())
            {
                return *locked_handle->get<Type>().lock();
            }
        }

        return service()->unknown_type();
    }

    Handle::~Handle()    
    {   
        service()->on_handle_destroyed(this);
    
    }

    RAOE::Engine& Handle::engine() const { return m_service.engine(); } 
   
}



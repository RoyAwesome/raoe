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
#include "resource/resource_service.hpp"
#include "engine.hpp"

namespace RAOE::Resource
{
    bool Handle::loaded() const
    {
        return m_resource && m_resource->loadstatus() == IResource::ELoadStatus::Loaded;
    }

    void Handle::pin()    
    {
        engine().get_service<RAOE::Resource::Service>()->pin_resource(tag());
    }
}



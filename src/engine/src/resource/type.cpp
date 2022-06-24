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

#include "resource/type.hpp"

namespace RAOE::Resource
{
    void Type::add_loader(const std::weak_ptr<ILoader>& loader)
    {    
        if(loader.expired())
        {
            spdlog::error("Attempted to add a loader to type {} but it was expired", tag());
            return;
        }
        //clean up the loaders, removing any if they've expired
        m_loaders.erase(std::remove_if(m_loaders.begin(), m_loaders.end(), [](const std::weak_ptr<ILoader>& ptr) { return ptr.expired(); }), m_loaders.end());
    
        m_loaders.emplace_back(loader);
    }

    void Type::remove_loader(const std::weak_ptr<ILoader>& loader)
    {    
        //clean up the loaders, removing any if they've expired
        m_loaders.erase(std::remove_if(m_loaders.begin(), m_loaders.end(), [](const std::weak_ptr<ILoader>& ptr) { return ptr.expired(); }), m_loaders.end());
    
         //clean up the loaders, removing any if they've expired
        m_loaders.erase(std::remove_if(m_loaders.begin(), m_loaders.end(), [loader](const std::weak_ptr<ILoader>& ptr) { return ptr.lock().get() == loader.lock().get(); }), m_loaders.end());
    
    }
}
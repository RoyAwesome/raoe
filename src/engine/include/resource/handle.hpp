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

#pragma once
#include "resource/tag.hpp"
#include "resource/iresource.hpp"
#include <optional>
#include <memory>

namespace RAOE
{
    class Engine;
}

namespace RAOE::Resource
{
    class Service;

    class Handle
    {
    public:
        using ResourcePointerRef = std::optional<std::reference_wrapper<std::unique_ptr<IResource>>>;
        friend class Service;

        ~Handle();

        const Tag& tag() const { return m_tag; }
        RAOE::Engine& engine() const;
        Service* service() const { return &m_service; }
        bool loaded() const;

        IResource* get() const;

        template<typename T>
        T* get() const
        {
            return dynamic_cast<T*>(get());
        }

        void pin();

        void load_resource_synchronously() {} ;
    private:
        Handle(Service& in_service, const Tag& in_tag)
            : m_service(in_service)
            , m_tag(in_tag)
            , m_resource()
        {            
        }
        
        Handle(Service& in_service, const Tag& in_tag, std::unique_ptr<IResource>& in_resource)
            : m_service(in_service)
            , m_tag(in_tag)
            , m_resource(in_resource)
        {
        }
        
        Service& m_service;
        Tag m_tag;
        ResourcePointerRef m_resource;
    };
}
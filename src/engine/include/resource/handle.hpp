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
        friend class Service;

        Handle(const Handle&) = delete; //Handles cannot be copied.  They are passed around as shared pointers
        Handle& operator=(const Handle&) = delete; //Handles cannot be copied.  They are passed around as shared pointers
        
        Handle(Handle&&) = delete; //Handles cannot be moved.  They are passed around as shared pointers       
        Handle& operator=(Handle&&) = delete; //Handles cannot be moved.  They are passed around as shared pointers

        ~Handle();

        [[nodiscard]] const Tag& tag() const { return m_tag; }
        [[nodiscard]] const Tag& resource_type() const { return m_resource_type; }
        [[nodiscard]] std::weak_ptr<Handle> get_resource_type_handle() const;
        [[nodiscard]] RAOE::Engine& engine() const;
        [[nodiscard]] Service* service() const { return &m_service; }
        [[nodiscard]] bool loaded() const;

        [[nodiscard]] std::weak_ptr<IResource> get() const { return m_resource; }

        template<std::derived_from<IResource> T>
        [[nodiscard]] std::weak_ptr<T> get() const { return std::dynamic_pointer_cast<T>(get().lock()); }

        void pin();

        void load_resource_synchronously();
    private:
        Handle(Service& in_service, Tag in_tag, Tag in_type)
            : m_service(in_service)
            , m_tag(std::move(in_tag))
            , m_resource_type(std::move(in_type))
        {            
        }
        
        Handle(Service& in_service, Tag in_tag, std::weak_ptr<IResource> in_resource, Tag in_type)
            : m_service(in_service)
            , m_tag(std::move(in_tag))
            , m_resource(std::move(in_resource))
            , m_resource_type(std::move(in_type))
        {
        }
        
        Service& m_service;
        Tag m_tag;
        std::weak_ptr<IResource> m_resource;
        Tag m_resource_type;
    };
}
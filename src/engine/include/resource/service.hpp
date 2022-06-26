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

#include "core.hpp"
#include "services/iservice.hpp"
#include "resource/tag.hpp"
#include "resource/iresource.hpp"

#include <unordered_map>
#include <memory>

namespace RAOE::Resource
{
    class Handle;
    class Type;

    namespace TypeTags
    {   
        extern const Tag Type;
        extern const Tag Unknown;
        extern const Tag Cog;
        extern const Tag Gear;
        extern const Tag Loader;
    }

    class Service : public RAOE::Service::IService
    {
    public:
        friend class Handle;
        friend void print_handle_information(Engine&);
        Service(RAOE::Engine& in_engine);

        [[nodiscard]] std::shared_ptr<Handle> get_resource(const Tag& tag);
        [[nodiscard]] std::weak_ptr<Handle> get_resource_weak(const Tag& tag) const;

        std::shared_ptr<Handle> load_resource(const Tag& tag);
        std::shared_ptr<Handle> emplace_resource(const Tag& tag, const std::weak_ptr<IResource>& resource, const Tag& resource_type);
        std::shared_ptr<Handle> emplaced_owned_resource(const Tag& tag, const std::shared_ptr<IResource>& resource, const Tag& resource_type)  ;


        std::weak_ptr<Handle> create_resource_type(const Tag& tag);
        

        [[nodiscard]] const Type& unknown_type() const;
    private:
        std::shared_ptr<Handle> find_or_create_handle(const Tag& tag);
        void pin_resource(Handle* handle);
        void on_handle_destroyed(Handle* destroying_handle);
        void manage_resource(const Tag& tag, std::shared_ptr<IResource> resource, const Tag& resource_type);

        std::unordered_map<Tag, std::weak_ptr<Handle>> m_handle_map;
        std::unordered_map<Tag, std::shared_ptr<Handle>> m_pinned_resources;
        std::unordered_map<Tag, std::shared_ptr<IResource>> m_owned_resources;
    };
}
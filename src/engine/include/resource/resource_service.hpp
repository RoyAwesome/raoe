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

    class Service : public RAOE::Service::IService
    {
    public:
        friend class Handle;
        Service(RAOE::Engine& in_engine)
            : IService(in_engine)
        {
        }

        std::shared_ptr<Handle> get_resource(const Tag& tag);
        std::weak_ptr<Handle> get_resource_weak(const Tag& tag);
        
        std::shared_ptr<Handle> load_resource(const Tag& tag);

        std::shared_ptr<Handle> emplace_resource(const Tag& tag, std::unique_ptr<IResource>&& resource);



    private:
        std::shared_ptr<Handle> find_or_create_handle(const Tag& tag);
        void pin_resource(const Tag& tag);

        std::unordered_map<Tag, std::weak_ptr<Handle>> handle_map;
        std::unordered_map<Tag, std::shared_ptr<Handle>> m_pinned_resources;
    };
}
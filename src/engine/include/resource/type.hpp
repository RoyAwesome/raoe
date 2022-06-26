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
#include "resource/iresource.hpp"
#include "resource/tag.hpp"

namespace RAOE::Resource
{
    class ILoader;

    class Type : public IResource
    {
    public:
        Type(Tag in_tag)
            : m_type_tag(std::move(in_tag))
        {            
        }
        Type() = delete;
        Type(const Type&) = default;
        Type(Type&&) = default;
        ~Type() override = default;

        Type& operator=(const Type&) = default;
        Type& operator=(Type&&) = default;

        [[nodiscard]] const Tag& type_tag() const { return m_type_tag; }
        [[nodiscard]] const Tag& tag() const { return type_tag(); }

        //BEGIN: IResource Interface
        [[nodiscard]] RAOE::Resource::IResource::ELoadStatus loadstatus() const override { return RAOE::Resource::IResource::ELoadStatus::Loaded; }
        //END: IResource Interface

        void add_loader(const std::weak_ptr<ILoader>& loader);
        void remove_loader(const std::weak_ptr<ILoader>& loader);

        [[nodiscard]] const std::vector<std::weak_ptr<ILoader>>& loaders() const { return m_loaders; }
    private:
        Tag m_type_tag;
        std::vector<std::weak_ptr<ILoader>> m_loaders;
    };
}
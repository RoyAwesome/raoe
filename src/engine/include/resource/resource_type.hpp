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
    class Type : public IResource
    {
    public:
        Type(const Tag& in_tag)
            : m_type_tag(in_tag)
        {            
        }

        const Tag& type_tag() const { return m_type_tag; }
        const Tag& tag() const { return type_tag(); }

        //BEGIN: IResource Interface
        virtual RAOE::Resource::IResource::ELoadStatus loadstatus() const override { return RAOE::Resource::IResource::ELoadStatus::Loaded; }
        //END: IResource Interface
    private:
        Tag m_type_tag;
    };
}
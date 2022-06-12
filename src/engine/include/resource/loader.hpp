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

#include "resource/iresource.hpp"
#include <vector>
#include <string>
#include <memory>

namespace RAOE::Resource
{
    template<is_resource T>
    class ILoader : public IResource
    {
    public:
        virtual ELoadStatus loadstatus() const override { return ELoadStatus::Loaded; }
        virtual std::shared_ptr<T> load_resource(const std::istream& data_stream) = 0;
    private:
        //File extensions for this loader
        std::vector<std::string> m_extensions;
    };
}
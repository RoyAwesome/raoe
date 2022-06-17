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
#include "resource/loader.hpp"
#include <istream>
#include <sstream>
#include <string>
#include <string_view>

namespace RAOE::Resource
{
    class Tag;
}

namespace RAOE::Resource::Asset
{  
    namespace Tags
    {
        extern const Tag TextLoader;
    }
    class TextAsset : public IResource
    {
    public:
        TextAsset(std::string_view in_string)
            : m_contents(in_string)
        {}

        virtual ELoadStatus loadstatus() const { return ELoadStatus::Loaded; }    

        std::string_view contents() { return m_contents; }

    private:
        std::string m_contents;
    };

    class TextAssetLoader : public ILoader<TextAsset>
    {
    public:
        
        virtual std::shared_ptr<TextAsset> load_resource(const std::istream& data_stream) override
        {
            std::stringstream buffer;
            buffer << data_stream.rdbuf();

            return std::make_shared<TextAsset>(buffer.view());;
        }
    };
}
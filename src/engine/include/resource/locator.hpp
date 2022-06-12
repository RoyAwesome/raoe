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

#include "resource/service.hpp"
#include "resource/tag.hpp"
#include "resource/handle.hpp"
#include <filesystem>
#include <string_view>

namespace RAOE::Resource
{
    template<typename T>
    concept Locator = requires(T t, Service& s, Tag tag)
    {
        { t(s, tag) } -> std::same_as<std::filesystem::path>;
    };

    struct DefaultLocator
    {
        std::filesystem::path operator()(Service& service, Tag tag)
        {
            using namespace std::literals::string_view_literals;
            std::string_view cog_folder = tag.prefix();
            if(cog_folder == "raoe"sv)
            {
                cog_folder = "engine"sv;
            }
            return std::filesystem::path(cog_folder) / std::filesystem::path(tag.identifier());
        }
    };

    struct ResolvedResource
    {
        std::filesystem::path resolved_path;
        RAOE::Resource::Tag filetype;
    };

    template<Locator L = DefaultLocator>
    class ResourceResolver
    {
    public:
        void operator()(std::shared_ptr<Handle> handle, std::output_iterator<ResolvedResource> auto out_itr)
        {
            L locator_func {};
            std::filesystem::path unresolved_path = locator_func(*handle->service(), handle->tag());

            std::filesystem::path parent_path = unresolved_path.parent_path();

            std::filesystem::path actual_path = std::filesystem::current_path() / "assets" / parent_path;
            if(!std::filesystem::is_directory(actual_path))
            {
                return;
            }

            //search up all the files at the unresolved path's directory
            for(auto file : std::filesystem::directory_iterator(actual_path))
            {
                if(!file.is_regular_file())
                {
                    continue;                   
                }
                if(file.path().stem() != unresolved_path.stem())
                {
                    continue;
                }
                auto extension = file.path().extension();

                //Find the file type that matches this extension

                //build and return the resolved info
                *(++out_itr) = ResolvedResource { file.path(), TypeTags::Unknown };
            }
        }
    };

}
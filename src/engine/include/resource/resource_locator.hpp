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

#include "resource_service.hpp"
#include "tag.hpp"
#include <filesystem>
#include <string_view>

namespace RAOE::Resource
{
    template<typename T>
    concept Locator = requires(T t, Service& s, Tag tag)
    {
        { t(s, tag) } -> std::same_as<std::filesystem::path>;
    };

    inline std::filesystem::path default_locator(Service& service, Tag tag)
    {
        using namespace std::literals::string_view_literals;
        std::string_view cog_folder = tag.prefix();
        if(cog_folder == "raoe"sv)
        {
            cog_folder = "engine"sv;
        }
        return std::filesystem::path(cog_folder) / std::filesystem::path(tag.identifier()));
    }

}
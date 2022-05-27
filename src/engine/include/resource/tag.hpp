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

#include <string>
#include <string_view>

namespace RAOE::Resource
{
    class Tag
    {
        friend struct std::hash<RAOE::Resource::Tag>;
    public:
        explicit Tag(std::string_view in_tag);
        Tag(std::string_view prefix, std::string_view path);

        std::string_view prefix() const;
        std::string_view identifier() const;

        auto operator<=>(const Tag& rhs) const = default;

        operator std::string_view() const { return m_tag; }        
        operator bool() const {return prefix().length() != 0 && identifier().length() != 0; }

        static std::string_view DefaultPrefix;
    private:
        std::string m_tag;
    };
}

namespace std
{
    template<> struct hash<RAOE::Resource::Tag>
    {
        std::size_t operator()(RAOE::Resource::Tag const& s) const
        {
            return std::hash<std::string>{}(s.m_tag);
        }
    };
}
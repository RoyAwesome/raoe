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

#include "types.hpp"

#include <concepts>
#include <string_view>
#include <charconv>
#include "typeinfo/typename.hpp"

namespace raoe::string
{
    template<typename T>
    concept from_stringable = requires (std::string_view a, T b)
    {   
        from_string(a, b);
    };

    template<typename T>
    concept real = std::integral<T> || std::floating_point<T>;

    inline bool from_string(std::string_view arg, real auto& value)
    {        
        auto result = std::from_chars(arg.data(), arg.data() + arg.size(), value);
        const bool success = result.ec != std::errc::invalid_argument;     
        return success;      
    }

    //Specialization for strings.  This is because they do not need to be parsed
    inline bool from_string(std::string_view arg, std::string& value)
    {
        value = std::string(arg);
        return true;
    }

    inline bool from_string(std::string_view arg, std::string_view& value)
    {
        value = arg;
        return true;
    }
}
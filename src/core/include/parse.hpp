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
#include <tuple>
#include <cctype>
#include "types.hpp"

namespace raoe::core
{
    inline namespace _
    {
        template<typename Out>
        void parse_split(std::string_view from, Out out_itr)
        {          
            //Set up the search parameters.  
            using ScanFunc_T = std::function<bool(std::string_view, int32)>;

            //Escaped checks if there is a \ before the cursor character (used when escaping sequences)
            ScanFunc_T is_escaped = [](std::string_view sv, int32 cursor) 
            {
                const bool out_of_range = (cursor - 1) < 0;
                return !out_of_range && sv[cursor-1] == '\\'; 
            };
            //Checks for if the cursor is on a whitespace char
            ScanFunc_T is_whitespace = [](std::string_view sv, int32 cursor) 
            { 
                return !!(std::isblank(sv.at(cursor))); 
            };
            //Parse ", but if we see a \" skip that.  
            ScanFunc_T is_quote = [&is_escaped](std::string_view sv, int32 cursor) -> bool 
            { 
                return sv[cursor] == '\"' && !is_escaped(sv, cursor); 
            }; 
            ScanFunc_T is_control = [&is_quote, &is_whitespace](std::string_view sv, int32 cursor) 
            { 
                return is_whitespace(sv, cursor) || is_quote(sv, cursor);
            };

            int32 cursor = 0;   
            while(cursor < from.length())
            {
                //Walk the cursor up to the first non-control character
                while(cursor < from.length() && is_control(from, cursor))
                {
                    cursor++;
                }

                //if we are at the end of the string, we're done
                if(cursor >= from.length())
                {
                    return;
                }

                int32 start = cursor;
                //Determine if the character right before the cursor is what we are looking for
                ScanFunc_T search_query = is_whitespace;
                if(((cursor - 1) >= 0) && is_quote(from, cursor - 1))
                {
                    search_query = is_quote;
                }

                //Walk the cursor forward until we match what we are looking for (or we hit the end of the string)
                while(cursor < from.length() && !search_query(from, cursor))
                {
                    cursor++;
                }             

                //substring from start to cursor, add it to the out iterator   
                *out_itr++ = from.substr(start, cursor - start);   
            }  
        }

        std::vector<std::string_view> parse_split(std::string_view from)
        {
            std::vector<std::string_view> elems;
            parse_split(from, std::back_inserter(elems));
            return elems;
        }
    }


    template<typename... Args>
    std::tuple<Args...> parse(std::string_view str)
    {

    }
}
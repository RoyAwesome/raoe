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

#include <algorithm>
#include <locale>
#include <cctype>
#include <sstream>
#include <iterator>
#include <vector>

namespace raoe::core
{    
    //Left Trim a string, inline.  Removes all whitespace characters from the left side of the string, modifying it in place
    static inline void ltrim(std::string& s)
    {
        s.erase(
            s.begin(), 
            std::find_if(s.begin(), s.end(), [](unsigned char ch){
            return !std::isspace(ch);
            })
        );
    }

    //Right Trim a string, inline.  Removes all whitespace characters from the right side of the string, modifying it in place  
    static inline void rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch){
            return !std::isspace(ch);
        }).base(),
        s.end());
    }

    //Trim a string, inline.  Removes all whitespace characters from the right and left side of the string, modifying it in place  
    static inline void trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
    }

    //Right Trim a string, copy.  Removes all whitespace characters from the right side of the string, returning a copy of the original string
    static inline std::string rtrim_c(std::string s)
    {
        rtrim(s);
        return s;
    }

    //Left Trim a string, copy.  Removes all whitespace characters from the left side of the string, returning a copy of the original string
    static inline std::string ltrim_c(std::string s)
    {
        ltrim(s);
        return s;
    }

    //Trim a string, copy.  Removes all whitespace characters from the left and right side of the string, returning a copy of the original string
    static inline std::string trim_c(std::string s)
    {
        rtrim(s);
        ltrim(s);
        return s;
    }
    template<typename Out>
    static inline void split(const std::string& s, char delim, Out result)
    {
        std::istringstream iss(s);
        std::string item;
        while(std::getline(iss, item, delim))
        {
            if(!item.empty())
            {
                *result++ = item;
            }
        }
    }

    static inline std::vector<std::string> split(const std::string& s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }
}
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

#include "resource/tag.hpp"
#include "core.hpp"
#include "ctre.hpp"


using namespace std::literals::string_view_literals;

namespace RAOE::Resource
{   
    std::string_view Tag::DefaultPrefix = "raoe"sv;
    Tag::Tag(std::string_view in_tag)
        : m_tag(in_tag)
    {        
        const bool has_colon_no_prefix = !m_tag.starts_with(':') && prefix().length() == 0;
        const bool has_no_colon =  m_tag.find_first_of(':') == std::string::npos;
        if(has_no_colon || has_colon_no_prefix)
        {
            m_tag =  fmt::format("{}:{}", DefaultPrefix, m_tag);
        }     
        //Validate the prefix
        {        
            auto colon_pos = m_tag.find_first_of(':');         
            auto prefix_matcher = ctre::match<"[a-zA-Z0-9_\\-\\.]*$">;
            if(!prefix_matcher(m_tag.begin(), m_tag.begin() + colon_pos))
            {
                m_tag = "";
                return;
            }            
        }

        //Validate the identifier
        {
            auto colon_pos = m_tag.find_first_of(':');           
            auto ident_matcher = ctre::match<"[a-zA-Z0-9_\\-\\.\\/]*$">;

            if(!ident_matcher(m_tag.begin() + colon_pos + 1, m_tag.end()))
            {
                m_tag = "";
                return;
            }            
        }
        //Check if we're just a colon.  That means both the prefix and the ident failed to parse, so just zero us out
        if(m_tag == ":")
        {
            m_tag = "";
            return;
        }
    }

    Tag::Tag(std::string_view prefix, std::string_view path)    
        : Tag(std::string(prefix) + ":" + std::string(path))    
    {
    }

    std::string_view Tag::prefix() const    
    {   
        if(m_tag.find(':') == std::string::npos)
        {
            return ""sv;
        }

        std::string_view tag_as_sv = *this;
        return tag_as_sv.substr(0, tag_as_sv.find_first_of(':'));    
    }

    std::string_view Tag::identifier() const    
    {   
        if(m_tag.find(':') == std::string::npos)
        {
            return ""sv;
        }

        std::string_view tag_as_sv = *this;
        return tag_as_sv.substr(tag_as_sv.find_first_of(':') + 1); 
    }

}

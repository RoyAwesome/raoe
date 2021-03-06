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

#include <cstdint>
#include <type_traits>

using uint8 = uint8_t;  
using uint16 = uint16_t;   
using uint32 = uint32_t;  
using uint64 = uint64_t;   

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;


namespace raoe
{
    template<typename T>
    struct static_false : std::false_type {};
}
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

#include "core.hpp"
#include "glm/glm.hpp"
namespace flecs
{
    class world;
}

namespace RAOE::Framework
{      


    struct transform2d
    {
        glm::vec2 location = glm::vec2(0,0);
        float rotation = 0;
        glm::vec2 scale = glm::vec2(1, 1);
        int32 z_order = 0;
    };
}
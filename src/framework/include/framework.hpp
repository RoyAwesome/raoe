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

namespace RAOE
{
    class Engine;
}

namespace RAOE::Resource
{
    class Tag;
}

namespace RAOE::Framework
{
    namespace Tags
    {
        extern const RAOE::Resource::Tag GameType;
    }

    

    class IGame : public RAOE::Resource::IResource
    {
    public:
        explicit IGame(RAOE::Engine& in_engine)
            : m_engine(in_engine)
        {

        }
        [[nodiscard]] ELoadStatus loadstatus() const override { return ELoadStatus::Loaded; }   

        virtual void begin() = 0;

        [[nodiscard]] const RAOE::Engine& engine() const { return m_engine; }
    private:
        RAOE::Engine& m_engine;
    };
}
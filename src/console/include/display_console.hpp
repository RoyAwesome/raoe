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
#include <string>
#include "spdlog/sinks/ringbuffer_sink.h"

namespace RAOE
{
    class Engine;
}

namespace RAOE::Console
{
    class DisplayConsole
    {
    public:
        DisplayConsole(RAOE::Engine&);
        ~DisplayConsole();

        void Draw(std::string title, bool* p_open);

        void ExecCommand(const std::string& command_line);

        RAOE::Engine& engine() { return m_engine; }

        //TODO: Module level protection
    //INTERNAL:
        int32 history_pos;        
        std::vector<std::string> history;
    private:
        std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> ring_buffer;
        std::string input_buffer;

        RAOE::Engine& m_engine;
    };
}
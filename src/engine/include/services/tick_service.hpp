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

#include "core_minimal.hpp"
#include "services/iservice.hpp"

namespace RAOE::Service
{
    class TickService : public IService
    {
        using Super = IService;
    public:
        TickService(Engine& in_engine)
        : IService(in_engine)
        {            
        }

        void register_tickfunc(std::function<void()>&& tickfunc)
        {
            tick_funcs.emplace_back(std::move(tickfunc));
        }

        void request_exit() { exit_requested = true; }
        bool should_exit() const { return exit_requested; }

        friend class RAOE::Engine;
    private:
        void run_frame()
        {
            for(const auto& tick_func : tick_funcs)
            {
                tick_func();
            }
        }

        bool exit_requested = false;
        std::vector<std::function<void()>> tick_funcs;
    };
}
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

#include <concepts>

namespace RAOE
{
    class Engine;
    namespace Service
    {
        class IService;
    }

    template<typename T>
    concept has_engine_ref = std::constructible_from<T, RAOE::Engine&> && requires(T t) 
    {
        { t.engine() } -> std::same_as<RAOE::Engine&>;
    };

    template<typename T>
    concept is_service = std::derived_from<T, RAOE::Service::IService> && has_engine_ref<T>;
}
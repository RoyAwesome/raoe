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
#include <tuple>
#include <type_traits>

namespace raoe
{
    template<std::size_t N, typename... T, std::size_t... I>
    std::tuple<std::tuple_element_t<N+I, std::tuple<T...>>...> sub(std::index_sequence<I...>);

    template<std::size_t N, typename... T>
    using subpack = decltype(sub<N, T...>(std::make_index_sequence<sizeof...(T) - N>{}));
}

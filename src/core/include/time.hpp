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

#include <functional>

namespace raoe::core::time
{
#if USE_DOUBLE_PRECISION_TIME
    using time_s = double;
#else 
    using time_s = float;
#endif

    using time_func = std::function<time_s()>;

    time_s time_since(time_s in_t, time_func time_func)
    {
        return time_func() - in_t;
    }
}
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

#include "resource/handle.hpp"
#include <coroutine>

namespace RAOE::Resource
{
    inline auto handle_loaded(std::shared_ptr<RAOE::Resource::Handle> in_handle)
    {
         struct awaiter
            {
                awaiter(std::shared_ptr<RAOE::Resource::Handle> in_handle)  noexcept
                    : m_handle(std::move(in_handle))
                {}

                bool await_ready() const noexcept
                {
                    return m_handle->loaded();
                }

                bool await_suspend(std::coroutine_handle<> awaiting_coro) noexcept
                {
                    return m_handle->loaded();
                }

                void await_resume() noexcept {}

                std::shared_ptr<RAOE::Resource::Handle> m_handle;
            };

        return awaiter { std::move(in_handle) };
    }
}
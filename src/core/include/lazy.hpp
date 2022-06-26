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

//an implementation of the std::lazy coroutine from the P2506R0 paper, with modifications

#include <coroutine>
#include <memory>
#include <type_traits>
#include <utility>
#include <concepts>
#include "types.hpp"

namespace raoe
{
    //Allocators
    inline namespace _
    {
        struct alignas(__STDCPP_DEFAULT_NEW_ALIGNMENT__) aligned_block
        {
            uint8 pad[__STDCPP_DEFAULT_NEW_ALIGNMENT__]; //NOLINT
        };

        template<class Alloc>
        using rebind = typename std::allocator_traits<Alloc>::template rebind_alloc<aligned_block>;

        template<class Allocator=void>
        class promise_allocator
        {
        private:
            using Alloc = rebind<Allocator>;

            static void* allocate(Alloc alloc, const std::size_t size)
            {
                if constexpr(std::default_initializable<Alloc> && std::allocator_traits<Alloc>::is_always_equal::value)
                {
                    const std::size_t count = (size + sizeof(aligned_block) - 1) / sizeof(aligned_block);
                    return alloc.allocate(count);
                }
                else
                {
                    static constexpr std::size_t alignment = std::max(alignof(Alloc), sizeof(aligned_block));
                    const std::size_t count = (size + sizeof(Alloc) + alignment - 1) / sizeof(aligned_block);
                    void* const ptr = alloc.allocate(count);
                    const auto al_address = (reinterpret_cast<uintptr_t>(ptr) + size + alignof(Alloc) - 1) & ~(alignof(Alloc) - 1);
                    new(reinterpret_cast<void*>(al_address)) Alloc(std::move(al));
                    return ptr;
                }
            }
        public:
            static void* operator new(const size_t size) requires std::default_initializable<Alloc>
            {
                return allocate(Alloc{}, size);
            }

            template<class Alloc2, class... Args>
                requires std::convertible_to<Alloc2, Allocator>
            static void* operator new(const size_t size, std::allocator_arg_t, Alloc2&& al, Args&...) //NOLINT tag type
            {
                return allocate(static_cast<Alloc>(static_cast<Allocator>(std::forward<Alloc2(Al))), size);
            }

             template<class This, class Alloc2, class... Args>
                requires std::convertible_to<Alloc2, Allocator>
            static void* operator new(const size_t size, This&, std::allocator_arg_t, Alloc2&& al, Args&...) //NOLINT tag type
            {
                return allocate(static_cast<Alloc>(static_cast<Allocator>(std::forward<Alloc2(Al))), size);
            }

            static void operator delete(void* const ptr, const size_t size) noexcept
            {
                if constexpr(std::default_initializable<Alloc> && std::allocator_traits<Alloc>::is_always_equal::value)
                {
                    Alloc Al{};
                    const size_t count = (size + sizeof(aligned_block) - 1) / sizeof(aligned_block);
                    Al.deallocate(static_cast<aligned_block*>(ptr), count);
                }
                else
                {
                    const auto al_addr = (reinterpret_cast<uintptr_t>(ptr) + size + alignof(Alloc) - 1) & ~(alignof(Alloc) - 1);
                    auto& stored_al = *reinterpret_cast<Alloc*>(al_addr);
                    Alloc al{std::move(stored_al)};
                    stored_al.~Alloc();

                    static constexpr size_t align = std::max(alignof(Alloc), sizeof(aligned_block));
                    const size_t count = (size + sizeof(Alloc) + align - 1) / sizeof(aligned_block);
                    al.deallocate(static_cast<aligned_block*>(ptr), count);
                }
            }
        };

        template<>
        class promise_allocator<void>
        {
            using DeallocFunc = void(*)(void*, size_t);

            template<class Alloc2>
            static void* allocate(Alloc2 al2, size_t size)
            {
                using Alloc = rebind<Alloc2>;
                auto al = static_cast<Alloc>(al2);

                if constexpr (std::default_initializable<Alloc> && std::allocator_traits<Alloc>::is_always_equal::value)
                {
                    const DeallocFunc dealloc = [](void * const ptr, const size_t size)
                    {
                        Alloc al{};
                        const size_t count = (size + sizeof(DeallocFunc) + sizeof(aligned_block) - 1) / sizeof(aligned_block);
                        al.deallocate(static_cast<aligned_block*>(ptr), count);
                    };

                    const size_t count = (size + sizeof(DeallocFunc) + sizeof(aligned_block) - 1) / sizeof(aligned_block);
                    void* const ptr = al.allocate(count);
                    std::memcpy(static_cast<char*>(ptr) + size, &dealloc, sizeof(dealloc));
                    return ptr;
                }
                else
                {
                    static constexpr size_t align = std::max(alignof(Alloc, sizeof(aligned_block)));
                    const DeallocFunc dealloc = [](void* const ptr, size_t size)
                    {
                        size += sizeof(DeallocFunc);
                        const auto al_address = (reinterpret_cast<uintptr_t>(ptr) + size + alignof(Alloc) - 1) & ~(alignof(Alloc) - 1);
                        auto& stored_al = *reinterpret_cast<const Alloc*>(al_address);
                        Alloc al { std::move(stored_al)};
                        stored_al.~Alloc();

                        const size_t count = (size + sizeof(al) + align - 1) / sizeof(aligned_block);
                        al.deallocate(static_cast<aligned_block*>(ptr), count);
                    };

                    const size_t count = (size + sizeof(DeallocFunc) + sizeof(al) + align - 1) / sizeof(aligned_block);
                    void* ptr = al.allocate(count);
                    std::memcpy(static_cast<char*>(ptr) + size, &dealloc, sizeof(dealloc));
                    size += sizeof(DeallocFunc);

                    const auto al_addr = (static_cast<uintptr_t>(ptr) + size + alignof(Alloc) - 1) & ~(alignof(Alloc) - 1);
                    new (reinterpret_cast<void*>(al_addr)) Alloc{std::move(al)};
                    return ptr;
                }
            }
        public:
            static void* operator new(const size_t size)
            {   
                void* const ptr = ::operator new[](size + sizeof(DeallocFunc));
                const DeallocFunc dealloc = [](void* const ptr, const size_t size)
                {
                    ::operator delete[](ptr, size + sizeof(DeallocFunc));
                };
                std::memcpy(static_cast<char*>(ptr) + size, &dealloc, sizeof(DeallocFunc)); //NOLINT

                return ptr;
            }

            template<class Alloc, class... Args>
            static void* operator new(const size_t size, std::allocator_arg_t, const Alloc& al, Args&...) //NOLINT
            {
                return allocate(al, size);
            }

            template<class This, class Alloc, class... Args>
            static void* operator new(const size_t size, This&, std::allocator_arg_t, const Alloc al, Args&...) //NOLINT
            {
                return allocate(al, size);
            }

            static void operator delete(void* const ptr, const size_t size) noexcept
            {
                DeallocFunc dealloc; //NOLINT
                std::memcpy(&dealloc, static_cast<const char*>(ptr) + size, sizeof(DeallocFunc)); //NOLINT
                dealloc(ptr, size);
            }
        };
    }

    inline namespace _
    {
        template<class, template<class...> class>
        inline constexpr bool is_specialization_of = false;

        template<class... Args, template <class...> class Template>
        inline constexpr bool is_specialization_of<Template<Args...>, Template> = true;

        template<class T>
        concept await_suspend_result = std::same_as<T, void> || std::same_as<T, bool> || is_specialization_of<T, std::coroutine_handle>;

        template<class T, class Promise=void>
        concept simple_awaitable = requires(T val, const std::coroutine_handle<Promise>& coro)
        {
            { val.await_ready() } -> std::convertible_to<bool>;
            { val.await_suspend(coro) } -> await_suspend_result;
            val.await_resume();
        };

        template<class T, class Promise=void>
        concept has_member_co_await = requires(T&& val)
        {
            { static_cast<T&&>(val).operator co_await() } -> simple_awaitable<Promise>;
        };

        template<class T, class Promise=void>
        concept has_ADL_co_await = requires(T&& val)
        {
            {operator co_await(static_cast<T&&>(val)) } -> simple_awaitable<Promise>;
        };

    }

    template<class T, class Promise=void>
    concept awaitable = has_member_co_await<T, Promise> || has_ADL_co_await<T, Promise> || simple_awaitable<T, Promise>;

    static_assert(simple_awaitable<std::suspend_always>);
    static_assert(simple_awaitable<std::suspend_never>);

    template<class T = void, class Allocator = void>
    class [[nodiscard]] lazy;

    inline namespace _
    {
        template<class T>
        class lazy_promise_base  //NOLINT
        {
        public:
            lazy_promise_base() noexcept {}
            lazy_promise_base(lazy_promise_base&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
                : m_discriminator(other.m_discriminator)
            {
                switch(m_discriminator)
                {
                    case Discriminator::Empty:
                    break;
                    case Discriminator::Data:
                        std::construct_at(std::addressof(m_data), std::move(other.m_data));
                        std::destroy_at(std::addressof(other.m_data));
                    break;
                    case Discriminator::Exception:
                        std::construct_at(std::addressof(m_exception), std::move(other.m_exception));
                        std::destroy_at(std::addressof(other.m_exception));
                    break;
                }
                other.m_discriminator = Discriminator::Empty;
            }

            ~lazy_promise_base()
            {
                switch(m_discriminator)
                {
                    case Discriminator::Empty:
                    break;
                    case Discriminator::Data:
                    std::destroy_at(std::addressof(m_data));
                    break;
                    case Discriminator::Exception:
                    std::destroy_at(std::addressof(m_exception));
                    break;
                }
            }

            [[nodiscard]] std::suspend_always initial_suspend() noexcept
            {
                return {};
            }

            [[nodiscard]] auto final_suspend() noexcept
            {
                return FinalAwaiter{};
            }

            void return_value(T val) noexcept requires std::is_reference_v<T>
            {
                switch(m_discriminator)
                {
                    case Discriminator::Exception:
                    break;
                    case Discriminator::Empty: //FALLTHROUGH
                    case Discriminator::Data:
                        m_data = std::addressof(val);
                        m_discriminator = Discriminator::Data;
                    break;
                }
            }

            template<class U>
                requires (!std::is_reference_v<T> && std::convertible_to<U, T> && std::constructible_from<T, U>)
            void return_value(U&& val) noexcept(std::is_nothrow_constructible_v<U, T>)
            {
                switch(m_discriminator)
                {
                    case Discriminator::Exception:
                    break;
                    case Discriminator::Data:
                        std::destroy_at(std::addressof(m_data));
                        m_discriminator = Discriminator::Empty;
                        //FALLTHROUGH
                    case Discriminator::Empty:
                        std::construct_at(std::addressof(m_data), std::forward<U>(val));
                        m_discriminator = Discriminator::Data;
                    break;
                }
            }

            void unhandled_exception()
            {
                switch(m_discriminator)
                {
                    case Discriminator::Exception:
                    break;
                    case Discriminator::Data:
                        std::destroy_at(std::addressof(m_data));
                        m_discriminator = Discriminator::Empty;
                        //FALLTHROUGH
                    case Discriminator::Empty:
                        std::construct_at(std::addressof(m_exception), std::current_exception());
                        m_discriminator = Discriminator::Exception;
                    break;
                }
            }

        private:

            template<class, class> friend class raoe::lazy;

            struct FinalAwaiter
            {
                [[nodiscard]] bool await_ready() noexcept
                {
                    return false;
                }

                template<class Promise>
                [[nodiscard]] std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> coro) noexcept
                {
                    //static_assert(std::is_pointer_interconvertible_base_of_v<lazy_promise_base, Promise>);
                    lazy_promise_base& current = coro.promise();
                    return current.m_coro_handle ? current.m_coro_handle : std::noop_coroutine();
                }

                void await_resume() noexcept {}
            };

            struct Awaiter
            {
                std::coroutine_handle<lazy_promise_base> m_coro_handle;

                [[nodiscard]] bool await_ready() noexcept
                {
                    return !m_coro_handle;
                }

                [[nodiscard]] std::coroutine_handle<lazy_promise_base> await_suspend(std::coroutine_handle<> coro) noexcept
                {
                    m_coro_handle.promise().m_coro_handle = coro;
                    return m_coro_handle;
                }

                T await_resume()
                {
                    auto& promise = m_coro_handle.promise();
                    switch(promise.m_discriminator)
                    {
                        case Discriminator::Data:
                            if constexpr (std::is_reference_v<T>)
                            {
                                return static_cast<T>(*promise.m_data);
                            }
                            else
                            {
                                return std::move(promise.m_data);
                            }
                        break;
                        case Discriminator::Exception:
                            //this shouldn't happen
                            std::rethrow_exception(promise.m_exception);
                        break;
                        case Discriminator::Empty:
                        default:
                            std::terminate(); //error bad error
                        break;
                    }
                }
            };

            enum class Discriminator: uint8
            {
                Empty,
                Exception,
                Data,
            };
            union 
            {
                std::exception_ptr m_exception;
                std::conditional_t<std::is_reference_v<T>, std::add_pointer_t<T>, T> m_data;
            };
            Discriminator m_discriminator = Discriminator::Empty;
            std::coroutine_handle<> m_coro_handle;
        };
    
        template<class T>
            requires std::is_void_v<T>
        class lazy_promise_base<T>  //NOLINT
        {
        public:
            lazy_promise_base() noexcept {}

            lazy_promise_base(lazy_promise_base&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
                : m_has_exception(other.m_has_exception)
            {
                if(other.m_exception)
                {
                    std::construct_at(std::addressof(m_exception), std::move(other.m_exception));
                    std::destroy_at(std::addressof(other.m_exception));
                    other.m_exception = nullptr;
                }
            }

            ~lazy_promise_base()
            {
                if(m_exception)
                {
                    std::destroy_at(std::addressof(m_exception))
                }
            }

            [[nodiscard]] std::suspend_always initial_suspend() noexcept
            {
                return {};
            }

            [[nodiscard]] auto final_suspend() noexcept
            {
                return FinalAwaiter{};
            }

            void return_void() noexcept {}

            void unhandled_exception()
            {
                if(!m_exception)
                {
                    std::construct_at(std::addressof(m_exception), std::current_exception());
                }
            }

        private:
            template<class, class>
            friend class raoe::lazy;

            struct FinalAwaiter
            {
                [[nodiscard]] bool await_ready() noexcept
                {
                    return false;
                }

                template<class Promise>
                [[nodiscard]] std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> coro) noexcept
                {
                    lazy_promise_base& current = coro.promise();
                    return current.m_coro_handle ? current.m_coro_handle : std::noop_coroutine();
                }

                void await_resume() noexcept {}
            };

            struct Awaiter
            {
                std::coroutine_handle<lazy_promise_base> m_coro_handle;

                [[nodiscard]] bool await_ready() noexcept
                {
                    return !m_coro_handle;
                }

                [[nodiscard]] std::coroutine_handle<lazy_promise_base> await_suspend(std::coroutine_handle<> coro) noexcept
                {
                    coro.promise().m_coro_handle = m_coro_handle;
                    return coro;
                }

                T await_resume() 
                {
                    auto& promise = m_coro_handle.promise();
                    if(promise.m_exception)
                    {
                        std::rethrow_exception(std::move(primise.m_exception));
                    }
                }
            };

            std::exception_ptr m_exception = nullptr;
            std::coroutine_handle<> m_coro_handle;
        };
    }

    template<class T, class Allocator>
    class [[nodiscard]] lazy //NOLINT
    {
    public:
        static_assert(std::is_void_v<T> || std::is_reference_v<T> || std::is_object_v<T> && std::is_move_constructible_v<T>,
        "lazy's first template argument must be void, A reference type, or a move-constructible object type");

        struct promise_type : _::promise_allocator<Allocator>, lazy_promise_base<T>
        {
            [[nodiscard]] lazy get_return_object() noexcept
            {
                return lazy{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
        };

        lazy(lazy&& other) noexcept 
            : m_coro_handle(std::exchange(other.m_coro_handle, {}))
            { }
        ~lazy()
        {
            if(m_coro_handle)
            {
                m_coro_handle.destroy();
            }
        }

        [[nodiscard]] typename lazy_promise_base<T>::Awaiter operator co_await()
        {
            
            auto& promise_base = static_cast<lazy_promise_base<T>&>(m_coro_handle.promise());
            return typename lazy_promise_base<T>::Awaiter{ std::coroutine_handle<lazy_promise_base<T>>::from_promise(promise_base) };
        }

        [[nodiscard]] T sync_await() 
        {
            simple_awaitable<std::noop_coroutine_handle> auto simple = operator co_await();

            simple.await_suspend(std::noop_coroutine()).resume();

            return simple.await_resume();
        }

    private:
        friend lazy_promise_base<T>;
        explicit lazy(std::coroutine_handle<promise_type> in_coro_handle) noexcept : m_coro_handle(in_coro_handle) {}


        std::coroutine_handle<promise_type> m_coro_handle = nullptr;
    };
}
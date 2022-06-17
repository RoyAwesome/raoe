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


#include <functional>
#include "core.hpp"
#include "services/iservice.hpp"
#include "container/subclass_map.hpp"
#include "engine_fwd.hpp"

int main(int, char**);

namespace RAOE
{
    class Engine;



    class Engine
    {     
    public:     
       Engine(const Engine&) = delete;
        Engine(Engine&&) = delete;

        ~Engine();
    protected:
        Engine(int argv, char** argc);
        struct FromTest {};
        Engine(FromTest) : Engine() { }

        //Main calls Init and Run (but nobody else can).  So it's friend.  
        friend int ::main(int, char**);
        friend class CogManager;

        void Startup();
        bool Run();
        void Shutdown();

    public:

        template<is_service T>
        std::weak_ptr<T> get_service()
        {
            return services.find<T>();
        }

        template<is_service T>
        std::weak_ptr<T> init_service()
        {
            return services.insert<T>(*this);
        }

        template<is_service T>
        void destroy_service()
        {
            services.erase<T>();
        }
    private:
        Engine(); //Base Ctor, must be called by one of the protected ctors.  
        
        raoe::container::subclass_map<RAOE::Service::IService> services;
    };


}
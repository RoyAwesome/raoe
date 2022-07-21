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
#include "lazy.hpp"

namespace RAOE
{
    void enqueue_task(RAOE::Engine& engine, raoe::lazy<>&& task);
}

namespace RAOE::Service
{
    class TaskService;

    class task_provider
    {
    public:
        friend class RAOE::Service::TaskService;

        void startup_task(raoe::lazy<>&& task) { m_startup_tasks.emplace_back(std::move(task)); }
        void shutdown_task(raoe::lazy<>&& task) { m_shutdown_tasks.emplace_back(std::move(task)); }
    private:
        std::list<raoe::lazy<>> m_startup_tasks;
        std::list<raoe::lazy<>> m_shutdown_tasks;    
    };

    class TaskService : public IService
    {
    public:
        TaskService(Engine& in_engine)
            : IService(in_engine)
        {            
        }

        void process_tasks();
        void add_task(raoe::lazy<>&& task);
        
        void enqueue_startup_tasks(task_provider& provider)
        {
            append_tasks(provider.m_startup_tasks);
        }

        void enqueue_shutdown_tasks(task_provider& provider)
        {
            append_tasks(provider.m_shutdown_tasks);
        }    

        void append_tasks(std::list<raoe::lazy<>>& in_list);
    private:
        std::list<raoe::lazy<>> m_task_list;
    };
}
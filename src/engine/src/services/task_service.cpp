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


#include "services/task_service.hpp"
#include "engine.hpp"

namespace RAOE::Service
{
    void TaskService::process_tasks()
    {
        for(const raoe::lazy<>& task : m_task_list)
        {
            if(!task.done())
            {
                task.resume();
            }
        }

        //Remove all done tasks
        m_task_list.remove_if([](const raoe::lazy<>& task){ return task.done(); });
    }

    void TaskService::add_task(raoe::lazy<>&& task)    
    {   
        m_task_list.emplace_back(std::move(task));
    }

    void TaskService::append_tasks(std::list<raoe::lazy<>>& in_list)    
    {
        for(raoe::lazy<>& task : in_list)
        {
            add_task(std::move(task));
        }       
        in_list.clear();    
    }

}

void RAOE::enqueue_task(RAOE::Engine& engine, raoe::lazy<>&& task)
{
    if(auto task_service = engine.get_service<RAOE::Service::TaskService>().lock())
    {
        task_service->add_task(std::move(task));
    }
}

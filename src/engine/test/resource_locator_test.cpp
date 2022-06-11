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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "resource/resource_locator.hpp"
#include "engine.hpp"

namespace ResourceLocatorTest
{
    class Engine : public RAOE::Engine
    {
    public:
        Engine() : RAOE::Engine(RAOE::Engine::FromTest {}) 
        {
            init_service<RAOE::Resource::Service>();
        }
    };
}

TEST(Locator, GenericTest)
{
    using Engine = ResourceLocatorTest::Engine;
    Engine e;

    if(auto resource_service = e.get_service<RAOE::Resource::Service>().lock())
    {        
        RAOE::Resource::DefaultLocator default_locator;
        std::filesystem::path p = default_locator(*(resource_service.get()), RAOE::Resource::Tag("raoe:testfile"));
        spdlog::info("{}", p.string());
    }
    
}

TEST(Locator, ResourceResolverTest)
{
    spdlog::info(std::filesystem::current_path().string());
    using Engine = ResourceLocatorTest::Engine;
    Engine e;

    if(auto resource_service = e.get_service<RAOE::Resource::Service>().lock())
    {
        std::vector<RAOE::Resource::ResolvedResource> resolved_resources;

        RAOE::Resource::ResourceResolver resolver {};
        std::shared_ptr resource_handle = resource_service->get_resource(RAOE::Resource::Tag("raoe:test/testasset"));
        resolver(resource_handle, std::back_inserter(resolved_resources));
        spdlog::info("Num Resources {}", resolved_resources.size());
        for(auto& res_resource : resolved_resources)
        {
            spdlog::info(res_resource.resolved_path.string());
        }
    }
}
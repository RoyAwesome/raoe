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

#include "resource/locator.hpp"
#include "resource/handle.hpp"
#include "resource/tag.hpp"
#include "resource/type.hpp"
#include "engine.hpp"

#include "resource/assets/text_asset.hpp"
#include <fstream>
#include <string_view>

using namespace std::literals::string_view_literals;

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
        std::filesystem::path p = default_locator(*(resource_service), RAOE::Resource::Tag("raoe:testfile"));
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

        EXPECT_EQ(resolved_resources.size(), 1);

        for(auto& res_resource : resolved_resources)
        {
            spdlog::info(res_resource.resolved_path.string());
        }
    }
}

TEST(Loader, ResourceResolverTest)
{
    std::filesystem::path resolved_path;

    using Engine = ResourceLocatorTest::Engine;
    Engine e;
    if(auto resource_service = e.get_service<RAOE::Resource::Service>().lock())
    {
        std::vector<RAOE::Resource::ResolvedResource> resolved_resources;

        RAOE::Resource::ResourceResolver resolver {};
        std::shared_ptr resource_handle = resource_service->get_resource(RAOE::Resource::Tag("raoe:test/testasset"));
        resolver(resource_handle, std::back_inserter(resolved_resources));

        if(resolved_resources.size() == 1)
        {
            resolved_path = resolved_resources[0].resolved_path;
        }       
    }

    RAOE::Resource::Asset::TextAssetLoader loader;

    std::ifstream file(resolved_path.string());

    std::shared_ptr<RAOE::Resource::Asset::TextAsset> resource = loader.load_resource<RAOE::Resource::Asset::TextAsset>(file);

    file.close();

    spdlog::info(resource->contents());

    EXPECT_EQ(resource->contents(), "hello world"sv);
}

TEST(Loader, TypeHasLoaders)
{
    using Engine = ResourceLocatorTest::Engine;
    Engine e;
    if(auto resource_service = e.get_service<RAOE::Resource::Service>().lock())
    {
        std::shared_ptr<RAOE::Resource::Handle> resource_handle = resource_service->get_resource(RAOE::Resource::Asset::Tags::TextAsset); 
        EXPECT_TRUE(resource_handle);
        const auto& type = resource_handle->get_ref<RAOE::Resource::Type>();
        EXPECT_EQ(type.tag(), RAOE::Resource::Asset::Tags::TextAsset);
        EXPECT_EQ(type.loaders().size(), 1);
        EXPECT_EQ(type.loaders().at(0).lock()->tag(), RAOE::Resource::Asset::Tags::TextLoader);
    }
}
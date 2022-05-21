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

#include "resource/tag.hpp"

using namespace std::literals::string_view_literals;

TEST(Tag, Construction)
{
    RAOE::Resource::Tag tag("minecraft:dirt");
    EXPECT_EQ(tag.prefix(), "minecraft"sv);
    EXPECT_EQ(tag.identifier(), "dirt"sv );
    EXPECT_EQ(tag, "minecraft:dirt"sv); 
}

TEST(Tag, InvalidIdentifier)
{
    RAOE::Resource::Tag tag("minecraft:");
    EXPECT_EQ(tag.identifier(), ""sv);
    EXPECT_EQ(tag, false);
}

TEST(Tag, InvalidPrefix)
{
    RAOE::Resource::Tag tag(":test");
    EXPECT_EQ(tag.prefix(), ""sv);
    EXPECT_EQ(tag, false); 
}

TEST(Tag, DefaultNamespace)
{
    RAOE::Resource::Tag tag("dirt");
    EXPECT_EQ(tag.prefix(), RAOE::Resource::Tag::DefaultPrefix);
    EXPECT_EQ(tag.identifier(), "dirt"sv );
    EXPECT_EQ(tag, std::string_view(std::string(RAOE::Resource::Tag::DefaultPrefix) + ":dirt")); 
}

TEST(Tag, InvalidCharInNamespace)
{
    RAOE::Resource::Tag tag("voidcra/ft:dirt");
    EXPECT_EQ(tag, false);
}

TEST(Tag, PathDirectory)
{
    RAOE::Resource::Tag tag("minecraft:block/dirt");
    EXPECT_EQ(tag.prefix(), "minecraft"sv);
    EXPECT_EQ(tag.identifier(), "block/dirt"sv );
    EXPECT_EQ(tag, "minecraft:block/dirt"sv); 
}

TEST(Tag, InvalidCharInPath)
{
    RAOE::Resource::Tag tag("voidcraft:di()rt");
    EXPECT_EQ(tag, false);
}

TEST(Tag, Equality)
{
    RAOE::Resource::Tag TagA("voidcraft:dirt");
    RAOE::Resource::Tag TagB("voidcraft:dirt");

    EXPECT_EQ(TagA == TagB, true);
}

TEST(Tag, JustColon)
{
    RAOE::Resource::Tag tag(":");
    EXPECT_EQ(tag.identifier(), ""sv);
    EXPECT_EQ(tag.prefix(), ""sv);
    EXPECT_EQ(tag, false);
}
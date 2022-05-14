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

#include "container/subclass_map.hpp"

using namespace std::literals::string_view_literals;

struct animal
{
    virtual std::string_view speak() = 0;
};

struct cat : public animal
{
    virtual std::string_view speak() override { return "meow"sv; }
};

struct dog : public animal
{
    virtual std::string_view speak() override { return "woof"sv; }
};

struct cow : public animal
{
    virtual std::string_view speak() override { return "moo"sv; }
};

struct human : public animal
{
    virtual std::string_view speak() override { return "i'm so tired"sv; }
};

struct rex : public animal
{   
    virtual std::string_view speak() override { return "rawr"sv; }
};


TEST(SubclassMapTest, BasicUsage)
{
    raoe::container::subclass_map<animal> animal_map;

    animal_map.insert<cat>();
    animal_map.insert<dog>();
    animal_map.insert<cow>();
    animal_map.insert<human>();

    EXPECT_EQ(animal_map.find<cat>()->speak(), "meow"sv);
    EXPECT_EQ(animal_map.find<rex>(), nullptr);
    EXPECT_EQ(animal_map.size(), 4); //ensure that trying to find a type that is not in the map didn't increase the size of the map
}

TEST(SubclassMapTest, Count)
{
    raoe::container::subclass_map<animal> animal_map;

    animal_map.insert<cat>();
    animal_map.insert<dog>();
    animal_map.insert<cow>();
    animal_map.insert<human>();

    
    EXPECT_EQ(animal_map.size(), 4);
}

TEST(SubclassMapTest, Contains)
{
    raoe::container::subclass_map<animal> animal_map;

    animal_map.insert<cat>();
    animal_map.insert<dog>();
    animal_map.insert<cow>();
    animal_map.insert<human>();

    EXPECT_EQ(animal_map.contains<human>(), true);
}
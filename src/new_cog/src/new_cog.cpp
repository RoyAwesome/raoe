
#include "cogs/cog.hpp"

namespace TestGear
{
    struct Test : public RAOE::Cogs::Gear
    {

    };
}

RAOE_DEFINE_GEAR(NewGearTest, TestGear::Test)

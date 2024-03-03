#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "microECS.h"

TEST_CASE("Entity Creation", "[world]")
{
    microECS::World world;

    SECTION("Basic Entity Creation")
    {
        auto entity1 = world.Entity();
        auto entity2 = world.Entity();

        REQUIRE(entity1.IsValid());
        REQUIRE(entity2.IsValid());
        REQUIRE(entity2.GetID() != entity1.GetID()); // Entities should have different IDs
    }

    SECTION("Entity Creation with Name")
    {
        std::string name = "TestEntity";
        auto entity = world.Entity(name);

        REQUIRE(entity.IsValid());
        REQUIRE(entity.GetName() == name);
    }
}

TEST_CASE("Entity Lookup", "[world]")
{
    microECS::World world;

    SECTION("Lookup Entity by Name")
    {
        std::string name = "TestEntity";
        auto entity = world.Entity(name);

        auto lookupEntity = world.Lookup(name);

        REQUIRE(entity.GetID() == lookupEntity.GetID());
    }
}
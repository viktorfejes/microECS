#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "microECS.h"

TEST_CASE("Entity Creation", "[world]") {
    microECS::World world;

    SECTION("Basic Entity Creation") {
        auto entity1 = world.Entity();
        auto entity2 = world.Entity();

        REQUIRE(entity1.IsValid());
        REQUIRE(entity2.IsValid());
        REQUIRE(entity2.GetID() != entity1.GetID()); // Entities should have different IDs
    }

    SECTION("Entity Creation with Name") {
        std::string name = "TestEntity";
        auto entity = world.Entity(name);

        REQUIRE(entity.IsValid());
        REQUIRE(entity.GetName() == name);
    }
}

TEST_CASE("Entity Lookup", "[world]") {
    microECS::World world;

    SECTION("Lookup Entity by Name") {
        std::string name = "TestEntity";
        auto entity = world.Entity(name);

        auto lookupEntity = world.Lookup(name);

        REQUIRE(entity.GetID() == lookupEntity.GetID());
    }
}

TEST_CASE("Singleton Components", "[world]") {
    microECS::World world;

    SECTION("Add Singleton Component") {
        struct Gravity {
            float value = 9.81f;
        };

        float gravityValue = 101.02f;
        Gravity& gravitySingleton = world.Set<Gravity>({ gravityValue });
        REQUIRE(gravitySingleton.value == gravityValue);
    }

    SECTION("Get Singleton Component") {
        struct Gravity {
            float value = 9.81f;
        };

        struct Time {
            float value = 0.0f;
            float deltaTime = 0.0f;
        };

        world.Set<Gravity>({ 101.02f });
        world.Set<Time>({ 10.0f, 0.1f });

        const Gravity* gravity = world.Get<Gravity>();
        const Time* time = world.Get<Time>();

        REQUIRE(gravity->value == 101.02f);
        REQUIRE(time->value == 10.0f);
        REQUIRE(time->deltaTime == 0.1f);
    }
}
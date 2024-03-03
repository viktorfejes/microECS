#include "catch2/catch.hpp"
#include "microECS.h"

TEST_CASE("Add component to entity", "[entity]")
{
    struct TestComponent
    {
        int value;
    };

    struct TestComponent_2
    {
        float value;
    };

    microECS::World world;

    SECTION("Using Add")
    {
        auto entity = world.Entity();
        entity.Add<TestComponent>();
        REQUIRE(entity.Has<TestComponent>());
    }

    SECTION("Using Set")
    {
        auto entity = world.Entity();
        entity.Set<TestComponent>({42});
        REQUIRE(entity.Has<TestComponent>());
        REQUIRE(entity.Get<TestComponent>()->value == 42);
    }

    SECTION("Using Add and Set")
    {
        auto entity = world.Entity();
        entity.Add<TestComponent>().Set<TestComponent>({42});
        REQUIRE(entity.Has<TestComponent>());
        REQUIRE(entity.Get<TestComponent>()->value == 42);
    }

    SECTION("Add multiple components")
    {
        auto entity = world.Entity();
        entity.Add<TestComponent>().Add<TestComponent_2>();
        REQUIRE(entity.Has<TestComponent, TestComponent_2>());
    }

    SECTION("Set multiple components")
    {
        auto entity = world.Entity();

        entity.Add<TestComponent>()
            .Add<TestComponent_2>()
            .Set<TestComponent>({42})
            .Set<TestComponent_2>({3.14f});

        REQUIRE(entity.Has<TestComponent, TestComponent_2>());
        REQUIRE(entity.Get<TestComponent>()->value == 42);
        REQUIRE(entity.Get<TestComponent_2>()->value == 3.14f);
    }

    SECTION("Remove component")
    {
        auto entity = world.Entity();
        entity.Add<TestComponent>().Remove<TestComponent>();
        REQUIRE_FALSE(entity.Has<TestComponent>());
    }
}
![microECS header](docs/images/microecs_header_image.jpg)

`microECS` is a header-only, small & compact library for simple ECS needs. Primarily made for game engines but can be used for simulations or other applications as well. Here are some of the highlights of the framework:

- 100% open-source under the [MIT License](LICENSE.md).
- Header-only, so it is easy to incorporate into your project.
- Uses sparse set architecture for performant loops, inserts, and removals.
- Automatic component registration
- Small size, and zero dependencies.

# Quickstart

```cpp
#include "microECS/microECS.h"

struct Position
{
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity
{
    float dx = 0.0f;
    float dy = 0.0f;
};


int main() {
    // Start by creating a new microECS::World,
    // this governs the whole system.
    microECS::World world; // If needed, it's possible to have multiple Worlds.

    // Add Entities to the world, with or without names.
    microECS::Entity entity1 = world.Entity();
    microECS::Entity entity2 = world.Entity("EntityName1");
    microECS::Entity entity3 = world.Entity("EntityName2");

    // You can easily find named Entities
    microECS::Entity namedEntity = world.Lookup("EntityName1");

    // When Entities get destroyed, their IDs get reused.
    entity1.Destroy();

    // Adding and removing Components
    entity2.Add<Position>(); // Add with default values
    entity2.Set<Position>({ 2.5f, 3.14f }); // Sets value of component, or if not found, adds it

    // Methods can be chained
    entity3.Set<Position>({ -1.0f, 8.2f })
            .Add<Velocity>();

    // Component removal
    entity3.Remove<Velocity>();

    // Check for components
    entity2.Has<Position>(); // returns true
    entity2.Has<Position, Velocity>(); // returns false

    // Components can also be sorted.
    // This uses a memory efficient in-place sorting of the component pool
    world.Sort<Position>([](const Position& a, const Position& b) {
        return a.x < b.x;
    });

    // Views
    microECS::View view = World.View<Position>();
    view.Each([](microECS::EntityID entityID, Position& position) {
        position.x += 1.0f;
        position.y += 1.0f;
    });
}

```

# Warning

`microECS` is a new framework that is under development. Consider it work-in-progress and only use it in production if you are aware of what you are getting yourself into.
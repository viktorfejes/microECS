#pragma once

#include <cstdint>
#include <limits>

namespace ECS
{
    using EntityID = uint32_t;
    using ComponentID = uint8_t;

    constexpr size_t INIT_COMPONENT_POOL_SIZE = 32;
    constexpr uint8_t INVALID_COMPONENT_ID = std::numeric_limits<uint8_t>::max();
    constexpr uint32_t INVALID_ENTITY_ID = std::numeric_limits<uint32_t>::max();
    constexpr size_t INVALID_COLUMN_INDEX = std::numeric_limits<size_t>::max();
    constexpr size_t MAX_COMPONENT_TYPES = std::numeric_limits<uint8_t>::max() - 1;
}
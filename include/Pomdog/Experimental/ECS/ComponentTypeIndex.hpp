// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace Gameplay {

class ComponentTypeIndex {
public:
    typedef std::uint8_t IndexType;

    template <class TComponent>
    static IndexType Index()
    {
        static_assert(std::is_unsigned<IndexType>::value, "IndexType is unsigned integer.");
        static_assert(!std::is_pointer<TComponent>::value, "TComponent is not pointer.");
        static_assert(std::is_object<TComponent>::value, "TComponent should be object type.");
        static const auto value = IncrementIndex();
        return value;
    }

private:
    static IndexType IncrementIndex();

private:
    static IndexType count;
};

} // namespace Gameplay
} // namespace Detail
} // namespace Pomdog

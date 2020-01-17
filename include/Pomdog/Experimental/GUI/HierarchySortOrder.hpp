// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog::GUI {

enum class HierarchySortOrder : std::uint8_t {
    Front = 0,
    Sortable = 1,
    Back = 2,
};

} // namespace Pomdog::GUI

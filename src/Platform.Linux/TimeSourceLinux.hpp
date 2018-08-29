// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Linux {

class TimeSourceLinux final {
public:
    TimeSourceLinux() = default;

    TimePoint Now() const;
};

} // namespace Linux
} // namespace Detail
} // namespace Pomdog

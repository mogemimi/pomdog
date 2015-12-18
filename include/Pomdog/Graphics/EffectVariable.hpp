// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "EffectAnnotation.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {

struct EffectVariable {
    std::string Name;
    EffectAnnotation Annotation;
    std::uint32_t StartOffset;
};

} // namespace Pomdog

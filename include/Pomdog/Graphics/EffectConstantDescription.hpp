// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_HPP
#define POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_HPP

#include "EffectVariable.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog {

struct EffectConstantDescription {
    std::vector<EffectVariable> Variables;
    std::string Name;
    std::uint32_t ByteSize;
};

} // namespace Pomdog

#endif // POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_HPP

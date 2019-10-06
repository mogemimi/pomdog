// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>
#include <tuple>

namespace Pomdog {
class ParticleClip;
} // namespace Pomdog

namespace Pomdog::Detail {

struct ParticleLoader final {
    [[nodiscard]] static std::tuple<ParticleClip, std::shared_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace Pomdog::Detail

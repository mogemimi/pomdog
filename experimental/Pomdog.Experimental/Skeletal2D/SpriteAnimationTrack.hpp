// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTimeInterval.hpp"
#include "Skin.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <optional>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

struct SpriteKeyframe {
    Rectangle Subrect;
    Vector2 Origin;
    AnimationTimeInterval Time;
    std::int16_t TexturePage;
    bool TextureRotate;
};

class SpriteAnimationTrack final {
public:
    SpriteAnimationTrack() = default;
    SpriteAnimationTrack(std::vector<SpriteKeyframe> && keys, std::uint32_t slotHashId);

    void Apply(Skin & skin, AnimationTimeInterval const& time);

    AnimationTimeInterval Length() const;

private:
    std::vector<SpriteKeyframe> keys;
    std::uint32_t slotHashId;
    std::optional<std::uint16_t> slotIndex;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog

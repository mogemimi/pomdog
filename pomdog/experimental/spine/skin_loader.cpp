// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skin_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/spine/skin_data.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skin_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::JointIndex;
using skeletal2d::RigidSlot;

// NOTE: A mesh index of 65535 means that the slot shows a region attachment.
constexpr u16 noMesh = 65535;

[[nodiscard]] std::tuple<std::vector<RigidSlot>, std::unique_ptr<Error>>
CreateSlots(
    const pomdogschemas::Skeletal2DSkin& skin,
    const TextureAtlas& textureAtlas)
{
    std::vector<RigidSlot> slots;

    const auto* skinSlots = skin.slots();
    if (skinSlots == nullptr) {
        return std::make_tuple(std::move(slots), nullptr);
    }
    slots.reserve(skinSlots->size());

    i16 drawOrder = 0;

    for (const auto* skinSlot : *skinSlots) {
        if (skinSlot->mesh() != noMesh) {
            // NOTE: A sprite cannot show a mesh.
            continue;
        }

        // NOTE: The key of an attachment is also the key of its region in the texture atlas.
        auto optIdx = textureAtlas.findRegionByKey(skinSlot->attachment_key());
        if (!optIdx.has_value()) {
            auto err = errors::make(pomdog::format("cannot find attachment {:#010x} in the texture atlas", skinSlot->attachment_key()));
            return std::make_tuple(std::vector<RigidSlot>{}, std::move(err));
        }

        auto atlasRegion = textureAtlas.getRegion(*optIdx);

        RigidSlot slot;

        slot.JointIndex = JointIndex{skinSlot->joint()};
        slot.HashID = skinSlot->slot_key();

        slot.Color = Color::createWhite();
        slot.DrawOrder = drawOrder;
        slot.Scale = Vector2{skinSlot->scale_x(), skinSlot->scale_y()};
        slot.Translate = Vector2{skinSlot->translate_x(), skinSlot->translate_y()};
        slot.Rotation = skinSlot->rotation();

        slot.TexturePage = 0;
        slot.TextureRotate = false;
        slot.Subrect = Rect2D{
            atlasRegion.subrectX,
            atlasRegion.subrectY,
            atlasRegion.subrectWidth,
            atlasRegion.subrectHeight,
        };
        auto textureXOffset = atlasRegion.xOffset;
        auto textureYOffset = atlasRegion.yOffset;
        slot.Origin.x = static_cast<f32>(atlasRegion.width / 2 - textureXOffset) / atlasRegion.subrectWidth;
        slot.Origin.y = static_cast<f32>(atlasRegion.height / 2 - textureYOffset) / atlasRegion.subrectHeight;

        if (slot.TextureRotate) {
            std::swap(slot.Subrect.width, slot.Subrect.height);
            std::swap(slot.Origin.x, slot.Origin.y);
        }

        slots.push_back(std::move(slot));
        ++drawOrder;
    }

    return std::make_tuple(std::move(slots), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<skeletal2d::Skin>, std::unique_ptr<Error>>
CreateSkin(
    const SkinData& skinData,
    const TextureAtlas& textureAtlas,
    std::string_view skinName)
{
    const auto* skin = skinData.findSkin(computeStringHash32(skinName));
    if (skin == nullptr) {
        auto err = errors::make(pomdog::format("cannot find skin '{}'", skinName));
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [slots, slotsErr] = CreateSlots(*skin, textureAtlas);
    if (slotsErr != nullptr) {
        auto wrapped = errors::wrap(std::move(slotsErr), pomdog::format("failed to create skin '{}'", skinName));
        return std::make_tuple(nullptr, std::move(wrapped));
    }

    auto result = std::make_shared<skeletal2d::Skin>(std::move(slots));
    return std::make_tuple(std::move(result), nullptr);
}

} // namespace pomdog::spine

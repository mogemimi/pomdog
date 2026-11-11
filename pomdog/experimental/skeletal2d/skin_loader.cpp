// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skin_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/skeletal2d/skin_data.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skin_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

// NOTE: A mesh index of 65535 means that the slot shows a region attachment.
constexpr u16 noMesh = 65535;

[[nodiscard]] std::tuple<std::vector<RigidSlot>, std::unique_ptr<Error>>
createSlots(
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

        slot.jointIndex = JointIndex{skinSlot->joint()};
        slot.hashID = skinSlot->slot_key();

        slot.color = Color::createWhite();
        slot.drawOrder = drawOrder;
        slot.scale = Vector2{skinSlot->scale_x(), skinSlot->scale_y()};
        slot.translate = Vector2{skinSlot->translate_x(), skinSlot->translate_y()};
        slot.rotation = skinSlot->rotation();

        slot.texturePage = 0;
        slot.textureRotate = false;
        slot.subrect = Rect2D{
            atlasRegion.subrectX,
            atlasRegion.subrectY,
            atlasRegion.subrectWidth,
            atlasRegion.subrectHeight,
        };
        auto textureXOffset = atlasRegion.xOffset;
        auto textureYOffset = atlasRegion.yOffset;
        slot.origin.x = static_cast<f32>(atlasRegion.width / 2 - textureXOffset) / atlasRegion.subrectWidth;
        slot.origin.y = static_cast<f32>(atlasRegion.height / 2 - textureYOffset) / atlasRegion.subrectHeight;

        if (slot.textureRotate) {
            std::swap(slot.subrect.width, slot.subrect.height);
            std::swap(slot.origin.x, slot.origin.y);
        }

        slots.push_back(std::move(slot));
        ++drawOrder;
    }

    return std::make_tuple(std::move(slots), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<Skin>, std::unique_ptr<Error>>
createSkin(
    const SkinData& skinData,
    const TextureAtlas& textureAtlas,
    std::string_view skinName)
{
    const auto* skin = skinData.findSkin(computeStringHash32(skinName));
    if (skin == nullptr) {
        auto err = errors::make(pomdog::format("cannot find skin '{}'", skinName));
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [slots, slotsErr] = createSlots(*skin, textureAtlas);
    if (slotsErr != nullptr) {
        auto wrapped = errors::wrap(std::move(slotsErr), pomdog::format("failed to create skin '{}'", skinName));
        return std::make_tuple(nullptr, std::move(wrapped));
    }

    auto result = std::make_shared<Skin>(std::move(slots));
    return std::make_tuple(std::move(result), nullptr);
}

} // namespace pomdog::skeletal2d

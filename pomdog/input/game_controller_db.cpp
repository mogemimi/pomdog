// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/game_controller_db.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/platform.h"
#include "pomdog/input/gamepad_uuid.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/game_controller_db_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

[[nodiscard]] constexpr const char*
getPlatformSuffix() noexcept
{
#if defined(POMDOG_PLATFORM_WIN32)
    return ":windows";
#elif defined(POMDOG_PLATFORM_LINUX)
    return ":linux";
#elif defined(POMDOG_PLATFORM_MACOSX)
    return ":macos";
#elif defined(POMDOG_PLATFORM_ANDROID)
    return ":android";
#elif defined(POMDOG_PLATFORM_APPLE_IOS)
    return ":ios";
#else
    return ":unknown";
#endif
}

[[nodiscard]] detail::GamepadMappingEntry
getDefaultMappings() noexcept
{
    detail::GamepadMappingEntry mappings = {};

#if defined(POMDOG_PLATFORM_LINUX)
    // NOTE: Please see this header
    // https://github.com/torvalds/linux/blob/4982327ff6755377a8a66e84113f496f3a6c53bc/include/uapi/linux/input-event-codes.h#L379-L398
    mappings.buttons = {{
        detail::ButtonKind::A,
        detail::ButtonKind::B,
        detail::ButtonKind::X,
        detail::ButtonKind::Y,
        detail::ButtonKind::LeftShoulder,
        detail::ButtonKind::RightShoulder,
        detail::ButtonKind::LeftMenu,
        detail::ButtonKind::RightMenu,
        detail::ButtonKind::Guide,
        detail::ButtonKind::LeftStick,
        detail::ButtonKind::RightStick,
        detail::ButtonKind::Extra1,
        detail::ButtonKind::Extra2,
        detail::ButtonKind::Invalid,
        detail::ButtonKind::Invalid,
        detail::ButtonKind::Invalid,
    }};
#elif defined(POMDOG_PLATFORM_MACOSX)
    mappings.buttons = {{
        detail::ButtonKind::A,
        detail::ButtonKind::B,
        detail::ButtonKind::X,
        detail::ButtonKind::Y,
        detail::ButtonKind::LeftShoulder,
        detail::ButtonKind::RightShoulder,
        detail::ButtonKind::LeftTrigger,
        detail::ButtonKind::RightTrigger,
        detail::ButtonKind::LeftMenu,
        detail::ButtonKind::RightMenu,
        detail::ButtonKind::LeftStick,
        detail::ButtonKind::RightStick,
        detail::ButtonKind::Guide,
        detail::ButtonKind::Extra1,
        detail::ButtonKind::Extra2,
        detail::ButtonKind::Invalid,
    }};
#else
    mappings.buttons = {{
        detail::ButtonKind::A,
        detail::ButtonKind::B,
        detail::ButtonKind::X,
        detail::ButtonKind::Y,
        detail::ButtonKind::LeftShoulder,
        detail::ButtonKind::RightShoulder,
        detail::ButtonKind::LeftMenu,
        detail::ButtonKind::RightMenu,
        detail::ButtonKind::LeftStick,
        detail::ButtonKind::RightStick,
        detail::ButtonKind::Guide,
        detail::ButtonKind::Extra1,
        detail::ButtonKind::Extra2,
        detail::ButtonKind::Invalid,
        detail::ButtonKind::Invalid,
        detail::ButtonKind::Invalid,
    }};
#endif

    mappings.axes = {{
        detail::AxisMapper{detail::ThumbStickKind::LeftStickX, detail::ButtonKind::Invalid, detail::ButtonKind::Invalid},
        detail::AxisMapper{detail::ThumbStickKind::LeftStickY, detail::ButtonKind::Invalid, detail::ButtonKind::Invalid},
        detail::AxisMapper{detail::ThumbStickKind::Invalid, detail::ButtonKind::LeftTrigger, detail::ButtonKind::Invalid},
        detail::AxisMapper{detail::ThumbStickKind::RightStickX, detail::ButtonKind::Invalid, detail::ButtonKind::Invalid},
        detail::AxisMapper{detail::ThumbStickKind::RightStickY, detail::ButtonKind::Invalid, detail::ButtonKind::Invalid},
        detail::AxisMapper{detail::ThumbStickKind::Invalid, detail::ButtonKind::RightTrigger, detail::ButtonKind::Invalid},
    }};

    mappings.hats = {{
        detail::ButtonKind::DPadUp,
        detail::ButtonKind::DPadRight,
        detail::ButtonKind::DPadDown,
        detail::ButtonKind::DPadLeft,
    }};

    return mappings;
}

void populateMappingsFromDB(
    detail::GamepadMappingEntry& mappings,
    std::string& deviceName,
    const pomdogschemas::GameControllerMapping* result) noexcept
{
    POMDOG_ASSERT(result != nullptr);

    if (result->name() != nullptr) {
        deviceName = result->name()->str();
    }

    // Reset to invalid before populating from database
    std::fill(std::begin(mappings.buttons), std::end(mappings.buttons), detail::ButtonKind::Invalid);
    for (auto& axis : mappings.axes) {
        axis.thumbStick = detail::ThumbStickKind::Invalid;
        axis.positiveTrigger = detail::ButtonKind::Invalid;
        axis.negativeTrigger = detail::ButtonKind::Invalid;
    }
    std::fill(std::begin(mappings.hats), std::end(mappings.hats), detail::ButtonKind::Invalid);

    if (const auto* buttons = result->buttons(); buttons != nullptr) {
        const auto count = std::min(
            static_cast<int>(buttons->size()),
            static_cast<int>(mappings.buttons.size()));
        for (int i = 0; i < count; ++i) {
            mappings.buttons[i] = static_cast<detail::ButtonKind>(buttons->Get(i));
        }
    }

    if (const auto* axes = result->axes(); axes != nullptr) {
        const auto count = std::min(
            static_cast<int>(axes->size()),
            static_cast<int>(mappings.axes.size()));
        for (int i = 0; i < count; ++i) {
            const auto* axis = axes->Get(i);
            mappings.axes[i].thumbStick = static_cast<detail::ThumbStickKind>(axis->thumb_stick());
            mappings.axes[i].positiveTrigger = static_cast<detail::ButtonKind>(axis->positive_trigger());
            mappings.axes[i].negativeTrigger = static_cast<detail::ButtonKind>(axis->negative_trigger());
        }
    }

    if (const auto* hats = result->hats(); hats != nullptr) {
        const auto count = std::min(
            static_cast<int>(hats->size()),
            static_cast<int>(mappings.hats.size()));
        for (int i = 0; i < count; ++i) {
            mappings.hats[i] = static_cast<detail::ButtonKind>(hats->Get(i));
        }
    }
}

class GameControllerDBImpl final : public GameControllerDB {
private:
    std::unique_ptr<u8[]> storage_;
    std::size_t size_;

public:
    GameControllerDBImpl() noexcept = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::unique_ptr<u8[]> storage, std::size_t size) noexcept
    {
        if (storage == nullptr || size == 0) {
            return errors::make("invalid storage");
        }
        storage_ = std::move(storage);
        size_ = size;

        // NOTE: Verify the FlatBuffer
        auto verifier = flatbuffers::Verifier(storage_.get(), size_);
        if (!pomdogschemas::VerifyGameControllerDBBuffer(verifier)) {
            return errors::make("invalid GameControllerDB data");
        }

        return nullptr;
    }

    [[nodiscard]] std::tuple<detail::GamepadMappingEntry, std::string>
    getMappings(const GamepadUUID& uuid) const noexcept override
    {
        return getMappings(uuid.toString());
    }

    [[nodiscard]] std::tuple<detail::GamepadMappingEntry, std::string>
    getMappings(const std::string& uuidString) const noexcept override
    {
        auto mappings = getDefaultMappings();
        std::string deviceName;

        if (storage_ != nullptr && size_ > 0) {
            const auto* db = pomdogschemas::GetGameControllerDB(storage_.get());
            if (db != nullptr && db->mappings() != nullptr) {
                const std::string keyStr = uuidString + getPlatformSuffix();
                const auto key = computeStringHash64(keyStr);

                if (const auto* result = db->mappings()->LookupByKey(key); result != nullptr) {
                    populateMappingsFromDB(mappings, deviceName, result);
                }
            }
        }

        return std::make_tuple(mappings, deviceName);
    }
};

class GameControllerDBDummy final : public GameControllerDB {
public:
    [[nodiscard]] std::tuple<detail::GamepadMappingEntry, std::string>
    getMappings([[maybe_unused]] const GamepadUUID& uuid) const noexcept override
    {
        return std::make_tuple(getDefaultMappings(), std::string{});
    }

    [[nodiscard]] std::tuple<detail::GamepadMappingEntry, std::string>
    getMappings([[maybe_unused]] const std::string& uuidString) const noexcept override
    {
        return std::make_tuple(getDefaultMappings(), std::string{});
    }
};

} // namespace

GameControllerDB::GameControllerDB() noexcept = default;

GameControllerDB::~GameControllerDB() noexcept = default;

std::tuple<std::shared_ptr<GameControllerDB>, std::unique_ptr<Error>>
createGameControllerDB(std::unique_ptr<u8[]> storage, std::size_t size)
{
    auto db = std::make_shared<GameControllerDBImpl>();
    if (auto err = db->initialize(std::move(storage), size); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(db, nullptr);
}

std::shared_ptr<GameControllerDB>
createGameControllerDBDummy()
{
    return std::make_shared<GameControllerDBDummy>();
}

} // namespace pomdog

// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/game_controller_db.h"
#include "pomdog/input/gamepad_uuid.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::createGameControllerDBDummy;
using pomdog::GameControllerDB;
using pomdog::GamepadUUID;
using pomdog::detail::AxisMapper;
using pomdog::detail::ButtonKind;
using pomdog::detail::GamepadMappingEntry;
using pomdog::detail::ThumbStickKind;

TEST_CASE("GameControllerDB")
{
    SUBCASE("createGameControllerDBDummy returns non-null")
    {
        auto db = createGameControllerDBDummy();
        REQUIRE(db.get() != nullptr);
    }
    SUBCASE("dummy getMappings returns default buttons by UUID struct")
    {
        auto db = createGameControllerDBDummy();
        REQUIRE(db.get() != nullptr);

        GamepadUUID uuid;
        uuid.busType = 0x03;
        uuid.vendorID = 0x054C;
        uuid.productID = 0x0CE6;
        uuid.versionNumber = 0x0100;

        auto [mappings, name] = db->getMappings(uuid);

        // Dummy always returns empty name
        REQUIRE(name.empty());

        // Default mappings should have A button at index 0
        REQUIRE(mappings.buttons[0] == ButtonKind::A);
        REQUIRE(mappings.buttons[1] == ButtonKind::B);
        REQUIRE(mappings.buttons[2] == ButtonKind::X);
        REQUIRE(mappings.buttons[3] == ButtonKind::Y);
    }
    SUBCASE("dummy getMappings returns default buttons by string")
    {
        auto db = createGameControllerDBDummy();
        REQUIRE(db.get() != nullptr);

        auto [mappings, name] = db->getMappings(std::string("03000000de280000ff11000001000000"));

        REQUIRE(name.empty());
        REQUIRE(mappings.buttons[0] == ButtonKind::A);
    }
    SUBCASE("dummy getMappings returns default axes")
    {
        auto db = createGameControllerDBDummy();
        REQUIRE(db.get() != nullptr);

        auto [mappings, name] = db->getMappings(std::string("any-uuid"));

        // Default axis 0: LeftStickX
        REQUIRE(mappings.axes[0].thumbStick == ThumbStickKind::LeftStickX);
        REQUIRE(mappings.axes[0].positiveTrigger == ButtonKind::Invalid);
        REQUIRE(mappings.axes[0].negativeTrigger == ButtonKind::Invalid);

        // Default axis 1: LeftStickY
        REQUIRE(mappings.axes[1].thumbStick == ThumbStickKind::LeftStickY);

        // Default axis 2: LeftTrigger
        REQUIRE(mappings.axes[2].thumbStick == ThumbStickKind::Invalid);
        REQUIRE(mappings.axes[2].positiveTrigger == ButtonKind::LeftTrigger);

        // Default axis 3: RightStickX
        REQUIRE(mappings.axes[3].thumbStick == ThumbStickKind::RightStickX);

        // Default axis 4: RightStickY
        REQUIRE(mappings.axes[4].thumbStick == ThumbStickKind::RightStickY);

        // Default axis 5: RightTrigger
        REQUIRE(mappings.axes[5].thumbStick == ThumbStickKind::Invalid);
        REQUIRE(mappings.axes[5].positiveTrigger == ButtonKind::RightTrigger);

        // Default hats: up, right, down, left
        REQUIRE(mappings.hats[0] == ButtonKind::DPadUp);
        REQUIRE(mappings.hats[1] == ButtonKind::DPadRight);
        REQUIRE(mappings.hats[2] == ButtonKind::DPadDown);
        REQUIRE(mappings.hats[3] == ButtonKind::DPadLeft);
    }
    SUBCASE("dummy results are consistent across calls")
    {
        auto db = createGameControllerDBDummy();
        auto [m1, n1] = db->getMappings(std::string("uuid-a"));
        auto [m2, n2] = db->getMappings(std::string("uuid-b"));

        // Both calls return the same default mappings
        REQUIRE(m1.buttons[0] == m2.buttons[0]);
        REQUIRE(m1.axes[0].thumbStick == m2.axes[0].thumbStick);
    }
}

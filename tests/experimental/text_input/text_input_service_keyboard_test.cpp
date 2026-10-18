// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_service_keyboard.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/keys.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <memory>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace textinput = pomdog::textinput;
using pomdog::Keys;

namespace {

/// Reports the text that a test sets and no keys.
class FakeKeyboard final : public pomdog::Keyboard {
public:
    std::string text;

    FakeKeyboard() = default;

    FakeKeyboard(const FakeKeyboard&) = delete;
    FakeKeyboard& operator=(const FakeKeyboard&) = delete;
    FakeKeyboard(FakeKeyboard&&) = delete;
    FakeKeyboard& operator=(FakeKeyboard&&) = delete;

    ~FakeKeyboard() override = default;

    [[nodiscard]] bool
    isKeyDown(Keys) const noexcept override
    {
        return false;
    }

    [[nodiscard]] bool
    isKeyUp(Keys) const noexcept override
    {
        return true;
    }

    [[nodiscard]] bool
    isControlKeyDown() const noexcept override
    {
        return false;
    }

    [[nodiscard]] bool
    isShiftKeyDown() const noexcept override
    {
        return false;
    }

    [[nodiscard]] bool
    isAltKeyDown() const noexcept override
    {
        return false;
    }

    [[nodiscard]] bool
    isSuperKeyDown() const noexcept override
    {
        return false;
    }

    [[nodiscard]] bool
    isAnyKeyDown() const noexcept override
    {
        return false;
    }

    [[nodiscard]] std::string_view
    getTextInput() const noexcept override
    {
        return text;
    }
};

[[nodiscard]] textinput::TextFieldState
makeField(pomdog::u64 id)
{
    textinput::TextFieldState field;
    field.id = id;
    return field;
}

} // namespace

TEST_CASE("TextInputServiceKeyboard")
{
    auto keyboard = std::make_shared<FakeKeyboard>();
    textinput::TextInputServiceKeyboard service{keyboard};

    SUBCASE("provides only committed text")
    {
        const auto capabilities = service.getCapabilities();
        REQUIRE(capabilities.committedText == textinput::FeatureSupport::Available);
        REQUIRE(capabilities.composition == textinput::FeatureSupport::NotImplemented);
        REQUIRE(capabilities.softwareKeyboard == textinput::FeatureSupport::NotImplemented);

        const auto keyboardState = service.getSoftwareKeyboardState();
        REQUIRE(keyboardState.visibility == textinput::SoftwareKeyboardVisibility::Unknown);
        REQUIRE_FALSE(keyboardState.area.has_value());
    }
    SUBCASE("drops text while no field has focus")
    {
        keyboard->text = "a";
        service.update();
        REQUIRE(service.getEvents().empty());
    }
    SUBCASE("commits text without control characters to the focused field")
    {
        service.setFocusedField(makeField(7));
        keyboard->text = "a\rb\x7F";
        service.update();

        const auto events = service.getEvents();
        REQUIRE(events.size() == 1);
        REQUIRE(events[0].kind == textinput::TextInputEventKind::Commit);
        REQUIRE(events[0].fieldID == 7);
        REQUIRE(events[0].text == "ab");

        keyboard->text = "";
        service.update();
        REQUIRE(service.getEvents().empty());
    }
    SUBCASE("sends no event for control characters alone")
    {
        service.setFocusedField(makeField(7));
        keyboard->text = "\b";
        service.update();
        REQUIRE(service.getEvents().empty());
    }
    SUBCASE("stops when the focus is cleared")
    {
        service.setFocusedField(makeField(7));
        service.clearFocusedField();
        keyboard->text = "a";
        service.update();
        REQUIRE(service.getEvents().empty());
    }
}

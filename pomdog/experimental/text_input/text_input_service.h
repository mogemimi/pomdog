// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/text_input/software_keyboard_state.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_capabilities.h"
#include "pomdog/experimental/text_input/text_input_event.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {

/// Connects text fields to the platform's text input: typed text, input method
/// editors (IME), and software keyboards.
///
/// A service knows nothing about how a GUI stores or draws text. In each frame
/// the application collects the platform's events with `update()`, hands
/// `getEvents()` to its GUI, and then tells the service which field has focus:
/// ```cpp
/// textInput->update();
/// gui.applyTextInput(textInput->getEvents());
/// if (const auto field = gui.getFocusedTextField(); field) {
///     textInput->setFocusedField(*field);
/// }
/// else {
///     textInput->clearFocusedField();
/// }
/// ```
/// Platforms differ in what they offer; `getCapabilities()` reports each
/// feature. Members whose feature is not available do nothing.
///
/// A service is not thread-safe. Use it from the thread that runs the game loop.
class TextInputService {
public:
    TextInputService() noexcept;

    TextInputService(const TextInputService&) = delete;
    TextInputService& operator=(const TextInputService&) = delete;

    virtual ~TextInputService();

    /// Returns which features the service provides.
    [[nodiscard]] virtual TextInputCapabilities
    getCapabilities() const noexcept = 0;

    /// Collects the events that arrived since the last call. Call it once per
    /// frame after the host processed the platform's events, such as at the
    /// beginning of `Game::update()`.
    virtual void
    update() = 0;

    /// Returns the events that the last `update()` collected, oldest first.
    /// The span stays valid until the next `update()`.
    [[nodiscard]] virtual std::span<const TextInputEvent>
    getEvents() const noexcept = 0;

    /// Directs text input to field and passes its caret area and surrounding
    /// text to the platform. Call it in every frame while a field has focus.
    ///
    /// A field with a different id cancels the composition of the previous
    /// field, so that text composed for one field never reaches another.
    virtual void
    setFocusedField(const TextFieldState& field) = 0;

    /// Stops text input: cancels a composition, turns the input method editor
    /// off so that keys reach the game, and hides a software keyboard that
    /// shows for the field.
    virtual void
    clearFocusedField() = 0;

    /// Passes the areas of the text fields that a tap focuses. Call it after
    /// each frame that lays out the fields; the areas replace the previous
    /// ones.
    ///
    /// A browser on a phone shows its software keyboard only for an element
    /// focused while it handles the tap, before the GUI sees the tap. A tap
    /// that ends in one of these areas focuses the service's element at once,
    /// so that the keyboard shows when the GUI then focuses the field.
    /// Platforms that need no such help ignore the areas.
    virtual void
    setTextFieldAreas(std::span<const TextFieldArea> areas) = 0;

    /// Asks the platform to show the software keyboard for the focused field.
    ///
    /// The platform may ignore the request, for example a browser outside the
    /// handling of a tap. Observe the result with `getSoftwareKeyboardState()`.
    virtual void
    showSoftwareKeyboard() = 0;

    /// Asks the platform to hide the software keyboard.
    virtual void
    hideSoftwareKeyboard() = 0;

    /// Returns whether the software keyboard shows and the area it covers.
    [[nodiscard]] virtual SoftwareKeyboardState
    getSoftwareKeyboardState() const noexcept = 0;
};

} // namespace pomdog::textinput

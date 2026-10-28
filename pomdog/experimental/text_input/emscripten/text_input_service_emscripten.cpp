// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/emscripten/text_input_service_emscripten.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten.h>

#include <algorithm>
#include <array>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: The JavaScript below is part of a C macro. It uses double quotes for
// strings and no comments, because the preprocessor reads it as C tokens, and
// clang-format must not touch it. Change kinds match ElementChange, and
// purposes match TextInputPurpose. The elements carry the
// data-pomdog-text-input attribute, by which the browser clipboard service
// takes the user's pastes into them.
//
// The current element holds the text and selection of the focused field. It
// reports each state it reaches, and TextMirror turns the changes into events.
//
// With a mouse, the element is an invisible sliver at the caret, where the
// candidate window opens, and the GUI edits the text. Keys that the GUI handles
// itself, such as Backspace and the arrow keys, leave the element alone, so
// that their edits do not happen twice; an input method keeps the keys it
// processes (keyCode 229).
//
// After touches, the element shows over the field while the GUI edits it, and
// the browser edits the text with the platform's own caret, selection,
// magnifier, menus, and keyboard trackpad. The element then carries the
// data-pomdog-native-editing attribute, which tells the keyboard backend to
// pass only Enter and Escape to the game. CSS variables style it. A password
// never reaches the element, so the GUI keeps editing it with the element
// invisible and empty.
//
// There are two elements. A phone keeps its keyboard when the focus moves from
// one element to another, and shows the layout of the new one, but refocusing
// the same element keeps the old layout. A field that needs another keyboard
// layout therefore moves the focus to the other element, and so does ending a
// composition, whose blur reports the end to the previous field. Otherwise
// moving to another field keeps the element focused, because a tap may have
// focused it for the new field before the GUI moved the focus, and a phone
// shows its keyboard only for that. For the same reason, after the GUI let its
// field go, the element stays focused while a finger is down and for a second
// after a tap on a text field area, since the GUI may focus the next field
// only when the finger lifts, as devgui does for a drag field.
//
// A key that the input method does not process ends a composition that ended
// without an event.
//
// Browsers on phones show the software keyboard only for an element focused
// while they handle a tap. After touches, the element is therefore focused
// when a tap ends in a text field area, not when the GUI focuses the field.
// A tap that the GUI does not turn into focus loses the element again. Focus
// moves the caret of iOS to the start, so the selection is set again. The
// font is at least 16 pixels, below which iOS zooms into a focused element.
//
// EM_JS defines variables that only the linker reads, without declarations.
POMDOG_EMCC_SUPPRESS_WARNING_PUSH
POMDOG_EMCC_SUPPRESS_WARNING("-Wmissing-variable-declarations")

// clang-format off
EM_JS_DEPS(pomdog_text_input, "$UTF8ToString,$stringToUTF8,$lengthBytesUTF8");

EM_JS(int, pomdog_text_input_create, (const char* canvasSelector), {
    if (Module.pomdogTextInput) {
        return 0;
    }
    var selector = UTF8ToString(canvasSelector);
    var canvas = (selector.length > 0) ? document.querySelector(selector) : Module.canvas;
    if (!canvas) {
        return 0;
    }

    var state = {
        canvas: canvas,
        inputs: [],
        input: null,
        queue: [],
        composing: false,
        active: false,
        layoutKey: "",
        areas: [],
        touch: false,
        tap: null,
        value: "",
        start: 0,
        end: 0,
        backward: false,
        caretRect: null,
        native: false,
        touchDown: false,
        claimUntil: 0,
        purpose: 0
    };
    var modes = ["text", "text", "numeric", "decimal", "email", "url", "search"];
    var editingKeys = ["Backspace", "Delete", "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Home", "End", "PageUp", "PageDown"];
    var blockedInputs = ["historyUndo", "historyRedo", "deleteByCut", "deleteByDrag", "insertFromDrop"];

    state.hide = function(input) {
        var style = input.style;
        input.removeAttribute("data-pomdog-native-editing");
        style.left = "0px";
        style.top = "0px";
        style.width = "1px";
        style.height = "1px";
        style.lineHeight = "1px";
        style.fontSize = "16px";
        style.fontFamily = "";
        style.padding = "0";
        style.borderRadius = "0";
        style.background = "transparent";
        style.color = "transparent";
        style.caretColor = "transparent";
        style.opacity = "0";
        style.pointerEvents = "none";
    };
    state.configure = function(input, purpose) {
        input.type = (purpose === 1) ? "password" : "text";
        input.inputMode = modes[purpose] || "text";
        input.enterKeyHint = (purpose === 6) ? "search" : "done";
    };
    state.isConfigured = function(input, purpose) {
        return (input.type === ((purpose === 1) ? "password" : "text")) && (input.inputMode === (modes[purpose] || "text")) && (input.enterKeyHint === ((purpose === 6) ? "search" : "done"));
    };
    state.report = function(input, kind) {
        if (input !== state.input) {
            return;
        }
        var value = input.value;
        var start = input.selectionStart || 0;
        var end = input.selectionEnd || 0;
        var backward = (input.selectionDirection === "backward");
        if ((kind === 0) && (value === state.value) && (start === state.start) && (end === state.end) && (backward === state.backward)) {
            return;
        }
        state.value = value;
        state.start = start;
        state.end = end;
        state.backward = backward;
        var startBytes = lengthBytesUTF8(value.substring(0, start));
        var endBytes = lengthBytesUTF8(value.substring(0, end));
        state.queue.push({kind: kind, text: value, caret: backward ? startBytes : endBytes, anchor: backward ? endBytes : startBytes});
    };
    state.focusInput = function() {
        var input = state.input;
        if (document.activeElement !== input) {
            input.focus({preventScroll: true});
            input.setSelectionRange(state.start, state.end, state.backward ? "backward" : "forward");
        }
    };
    state.moveTo = function(purpose) {
        var current = state.input;
        var focused = (document.activeElement === current);
        if (focused && (state.composing || !state.isConfigured(current, purpose))) {
            var next = (current === state.inputs[0]) ? state.inputs[1] : state.inputs[0];
            state.configure(next, purpose);
            next.value = "";
            next.style.left = current.style.left;
            next.style.top = current.style.top;
            next.focus({preventScroll: true});
            state.input = next;
            current.value = "";
            state.hide(current);
            state.layoutKey = "";
        }
        else if (!state.isConfigured(current, purpose)) {
            state.configure(current, purpose);
        }
        state.purpose = purpose;
        state.composing = false;
        state.input.value = "";
        state.value = "";
        state.start = 0;
        state.end = 0;
        state.backward = false;
    };
    state.layout = function() {
        var c = state.caretRect;
        if (!c) {
            return;
        }
        var input = state.input;
        var box = null;
        if (state.touch && state.active && (input.type !== "password")) {
            var centerX = c.x + c.width * 0.5;
            var centerY = c.y + c.height * 0.5;
            for (var i = 0; i < state.areas.length; ++i) {
                var area = state.areas[i];
                if ((centerX >= area.x) && (centerY >= area.y) && (centerX <= area.x + area.width) && (centerY <= area.y + area.height)) {
                    box = {x: area.x, y: area.y, width: Math.max(1, area.width), height: Math.max(1, area.height)};
                    break;
                }
            }
        }
        var native = (box !== null);
        if (!native) {
            box = {x: c.x, y: c.y, width: 1, height: Math.max(1, c.height)};
        }
        var rect = canvas.getBoundingClientRect();
        var left = rect.left + box.x;
        var top = rect.top + box.y;
        var key = left + "," + top + "," + box.width + "," + box.height + "," + native;
        if (key === state.layoutKey) {
            return;
        }
        state.layoutKey = key;
        state.native = native;
        var style = input.style;
        style.left = left + "px";
        style.top = top + "px";
        style.width = box.width + "px";
        style.height = box.height + "px";
        style.lineHeight = box.height + "px";
        if (native) {
            input.setAttribute("data-pomdog-native-editing", "");
            style.fontSize = Math.max(16, Math.floor(box.height * 0.65)) + "px";
            style.fontFamily = "var(--pomdog-text-input-font-family, system-ui, sans-serif)";
            style.padding = "0 var(--pomdog-text-input-padding, 4px)";
            style.borderRadius = "var(--pomdog-text-input-radius, 0px)";
            style.background = "var(--pomdog-text-input-background, Canvas)";
            style.color = "var(--pomdog-text-input-color, CanvasText)";
            style.caretColor = "auto";
            style.opacity = "1";
            style.pointerEvents = "auto";
        }
        else {
            input.removeAttribute("data-pomdog-native-editing");
            style.fontSize = Math.max(16, Math.floor(box.height * 0.8)) + "px";
            style.fontFamily = "";
            style.padding = "0";
            style.borderRadius = "0";
            style.background = "transparent";
            style.color = "transparent";
            style.caretColor = "transparent";
            style.opacity = "0";
            style.pointerEvents = "none";
        }
    };

    var makeInput = function() {
        var input = document.createElement("input");
        input.type = "text";
        input.autocomplete = "off";
        input.spellcheck = false;
        input.tabIndex = -1;
        input.setAttribute("autocorrect", "off");
        input.setAttribute("autocapitalize", "off");
        input.setAttribute("aria-hidden", "true");
        input.setAttribute("data-pomdog-text-input", "");
        var style = input.style;
        style.position = "fixed";
        style.margin = "0";
        style.border = "0";
        style.outline = "none";
        style.boxSizing = "border-box";
        style.appearance = "none";
        style.webkitAppearance = "none";
        state.hide(input);

        input.addEventListener("compositionstart", function() {
            if (input === state.input) {
                state.composing = true;
            }
        });
        input.addEventListener("compositionend", function() {
            if (input === state.input) {
                state.composing = false;
                state.report(input, 2);
            }
        });
        input.addEventListener("input", function(e) {
            state.report(input, (state.composing || e.isComposing) ? 1 : 0);
        });
        input.addEventListener("select", function() {
            if (!state.composing) {
                state.report(input, 0);
            }
        });
        input.addEventListener("keydown", function(e) {
            if (e.key === "Tab") {
                e.preventDefault();
                return;
            }
            if (e.isComposing || (e.keyCode === 229) || (input !== state.input)) {
                return;
            }
            if (state.composing) {
                state.composing = false;
                state.report(input, 2);
            }
            if (!state.native && (editingKeys.indexOf(e.key) >= 0)) {
                e.preventDefault();
            }
        });
        input.addEventListener("beforeinput", function(e) {
            if (!state.native && (blockedInputs.indexOf(e.inputType) >= 0)) {
                e.preventDefault();
            }
        });
        document.body.appendChild(input);
        return input;
    };
    state.inputs = [makeInput(), makeInput()];
    state.input = state.inputs[0];

    state.onSelectionChange = function() {
        if (!state.composing && (document.activeElement === state.input)) {
            state.report(state.input, 0);
        }
    };
    document.addEventListener("selectionchange", state.onSelectionChange);

    state.onPointerDown = function(e) {
        state.touch = (e.pointerType === "touch");
        if (!state.touch && state.active) {
            state.focusInput();
        }
        state.layout();
    };
    state.onTouchStart = function(e) {
        state.touch = true;
        state.touchDown = (e.touches.length > 0);
        var t = e.changedTouches[0];
        if ((e.touches.length !== 1) || !t) {
            state.tap = null;
            return;
        }
        state.tap = {id: t.identifier, x: t.clientX, y: t.clientY, travel: 0, time: e.timeStamp};
    };
    state.onTouchMove = function(e) {
        var tap = state.tap;
        if (!tap) {
            return;
        }
        for (var i = 0; i < e.changedTouches.length; ++i) {
            var t = e.changedTouches[i];
            if (t.identifier === tap.id) {
                tap.travel += Math.abs(t.clientX - tap.x) + Math.abs(t.clientY - tap.y);
                tap.x = t.clientX;
                tap.y = t.clientY;
            }
        }
    };
    state.onTouchEnd = function(e) {
        var tap = state.tap;
        state.tap = null;
        state.touchDown = (e.touches.length > 0);
        if (!tap || (e.type !== "touchend") || (e.touches.length !== 0) || (tap.travel >= 8) || (e.timeStamp - tap.time >= 500)) {
            return;
        }
        var rect = canvas.getBoundingClientRect();
        var x = tap.x - rect.left;
        var y = tap.y - rect.top;
        for (var i = 0; i < state.areas.length; ++i) {
            var area = state.areas[i];
            if ((x >= area.x) && (y >= area.y) && (x < area.x + area.width) && (y < area.y + area.height)) {
                if (!state.active) {
                    state.moveTo(area.purpose);
                }
                state.focusInput();
                state.layout();
                state.claimUntil = performance.now() + 1000;
                setTimeout(function() {
                    var input = state.input;
                    if (!state.active && !state.touchDown && (performance.now() >= state.claimUntil) && (document.activeElement === input)) {
                        input.blur();
                    }
                }, 1050);
                return;
            }
        }
    };
    var passive = {capture: true, passive: true};
    canvas.addEventListener("pointerdown", state.onPointerDown, true);
    canvas.addEventListener("touchstart", state.onTouchStart, passive);
    canvas.addEventListener("touchmove", state.onTouchMove, passive);
    canvas.addEventListener("touchend", state.onTouchEnd, passive);
    canvas.addEventListener("touchcancel", state.onTouchEnd, passive);

    Module.pomdogTextInput = state;
    return 1;
});

EM_JS(void, pomdog_text_input_destroy, (), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    var passive = {capture: true, passive: true};
    document.removeEventListener("selectionchange", state.onSelectionChange);
    state.canvas.removeEventListener("pointerdown", state.onPointerDown, true);
    state.canvas.removeEventListener("touchstart", state.onTouchStart, passive);
    state.canvas.removeEventListener("touchmove", state.onTouchMove, passive);
    state.canvas.removeEventListener("touchend", state.onTouchEnd, passive);
    state.canvas.removeEventListener("touchcancel", state.onTouchEnd, passive);
    state.inputs[0].remove();
    state.inputs[1].remove();
    delete Module.pomdogTextInput;
});

EM_JS(void, pomdog_text_input_focus, (int purpose), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    state.moveTo(purpose);
    state.active = true;
    if (!state.touch) {
        state.focusInput();
    }
    state.layout();
});

EM_JS(void, pomdog_text_input_blur, (), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    state.active = false;
    if (document.activeElement === state.input) {
        state.input.blur();
    }
    state.composing = false;
    state.input.value = "";
    state.value = "";
    state.start = 0;
    state.end = 0;
    state.backward = false;
    state.layout();
});

EM_JS(void, pomdog_text_input_leave_field, (), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    state.moveTo(state.purpose);
});

EM_JS(int, pomdog_text_input_release, (), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return 1;
    }
    if (state.touchDown || (performance.now() < state.claimUntil)) {
        return 0;
    }
    if (document.activeElement === state.input) {
        state.input.blur();
    }
    return 1;
});

EM_JS(void, pomdog_text_input_deactivate, (), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    state.active = false;
    state.layout();
});

EM_JS(void, pomdog_text_input_set_text, (const char* text, int size, int caret, int anchor), {
    var state = Module.pomdogTextInput;
    if (!state || state.composing) {
        return;
    }
    var input = state.input;
    var value = (size > 0) ? UTF8ToString(text, size) : "";
    var caretUnits = (caret > 0) ? UTF8ToString(text, caret).length : 0;
    var anchorUnits = (anchor > 0) ? UTF8ToString(text, anchor).length : 0;
    if (input.value !== value) {
        input.value = value;
    }
    var start = Math.min(caretUnits, anchorUnits);
    var end = Math.max(caretUnits, anchorUnits);
    var backward = (caretUnits < anchorUnits);
    input.setSelectionRange(start, end, backward ? "backward" : "forward");
    state.value = value;
    state.start = start;
    state.end = end;
    state.backward = backward;
});

EM_JS(void, pomdog_text_input_set_caret, (float x, float y, float width, float height), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    state.caretRect = {x: x, y: y, width: width, height: height};
    state.layout();
});

EM_JS(void, pomdog_text_input_set_areas, (const float* data, int count), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    var areas = [];
    var index = data >> 2;
    for (var i = 0; i < count; ++i) {
        var k = index + i * 5;
        areas.push({x: HEAPF32[k], y: HEAPF32[k + 1], width: HEAPF32[k + 2], height: HEAPF32[k + 3], purpose: HEAPF32[k + 4] | 0});
    }
    state.areas = areas;
    state.layout();
});

EM_JS(int, pomdog_text_input_peek, (int* fields), {
    var state = Module.pomdogTextInput;
    if (!state || (state.queue.length === 0)) {
        return 0;
    }
    var e = state.queue[0];
    var index = fields >> 2;
    HEAP32[index] = e.kind;
    HEAP32[index + 1] = lengthBytesUTF8(e.text);
    HEAP32[index + 2] = e.caret;
    HEAP32[index + 3] = e.anchor;
    return 1;
});

EM_JS(void, pomdog_text_input_pop, (char* buffer, int size), {
    var state = Module.pomdogTextInput;
    var e = state.queue.shift();
    if (size > 0) {
        stringToUTF8(e.text, buffer, size + 1);
    }
});
// clang-format on

POMDOG_EMCC_SUPPRESS_WARNING_POP

namespace pomdog::textinput::detail::emscripten {
namespace {

/// Identifies what the element reports with its state.
enum class ElementChange : int {
    /// The user edited the text or moved the selection outside a composition.
    Edit = 0,

    /// An input method changed the text it composes.
    Composition = 1,

    /// The composition ended.
    CompositionEnd = 2,
};

static_assert(static_cast<int>(TextInputPurpose::Password) == 1);
static_assert(static_cast<int>(TextInputPurpose::Search) == 6);

[[nodiscard]] bool
isControlByte(char c) noexcept
{
    const auto byte = static_cast<u8>(c);
    return (byte < 0x20) || (byte == 0x7F);
}

} // namespace

TextInputServiceEmscripten::TextInputServiceEmscripten() noexcept = default;

TextInputServiceEmscripten::~TextInputServiceEmscripten()
{
    if (initialized_) {
        pomdog_text_input_blur();
        pomdog_text_input_destroy();
    }
}

std::unique_ptr<Error>
TextInputServiceEmscripten::initialize(const std::string& canvasSelector) noexcept
{
    POMDOG_ASSERT(!initialized_);
    if (pomdog_text_input_create(canvasSelector.c_str()) == 0) {
        return errors::make("failed to create the input element; another service exists, or the canvas was not found");
    }
    initialized_ = true;
    return nullptr;
}

TextInputCapabilities TextInputServiceEmscripten::getCapabilities() const noexcept
{
    TextInputCapabilities capabilities;
    capabilities.committedText = FeatureSupport::Available;
    capabilities.composition = FeatureSupport::Available;
    capabilities.candidateWindowPosition = FeatureSupport::Available;

    capabilities.surroundingText = FeatureSupport::Available;

    // TODO: Show and hide the keyboard on request. A browser shows it only
    // within a tap, which setTextFieldAreas() handles.
    capabilities.softwareKeyboard = FeatureSupport::NotImplemented;
    capabilities.softwareKeyboardArea = FeatureSupport::NotImplemented;
    return capabilities;
}

void TextInputServiceEmscripten::update()
{
    takeEvents(focusedFieldID_);
    events_.clear();
    std::swap(events_, pendingEvents_);
}

std::span<const TextInputEvent> TextInputServiceEmscripten::getEvents() const noexcept
{
    return events_;
}

void TextInputServiceEmscripten::setFocusedField(const TextFieldState& field)
{
    POMDOG_ASSERT(field.id != 0);
    if (!initialized_) {
        return;
    }

    // NOTE: Place the element before focusing it, so that the candidate window
    // opens at the caret from the start.
    pomdog_text_input_set_caret(field.caretRect.x, field.caretRect.y, field.caretRect.width, field.caretRect.height);
    blurPending_ = false;

    if ((field.id != focusedFieldID_) || (field.purpose != purpose_)) {
        // NOTE: Moving the element to the field ends the composition of the
        // previous field. Take its events now, while they can still be
        // addressed to that field.
        pomdog_text_input_focus(static_cast<int>(field.purpose));
        takeEvents(focusedFieldID_);
        mirror_.reset({}, 0, 0);
        focusedFieldID_ = field.id;
        purpose_ = field.purpose;
    }

    // NOTE: Copy the field into the element when the field changed in another
    // way than the element reported, such as by a key or a click. A password
    // never leaves its field. The text that an input method composes stays.
    std::string_view text = field.surroundingText;
    if (field.purpose == TextInputPurpose::Password) {
        text = {};
    }
    const auto size = static_cast<u32>(text.size());
    const auto caret = std::min(field.caret, size);
    const auto anchor = std::min(field.anchor, size);
    if (!mirror_.isComposing() && !mirror_.matches(text, caret, anchor)) {
        mirror_.reset(text, caret, anchor);
        pomdog_text_input_set_text(text.data(), static_cast<int>(size), static_cast<int>(caret), static_cast<int>(anchor));
    }
}

void TextInputServiceEmscripten::clearFocusedField()
{
    if (!initialized_) {
        return;
    }
    if (focusedFieldID_ == 0) {
        if (blurPending_ && (pomdog_text_input_release() != 0)) {
            blurPending_ = false;
        }
        return;
    }

    // NOTE: End the composition now, but blur the element only once no tap
    // may still give the focus to another field, as when lifting the finger
    // turns a devgui drag field into a text field.
    pomdog_text_input_leave_field();
    pomdog_text_input_deactivate();
    takeEvents(focusedFieldID_);
    focusedFieldID_ = 0;
    mirror_.reset({}, 0, 0);
    blurPending_ = true;
}

void TextInputServiceEmscripten::setTextFieldAreas(std::span<const TextFieldArea> areas)
{
    if (!initialized_) {
        return;
    }

    // NOTE: A GUI passes the areas in every frame. Hand them to the element
    // only when they change.
    areaScratch_.clear();
    for (const auto& area : areas) {
        areaScratch_.push_back(area.rect.x);
        areaScratch_.push_back(area.rect.y);
        areaScratch_.push_back(area.rect.width);
        areaScratch_.push_back(area.rect.height);
        areaScratch_.push_back(static_cast<f32>(area.purpose));
    }
    if (areaScratch_ == areaData_) {
        return;
    }
    std::swap(areaScratch_, areaData_);
    pomdog_text_input_set_areas(areaData_.data(), static_cast<int>(areas.size()));
}

void TextInputServiceEmscripten::showSoftwareKeyboard()
{
}

void TextInputServiceEmscripten::hideSoftwareKeyboard()
{
}

SoftwareKeyboardState TextInputServiceEmscripten::getSoftwareKeyboardState() const noexcept
{
    return SoftwareKeyboardState{};
}

void TextInputServiceEmscripten::takeEvents(u64 fieldID)
{
    if (!initialized_) {
        return;
    }

    std::array<int, 4> fields = {};
    while (pomdog_text_input_peek(fields.data()) != 0) {
        const auto change = static_cast<ElementChange>(fields[0]);
        const auto caret = static_cast<u32>(fields[2]);
        const auto anchor = static_cast<u32>(fields[3]);

        // NOTE: The element writes a null terminator after the text, into the
        // byte that std::string keeps past its end.
        elementText_.resize(static_cast<std::size_t>(fields[1]));
        pomdog_text_input_pop(elementText_.data(), fields[1]);

        if (fieldID == 0) {
            continue;
        }
        mirrorEvents_.clear();
        switch (change) {
        case ElementChange::Edit:
            mirror_.applyEdit(elementText_, caret, anchor, fieldID, mirrorEvents_);
            break;
        case ElementChange::Composition:
            mirror_.applyComposition(elementText_, caret, anchor, fieldID, mirrorEvents_);
            break;
        case ElementChange::CompositionEnd:
            mirror_.endComposition(elementText_, caret, anchor, fieldID, mirrorEvents_);
            break;
        }

        for (auto& event : mirrorEvents_) {
            if (event.kind == TextInputEventKind::Commit) {
                std::erase_if(event.text, isControlByte);
                if (event.text.empty()) {
                    continue;
                }

                // NOTE: Join typed characters into one event, as other
                // services do.
                if (!pendingEvents_.empty()) {
                    auto& last = pendingEvents_.back();
                    if ((last.kind == TextInputEventKind::Commit) && (last.fieldID == fieldID)) {
                        last.text.append(event.text);
                        continue;
                    }
                }
            }
            pendingEvents_.push_back(std::move(event));
        }
    }
}

} // namespace pomdog::textinput::detail::emscripten

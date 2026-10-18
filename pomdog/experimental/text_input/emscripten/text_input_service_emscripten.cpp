// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/emscripten/text_input_service_emscripten.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten.h>

#include <array>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: The JavaScript below is part of a C macro. It uses double quotes for
// strings and no comments, because the preprocessor reads it as C tokens, and
// clang-format must not touch it. Event kinds match TextInputEventKind, and
// purposes match TextInputPurpose.
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

    var input = document.createElement("input");
    input.type = "text";
    input.autocomplete = "off";
    input.spellcheck = false;
    input.tabIndex = -1;
    input.setAttribute("autocorrect", "off");
    input.setAttribute("autocapitalize", "off");
    input.setAttribute("aria-hidden", "true");
    var style = input.style;
    style.position = "fixed";
    style.left = "0px";
    style.top = "0px";
    style.width = "1px";
    style.height = "1px";
    style.padding = "0";
    style.margin = "0";
    style.border = "0";
    style.outline = "none";
    style.opacity = "0";
    style.background = "transparent";
    style.color = "transparent";
    style.caretColor = "transparent";
    style.pointerEvents = "none";
    document.body.appendChild(input);

    var state = {
        canvas: canvas,
        input: input,
        queue: [],
        composing: false,
        active: false,
        caret: ""
    };
    var push = function(kind, text, caret, targetStart, targetEnd) {
        state.queue.push({
            kind: kind,
            text: text,
            caret: lengthBytesUTF8(text.substring(0, caret)),
            targetStart: lengthBytesUTF8(text.substring(0, targetStart)),
            targetEnd: lengthBytesUTF8(text.substring(0, targetEnd))
        });
    };

    input.addEventListener("compositionstart", function() {
        state.composing = true;
    });
    input.addEventListener("compositionupdate", function(e) {
        var text = e.data || "";
        push(1, text, text.length, 0, 0);
    });
    input.addEventListener("compositionend", function(e) {
        var text = e.data || "";
        state.composing = false;
        if (text.length > 0) {
            push(0, text, 0, 0, 0);
        }
        push(2, "", 0, 0, 0);
        input.value = "";
    });
    input.addEventListener("input", function(e) {
        if (state.composing || e.isComposing) {
            var start = input.selectionStart;
            var end = input.selectionEnd;
            if (start < end) {
                push(1, input.value, end, start, end);
            }
            else {
                push(1, input.value, end, 0, 0);
            }
            return;
        }
        if ((e.inputType !== "insertCompositionText") && (e.inputType !== "insertFromComposition") && (input.value.length > 0)) {
            push(0, input.value, 0, 0, 0);
        }
        input.value = "";
    });
    input.addEventListener("keydown", function(e) {
        if (e.key === "Tab") {
            e.preventDefault();
        }
    });

    state.onPointerDown = function() {
        if (state.active && (document.activeElement !== input)) {
            input.focus({preventScroll: true});
        }
    };
    canvas.addEventListener("pointerdown", state.onPointerDown, true);

    Module.pomdogTextInput = state;
    return 1;
});

EM_JS(void, pomdog_text_input_destroy, (), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    state.canvas.removeEventListener("pointerdown", state.onPointerDown, true);
    state.input.remove();
    delete Module.pomdogTextInput;
});

EM_JS(void, pomdog_text_input_focus, (int purpose), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    var input = state.input;
    var modes = ["text", "text", "numeric", "decimal", "email", "url", "search"];
    input.type = (purpose === 1) ? "password" : "text";
    input.inputMode = modes[purpose] || "text";
    input.enterKeyHint = (purpose === 6) ? "search" : "done";
    state.active = true;
    if (document.activeElement !== input) {
        input.focus({preventScroll: true});
    }
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
});

EM_JS(void, pomdog_text_input_set_caret, (float x, float y, float width, float height), {
    var state = Module.pomdogTextInput;
    if (!state) {
        return;
    }
    var rect = state.canvas.getBoundingClientRect();
    var left = rect.left + x;
    var top = rect.top + y;
    var lineHeight = Math.max(1, height);
    var key = left + "," + top + "," + lineHeight;
    if (key === state.caret) {
        return;
    }
    state.caret = key;
    var style = state.input.style;
    style.left = left + "px";
    style.top = top + "px";
    style.height = lineHeight + "px";
    style.lineHeight = lineHeight + "px";
    style.fontSize = Math.max(1, Math.floor(lineHeight * 0.8)) + "px";
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
    HEAP32[index + 3] = e.targetStart;
    HEAP32[index + 4] = e.targetEnd;
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

static_assert(static_cast<int>(TextInputEventKind::Commit) == 0);
static_assert(static_cast<int>(TextInputEventKind::CompositionUpdate) == 1);
static_assert(static_cast<int>(TextInputEventKind::CompositionEnd) == 2);
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

    // TODO: Mirror the surrounding text into the input element, which mobile
    // software keyboards need for deletion and predictions, and focus the
    // element within a tap for iOS Safari.
    capabilities.surroundingText = FeatureSupport::NotImplemented;
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

    if ((field.id == focusedFieldID_) && (field.purpose == purpose_)) {
        return;
    }
    if ((focusedFieldID_ != 0) && (field.id != focusedFieldID_)) {
        // NOTE: Blurring ends the composition of the previous field. Take its
        // events now, while they can still be addressed to that field.
        pomdog_text_input_blur();
        takeEvents(focusedFieldID_);
    }
    focusedFieldID_ = field.id;
    purpose_ = field.purpose;
    pomdog_text_input_focus(static_cast<int>(purpose_));
}

void TextInputServiceEmscripten::clearFocusedField()
{
    if (!initialized_ || (focusedFieldID_ == 0)) {
        return;
    }
    pomdog_text_input_blur();
    takeEvents(focusedFieldID_);
    focusedFieldID_ = 0;
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

    std::array<int, 5> fields = {};
    while (pomdog_text_input_peek(fields.data()) != 0) {
        TextInputEvent event;
        event.kind = static_cast<TextInputEventKind>(fields[0]);
        event.fieldID = fieldID;
        event.text.resize(static_cast<std::size_t>(fields[1]));
        event.caret = static_cast<u32>(fields[2]);
        event.targetStart = static_cast<u32>(fields[3]);
        event.targetEnd = static_cast<u32>(fields[4]);

        // NOTE: The element writes a null terminator after the text, into the
        // byte that std::string keeps past its end.
        pomdog_text_input_pop(event.text.data(), fields[1]);

        if (fieldID == 0) {
            continue;
        }
        if (event.kind == TextInputEventKind::Commit) {
            std::erase_if(event.text, isControlByte);
            if (event.text.empty()) {
                continue;
            }

            // NOTE: Join typed characters into one event, as other services do.
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

} // namespace pomdog::textinput::detail::emscripten

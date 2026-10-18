// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/emscripten/clipboard_service_emscripten.h"
#include "pomdog/experimental/clipboard/line_breaks.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten.h>

#include <array>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: The JavaScript below is part of a C macro. It uses double quotes for
// strings and no comments, because the preprocessor reads it as C tokens, and
// clang-format must not touch it. Event kinds match ClipboardEventKind, and
// statuses match ClipboardStatus. Request IDs pass as doubles, which hold
// integers up to 2^53 exactly.
//
// EM_JS defines variables that only the linker reads, without declarations.
POMDOG_EMCC_SUPPRESS_WARNING_PUSH
POMDOG_EMCC_SUPPRESS_WARNING("-Wmissing-variable-declarations")

// clang-format off
EM_JS_DEPS(pomdog_clipboard, "$UTF8ToString,$stringToUTF8,$lengthBytesUTF8");

EM_JS(int, pomdog_clipboard_create, (), {
    if (Module.pomdogClipboard) {
        return 0;
    }
    var state = {queue: []};
    state.onPaste = function(e) {
        var target = e.target;
        if (!target || !target.hasAttribute || !target.hasAttribute("data-pomdog-text-input")) {
            return;
        }
        e.preventDefault();
        var data = e.clipboardData;
        var hasText = !!data && (Array.prototype.indexOf.call(data.types, "text/plain") >= 0);
        state.queue.push({kind: 2, requestID: 0, status: hasText ? 0 : 1, text: hasText ? data.getData("text/plain") : ""});
    };
    document.addEventListener("paste", state.onPaste, true);
    Module.pomdogClipboard = state;
    return 1;
});

EM_JS(void, pomdog_clipboard_destroy, (), {
    var state = Module.pomdogClipboard;
    if (!state) {
        return;
    }
    document.removeEventListener("paste", state.onPaste, true);
    delete Module.pomdogClipboard;
});

EM_JS(int, pomdog_clipboard_get_support, (), {
    var clipboard = navigator.clipboard;
    var flags = 0;
    if (clipboard && clipboard.writeText) {
        flags |= 1;
    }
    if (clipboard && clipboard.readText) {
        flags |= 2;
    }
    if (window.isSecureContext) {
        flags |= 4;
    }
    return flags;
});

EM_JS(void, pomdog_clipboard_write_text, (double requestID, const char* text), {
    var state = Module.pomdogClipboard;
    var push = function(status) {
        state.queue.push({kind: 0, requestID: requestID, status: status, text: ""});
    };
    var clipboard = navigator.clipboard;
    if (!clipboard || !clipboard.writeText) {
        push(2);
        return;
    }
    clipboard.writeText(UTF8ToString(text)).then(function() {
        push(0);
    }, function(e) {
        push((e && (e.name === "NotAllowedError")) ? 3 : 4);
    });
});

EM_JS(void, pomdog_clipboard_read_text, (double requestID), {
    var state = Module.pomdogClipboard;
    var push = function(status, text) {
        state.queue.push({kind: 1, requestID: requestID, status: status, text: text});
    };
    var clipboard = navigator.clipboard;
    if (!clipboard || !clipboard.readText) {
        push(2, "");
        return;
    }
    clipboard.readText().then(function(text) {
        push(0, text);
    }, function(e) {
        push((e && (e.name === "NotAllowedError")) ? 3 : 4, "");
    });
});

EM_JS(int, pomdog_clipboard_peek, (int* fields, double* requestID), {
    var state = Module.pomdogClipboard;
    if (!state || (state.queue.length === 0)) {
        return 0;
    }
    var e = state.queue[0];
    var index = fields >> 2;
    HEAP32[index] = e.kind;
    HEAP32[index + 1] = e.status;
    HEAP32[index + 2] = lengthBytesUTF8(e.text);
    HEAPF64[requestID >> 3] = e.requestID;
    return 1;
});

EM_JS(void, pomdog_clipboard_pop, (char* buffer, int size), {
    var state = Module.pomdogClipboard;
    var e = state.queue.shift();
    if (size > 0) {
        stringToUTF8(e.text, buffer, size + 1);
    }
});
// clang-format on

POMDOG_EMCC_SUPPRESS_WARNING_POP

namespace pomdog::clipboard::detail::emscripten {
namespace {

static_assert(static_cast<int>(ClipboardEventKind::WriteCompleted) == 0);
static_assert(static_cast<int>(ClipboardEventKind::ReadCompleted) == 1);
static_assert(static_cast<int>(ClipboardEventKind::Paste) == 2);
static_assert(static_cast<int>(ClipboardStatus::Succeeded) == 0);
static_assert(static_cast<int>(ClipboardStatus::NoText) == 1);
static_assert(static_cast<int>(ClipboardStatus::Unsupported) == 2);
static_assert(static_cast<int>(ClipboardStatus::Denied) == 3);
static_assert(static_cast<int>(ClipboardStatus::Failed) == 4);

[[nodiscard]] FeatureSupport
toFeatureSupport(bool present, bool secure) noexcept
{
    if (present) {
        return FeatureSupport::Available;
    }

    // NOTE: Browsers hide navigator.clipboard on pages that are not secure,
    // such as plain HTTP from another host.
    if (!secure) {
        return FeatureSupport::Unavailable;
    }
    return FeatureSupport::Unsupported;
}

} // namespace

ClipboardServiceEmscripten::ClipboardServiceEmscripten() noexcept = default;

ClipboardServiceEmscripten::~ClipboardServiceEmscripten()
{
    if (initialized_) {
        pomdog_clipboard_destroy();
    }
}

std::unique_ptr<Error>
ClipboardServiceEmscripten::initialize() noexcept
{
    POMDOG_ASSERT(!initialized_);
    if (pomdog_clipboard_create() == 0) {
        return errors::make("another ClipboardServiceEmscripten already exists");
    }
    initialized_ = true;

    const auto support = pomdog_clipboard_get_support();
    const bool secure = (support & 4) != 0;
    capabilities_.writeText = toFeatureSupport((support & 1) != 0, secure);
    capabilities_.readText = toFeatureSupport((support & 2) != 0, secure);
    capabilities_.pasteEvents = FeatureSupport::Available;
    return nullptr;
}

ClipboardCapabilities ClipboardServiceEmscripten::getCapabilities() const noexcept
{
    return capabilities_;
}

void ClipboardServiceEmscripten::update()
{
    events_.clear();
    if (!initialized_) {
        return;
    }

    std::array<int, 3> fields = {};
    double requestID = 0.0;
    while (pomdog_clipboard_peek(fields.data(), &requestID) != 0) {
        std::string text;
        text.resize(static_cast<std::size_t>(fields[2]));

        // NOTE: The browser writes a null terminator after the text, into the
        // byte that std::string keeps past its end.
        pomdog_clipboard_pop(text.data(), fields[2]);

        ClipboardEvent event;
        event.kind = static_cast<ClipboardEventKind>(fields[0]);
        event.requestID = static_cast<u64>(requestID);
        event.status = static_cast<ClipboardStatus>(fields[1]);
        appendWithLF(text, event.text);
        events_.push_back(std::move(event));
    }
}

std::span<const ClipboardEvent> ClipboardServiceEmscripten::getEvents() const noexcept
{
    return events_;
}

u64 ClipboardServiceEmscripten::requestWriteText(std::string_view text)
{
    const auto requestID = nextRequestID_++;
    if (initialized_) {
        const std::string terminated{text};
        pomdog_clipboard_write_text(static_cast<double>(requestID), terminated.c_str());
    }
    return requestID;
}

u64 ClipboardServiceEmscripten::requestReadText()
{
    const auto requestID = nextRequestID_++;
    if (initialized_) {
        pomdog_clipboard_read_text(static_cast<double>(requestID));
    }
    return requestID;
}

} // namespace pomdog::clipboard::detail::emscripten

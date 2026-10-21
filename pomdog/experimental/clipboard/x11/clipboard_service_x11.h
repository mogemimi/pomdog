// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::clipboard::detail::x11 {

/// Implements `ClipboardService` with the CLIPBOARD selection of X11.
///
/// The service has its own connection to the X server and hidden windows, one
/// of which owns the text it writes, and handles their events in `update()`. While
/// it owns the selection, it answers the requests of other applications there,
/// so their pastes wait for the next frame. A read finishes in the `update()`
/// that receives the owner's answer. When the service is destroyed while it
/// owns the selection, it hands the text to the clipboard manager, if one runs,
/// so that the text outlives the application.
class ClipboardServiceX11 final : public ClipboardService {
public:
    ClipboardServiceX11() noexcept;

    ClipboardServiceX11(const ClipboardServiceX11&) = delete;
    ClipboardServiceX11& operator=(const ClipboardServiceX11&) = delete;

    ~ClipboardServiceX11() override;

    /// Connects to the X server that the DISPLAY environment variable names,
    /// as the game host does, and creates the window that owns the selection.
    [[nodiscard]] std::unique_ptr<Error>
    initialize() noexcept;

    [[nodiscard]] ClipboardCapabilities
    getCapabilities() const noexcept override;

    void
    update() override;

    [[nodiscard]] std::span<const ClipboardEvent>
    getEvents() const noexcept override;

    u64
    requestWriteText(std::string_view text) override;

    u64
    requestReadText() override;

private:
    struct Atoms final {
        ::Atom clipboard = 0;
        ::Atom targets = 0;
        ::Atom multiple = 0;
        ::Atom timestamp = 0;
        ::Atom utf8String = 0;
        ::Atom textPlainUTF8 = 0;
        ::Atom text = 0;
        ::Atom string = 0;
        ::Atom atom = 0;
        ::Atom atomPair = 0;
        ::Atom integer = 0;
        ::Atom incr = 0;
        ::Atom clipboardManager = 0;
        ::Atom saveTargets = 0;
        ::Atom transferProperty = 0;
        ::Atom timestampProperty = 0;
    };

    enum class TransferState : u8 {
        Idle,
        Converting,
        ReceivingIncrementally,
    };

    [[nodiscard]] ClipboardStatus
    writeText(std::string_view text);

    /// Handles the events that arrived on the connection, and fails a read
    /// whose owner stopped answering.
    void
    processEvents();

    void
    handleEvent(const ::XEvent& event);

    void
    answerSelectionRequest(const ::XSelectionRequestEvent& request);

    /// Stores the selection's text in property of requestor as target, and
    /// returns false for a target that the service does not offer.
    [[nodiscard]] bool
    convertSelection(::Window requestor, ::Atom target, ::Atom property);

    /// Converts each target that the MULTIPLE target lists in property of
    /// requestor.
    [[nodiscard]] bool
    convertMultiple(::Window requestor, ::Atom property);

    /// Asks the owner of the selection to convert it to target.
    void
    startConversion(::Atom target);

    void
    handleSelectionNotify(const ::XSelectionEvent& event);

    void
    handlePropertyNotify(const ::XPropertyEvent& event);

    /// Reads and deletes the transfer property, appending its bytes to the
    /// received data. Returns the number of bytes read, or nullopt when the
    /// property holds no text.
    [[nodiscard]] std::optional<std::size_t>
    receiveProperty();

    /// Ends the transfer and finishes every waiting read with its data.
    void
    finishTransfer();

    /// Finishes every waiting read with status and text.
    void
    finishReads(ClipboardStatus status, const std::string& text);

    /// Fails every waiting read and replaces the window that receives
    /// answers, so that an owner still answering the abandoned transfer cannot
    /// reach the next read.
    void
    abandonTransfer();

    /// Returns the time of the X server, which ICCCM asks clients to use
    /// instead of CurrentTime for selections.
    [[nodiscard]] ::Time
    getServerTime();

    /// Gives the text to the clipboard manager, waiting briefly for it to
    /// copy the text.
    void
    handOverToClipboardManager();

    void
    pushEvent(ClipboardEventKind kind, u64 requestID, ClipboardStatus status, const std::string& text);

private:
    ::Display* display_ = nullptr;

    /// The window that owns the selection.
    ::Window window_ = 0;

    /// The window that receives the answers to reads.
    ::Window readWindow_ = 0;

    Atoms atoms_;

    /// The largest property, in bytes, that one request can store.
    std::size_t maxPropertySize_ = 0;

    std::string ownedText_;
    ::Time ownershipTime_ = 0;
    bool ownsSelection_ = false;

    std::vector<u64> waitingReads_;
    std::string receivedData_;
    ::Atom requestedTarget_ = 0;
    ::Atom receivedType_ = 0;
    ::Time requestTime_ = 0;
    TransferState transferState_ = TransferState::Idle;
    std::chrono::steady_clock::time_point lastProgressTime_ = {};

    std::vector<ClipboardEvent> pendingEvents_;
    std::vector<ClipboardEvent> events_;
    u64 nextRequestID_ = 1;
};

} // namespace pomdog::clipboard::detail::x11

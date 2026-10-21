// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/x11/clipboard_service_x11.h"
#include "pomdog/experimental/clipboard/line_breaks.h"
#include "pomdog/experimental/clipboard/text_encoding.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <poll.h>

#include <array>
#include <cstring>
#include <limits>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard::detail::x11 {
namespace {

/// How long a read waits for the owner of the selection to answer, or to send
/// the next part of a transfer.
constexpr auto readTimeout = std::chrono::seconds(1);

/// How long the destroyed service waits for the clipboard manager to copy its
/// text.
constexpr auto handOverTimeout = std::chrono::seconds(1);

::Display* errorTrapDisplay = nullptr;
::XErrorHandler errorTrapPreviousHandler = nullptr;

int trapError(::Display* display, ::XErrorEvent* error)
{
    // NOTE: Xlib calls one handler for the errors of every connection.
    if (display == errorTrapDisplay) {
        return 0;
    }
    if (errorTrapPreviousHandler != nullptr) {
        return errorTrapPreviousHandler(display, error);
    }
    return 0;
}

/// Ignores the X errors of display until the end of the scope.
///
/// Xlib's default handler ends the process. Another application can close its
/// window at any time, so the requests for its window may fail.
class ScopedErrorTrap final {
public:
    explicit ScopedErrorTrap(::Display* display) noexcept
        : display_(display)
    {
        POMDOG_ASSERT(errorTrapDisplay == nullptr);
        errorTrapDisplay = display_;
        errorTrapPreviousHandler = ::XSetErrorHandler(&trapError);
    }

    ScopedErrorTrap(const ScopedErrorTrap&) = delete;
    ScopedErrorTrap& operator=(const ScopedErrorTrap&) = delete;

    ~ScopedErrorTrap()
    {
        // NOTE: An error arrives after the request that caused it, so wait for
        // the errors before restoring the handler.
        ::XSync(display_, False);
        ::XSetErrorHandler(errorTrapPreviousHandler);
        errorTrapDisplay = nullptr;
        errorTrapPreviousHandler = nullptr;
    }

private:
    ::Display* display_ = nullptr;
};

struct PropertyNotifyMatch final {
    ::Window window = 0;
    ::Atom atom = 0;
};

Bool isPropertyNotify([[maybe_unused]] ::Display* display, ::XEvent* event, ::XPointer argument)
{
    const auto match = reinterpret_cast<const PropertyNotifyMatch*>(argument);
    const bool matches = (event->type == PropertyNotify) &&
                         (event->xproperty.window == match->window) &&
                         (event->xproperty.atom == match->atom);
    return matches ? True : False;
}

struct Property final {
    ::Atom type = 0;
    int format = 0;
    std::string bytes;
};

/// Reads property of window, and deletes it when deleteProperty is True.
[[nodiscard]] std::optional<Property>
readProperty(::Display* display, ::Window window, ::Atom property, Bool deleteProperty)
{
    ::Atom type = None;
    int format = 0;
    unsigned long count = 0;
    unsigned long bytesAfter = 0;
    unsigned char* data = nullptr;
    const auto result = ::XGetWindowProperty(
        display, window, property, 0, std::numeric_limits<long>::max(), deleteProperty,
        AnyPropertyType, &type, &format, &count, &bytesAfter, &data);
    if (result != Success) {
        return std::nullopt;
    }

    Property taken;
    taken.type = type;
    taken.format = format;
    if (data != nullptr) {
        // NOTE: Xlib returns 32-bit items as longs.
        const std::size_t itemSize = (format == 32) ? sizeof(long) : ((format == 16) ? sizeof(short) : 1);
        taken.bytes.assign(reinterpret_cast<const char*>(data), count * itemSize);
        ::XFree(data);
    }
    return taken;
}

/// Creates a hidden window that reports changes to its properties.
[[nodiscard]] ::Window
createHiddenWindow(::Display* display) noexcept
{
    const auto window = ::XCreateSimpleWindow(display, ::XDefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    ::XSelectInput(display, window, PropertyChangeMask);
    return window;
}

} // namespace

ClipboardServiceX11::ClipboardServiceX11() noexcept = default;

ClipboardServiceX11::~ClipboardServiceX11()
{
    if (display_ == nullptr) {
        return;
    }
    if (window_ != 0) {
        handOverToClipboardManager();
        ::XDestroyWindow(display_, window_);
    }
    if (readWindow_ != 0) {
        ::XDestroyWindow(display_, readWindow_);
    }
    ::XCloseDisplay(display_);
}

std::unique_ptr<Error>
ClipboardServiceX11::initialize() noexcept
{
    POMDOG_ASSERT(display_ == nullptr);

    // NOTE: A connection of its own lets the service answer requests without
    // the host's event loop.
    display_ = ::XOpenDisplay(nullptr);
    if (display_ == nullptr) {
        return errors::make("XOpenDisplay() failed");
    }

    // NOTE: The windows stay hidden. One owns the selection, and the other
    // receives the answers to reads.
    window_ = createHiddenWindow(display_);
    readWindow_ = createHiddenWindow(display_);

    const auto intern = [this](const char* name) { return ::XInternAtom(display_, name, False); };
    atoms_.clipboard = intern("CLIPBOARD");
    atoms_.targets = intern("TARGETS");
    atoms_.multiple = intern("MULTIPLE");
    atoms_.timestamp = intern("TIMESTAMP");
    atoms_.utf8String = intern("UTF8_STRING");
    atoms_.textPlainUTF8 = intern("text/plain;charset=utf-8");
    atoms_.text = intern("TEXT");
    atoms_.string = intern("STRING");
    atoms_.atom = intern("ATOM");
    atoms_.atomPair = intern("ATOM_PAIR");
    atoms_.integer = intern("INTEGER");
    atoms_.incr = intern("INCR");
    atoms_.clipboardManager = intern("CLIPBOARD_MANAGER");
    atoms_.saveTargets = intern("SAVE_TARGETS");
    atoms_.transferProperty = intern("POMDOG_CLIPBOARD");
    atoms_.timestampProperty = intern("POMDOG_CLIPBOARD_TIMESTAMP");

    // NOTE: Request sizes count 4-byte units, and a ChangeProperty request
    // takes 24 bytes besides its data.
    auto requestUnits = ::XExtendedMaxRequestSize(display_);
    if (requestUnits == 0) {
        requestUnits = ::XMaxRequestSize(display_);
    }
    maxPropertySize_ = static_cast<std::size_t>(requestUnits) * 4 - 24;
    return nullptr;
}

ClipboardCapabilities ClipboardServiceX11::getCapabilities() const noexcept
{
    ClipboardCapabilities capabilities;
    capabilities.writeText = FeatureSupport::Available;
    capabilities.readText = FeatureSupport::Available;

    // NOTE: X11 has no paste event; the application reads the selection when
    // the user presses the paste shortcut.
    capabilities.pasteEvents = FeatureSupport::Unsupported;
    return capabilities;
}

void ClipboardServiceX11::update()
{
    processEvents();
    events_.clear();
    std::swap(events_, pendingEvents_);
}

std::span<const ClipboardEvent> ClipboardServiceX11::getEvents() const noexcept
{
    return events_;
}

u64 ClipboardServiceX11::requestWriteText(std::string_view text)
{
    const auto requestID = nextRequestID_++;
    pushEvent(ClipboardEventKind::WriteCompleted, requestID, writeText(text), {});
    return requestID;
}

u64 ClipboardServiceX11::requestReadText()
{
    const auto requestID = nextRequestID_++;

    // NOTE: Learn whether another application took the selection since the
    // last update.
    processEvents();

    if (ownsSelection_) {
        pushEvent(ClipboardEventKind::ReadCompleted, requestID, ClipboardStatus::Succeeded, ownedText_);
        return requestID;
    }
    if (::XGetSelectionOwner(display_, atoms_.clipboard) == None) {
        pushEvent(ClipboardEventKind::ReadCompleted, requestID, ClipboardStatus::NoText, {});
        return requestID;
    }

    // NOTE: Reads that start during a transfer share its result.
    waitingReads_.push_back(requestID);
    if (transferState_ == TransferState::Idle) {
        startConversion(atoms_.utf8String);
    }
    return requestID;
}

ClipboardStatus ClipboardServiceX11::writeText(std::string_view text)
{
    // TODO: Send text that does not fit in one request in parts (INCR).
    if (text.size() > maxPropertySize_) {
        return ClipboardStatus::Failed;
    }

    const auto time = getServerTime();
    ::XSetSelectionOwner(display_, atoms_.clipboard, window_, time);
    if (::XGetSelectionOwner(display_, atoms_.clipboard) != window_) {
        return ClipboardStatus::Failed;
    }
    ownedText_.assign(text);
    ownershipTime_ = time;
    ownsSelection_ = true;
    return ClipboardStatus::Succeeded;
}

void ClipboardServiceX11::processEvents()
{
    while (::XPending(display_) > 0) {
        ::XEvent event;
        ::XNextEvent(display_, &event);
        handleEvent(event);
    }

    // NOTE: An owner that stopped answering would otherwise hold every later
    // read.
    if ((transferState_ != TransferState::Idle) && ((std::chrono::steady_clock::now() - lastProgressTime_) > readTimeout)) {
        abandonTransfer();
    }
}

void ClipboardServiceX11::handleEvent(const ::XEvent& event)
{
    switch (event.type) {
    case SelectionRequest:
        answerSelectionRequest(event.xselectionrequest);
        break;
    case SelectionClear:
        // NOTE: Another application took the selection. A notice that was
        // queued before a later write took the selection back is stale, so
        // the server decides.
        if ((event.xselectionclear.window == window_) &&
            (event.xselectionclear.selection == atoms_.clipboard) &&
            (::XGetSelectionOwner(display_, atoms_.clipboard) != window_)) {
            ownsSelection_ = false;
            ownedText_.clear();
        }
        break;
    case SelectionNotify:
        handleSelectionNotify(event.xselection);
        break;
    case PropertyNotify:
        handlePropertyNotify(event.xproperty);
        break;
    default:
        break;
    }
}

void ClipboardServiceX11::answerSelectionRequest(const ::XSelectionRequestEvent& request)
{
    // NOTE: Clients that predate ICCCM 2.0 send no property and expect the
    // target as the property.
    const auto property = (request.property != None) ? request.property : request.target;

    // NOTE: A request made before the service took the selection is for the
    // previous owner.
    const bool current = (request.time == CurrentTime) || (request.time >= ownershipTime_);

    ::XEvent reply = {};
    reply.xselection.type = SelectionNotify;
    reply.xselection.display = display_;
    reply.xselection.requestor = request.requestor;
    reply.xselection.selection = request.selection;
    reply.xselection.target = request.target;
    reply.xselection.property = None;
    reply.xselection.time = request.time;

    ScopedErrorTrap trap{display_};
    if (ownsSelection_ && current && (request.owner == window_) && (request.selection == atoms_.clipboard)) {
        const bool converted = (request.target == atoms_.multiple)
                                   ? convertMultiple(request.requestor, property)
                                   : convertSelection(request.requestor, request.target, property);
        if (converted) {
            reply.xselection.property = property;
        }
    }
    ::XSendEvent(display_, request.requestor, False, NoEventMask, &reply);
}

bool ClipboardServiceX11::convertSelection(::Window requestor, ::Atom target, ::Atom property)
{
    if (target == atoms_.targets) {
        const std::array<::Atom, 6> targets = {
            atoms_.targets,
            atoms_.multiple,
            atoms_.timestamp,
            atoms_.utf8String,
            atoms_.textPlainUTF8,
            atoms_.text,
        };
        ::XChangeProperty(
            display_, requestor, property, atoms_.atom, 32, PropModeReplace,
            reinterpret_cast<const unsigned char*>(targets.data()), static_cast<int>(targets.size()));
        return true;
    }
    if (target == atoms_.timestamp) {
        // NOTE: Xlib passes 32-bit items as longs.
        const auto time = static_cast<long>(ownershipTime_);
        ::XChangeProperty(
            display_, requestor, property, atoms_.integer, 32, PropModeReplace,
            reinterpret_cast<const unsigned char*>(&time), 1);
        return true;
    }
    if ((target == atoms_.utf8String) || (target == atoms_.textPlainUTF8) || (target == atoms_.text)) {
        // NOTE: TEXT lets the owner choose the encoding.
        const auto type = (target == atoms_.text) ? atoms_.utf8String : target;
        ::XChangeProperty(
            display_, requestor, property, type, 8, PropModeReplace,
            reinterpret_cast<const unsigned char*>(ownedText_.data()), static_cast<int>(ownedText_.size()));
        return true;
    }
    return false;
}

bool ClipboardServiceX11::convertMultiple(::Window requestor, ::Atom property)
{
    // NOTE: The property lists pairs of a target and the property to store it
    // in. The owner replaces the property of each pair it cannot convert with
    // None.
    auto pairs = readProperty(display_, requestor, property, False);
    if (!pairs || (pairs->format != 32)) {
        return false;
    }
    std::vector<::Atom> atoms(pairs->bytes.size() / sizeof(::Atom));
    std::memcpy(atoms.data(), pairs->bytes.data(), atoms.size() * sizeof(::Atom));

    for (std::size_t i = 0; (i + 1) < atoms.size(); i += 2) {
        if ((atoms[i] == atoms_.multiple) || !convertSelection(requestor, atoms[i], atoms[i + 1])) {
            atoms[i + 1] = None;
        }
    }
    ::XChangeProperty(
        display_, requestor, property, atoms_.atomPair, 32, PropModeReplace,
        reinterpret_cast<const unsigned char*>(atoms.data()), static_cast<int>(atoms.size()));
    return true;
}

void ClipboardServiceX11::startConversion(::Atom target)
{
    requestedTarget_ = target;
    receivedType_ = None;
    receivedData_.clear();
    transferState_ = TransferState::Converting;
    lastProgressTime_ = std::chrono::steady_clock::now();

    requestTime_ = getServerTime();
    ::XDeleteProperty(display_, readWindow_, atoms_.transferProperty);
    ::XConvertSelection(display_, atoms_.clipboard, target, atoms_.transferProperty, readWindow_, requestTime_);
    ::XFlush(display_);
}

void ClipboardServiceX11::handleSelectionNotify(const ::XSelectionEvent& event)
{
    // NOTE: The answer carries the time of its request, which tells the answer
    // to the running conversion from a late answer to an abandoned one.
    if ((transferState_ != TransferState::Converting) ||
        (event.requestor != readWindow_) ||
        (event.selection != atoms_.clipboard) ||
        (event.target != requestedTarget_) ||
        (event.time != requestTime_) ||
        ((event.property != None) && (event.property != atoms_.transferProperty))) {
        return;
    }

    if (event.property == None) {
        // NOTE: Applications that predate UTF-8 offer only STRING.
        if (requestedTarget_ == atoms_.utf8String) {
            startConversion(atoms_.string);
        }
        else {
            finishReads(ClipboardStatus::NoText, {});
        }
        return;
    }

    auto property = readProperty(display_, readWindow_, atoms_.transferProperty, True);
    if (!property) {
        finishReads(ClipboardStatus::Failed, {});
        return;
    }
    if (property->type == atoms_.incr) {
        // NOTE: The owner sends large text in parts. Deleting the property,
        // which reading it did, asks for the first part.
        transferState_ = TransferState::ReceivingIncrementally;
        receivedType_ = requestedTarget_;
        lastProgressTime_ = std::chrono::steady_clock::now();
        return;
    }
    if (property->format != 8) {
        finishReads(ClipboardStatus::Failed, {});
        return;
    }
    receivedType_ = property->type;
    receivedData_ = std::move(property->bytes);
    finishTransfer();
}

void ClipboardServiceX11::handlePropertyNotify(const ::XPropertyEvent& event)
{
    if ((transferState_ != TransferState::ReceivingIncrementally) ||
        (event.window != readWindow_) ||
        (event.atom != atoms_.transferProperty) ||
        (event.state != PropertyNewValue)) {
        return;
    }

    auto property = readProperty(display_, readWindow_, atoms_.transferProperty, True);
    if (!property || ((property->format != 8) && !property->bytes.empty())) {
        abandonTransfer();
        return;
    }
    lastProgressTime_ = std::chrono::steady_clock::now();

    // NOTE: An empty part ends the transfer.
    if (property->bytes.empty()) {
        finishTransfer();
        return;
    }
    receivedType_ = property->type;
    receivedData_.append(property->bytes);
}

void ClipboardServiceX11::finishTransfer()
{
    std::string_view data = receivedData_;

    // NOTE: Some applications end their text with a null character, as C
    // strings do.
    if (const auto end = data.find('\0'); end != std::string_view::npos) {
        data = data.substr(0, end);
    }

    std::string decoded;
    if (receivedType_ == atoms_.string) {
        appendLatin1AsUTF8(data, decoded);
    }
    else if ((receivedType_ == atoms_.utf8String) || (receivedType_ == atoms_.textPlainUTF8)) {
        appendValidUTF8(data, decoded);
    }
    else {
        finishReads(ClipboardStatus::Failed, {});
        return;
    }

    std::string text;
    appendWithLF(decoded, text);
    finishReads(ClipboardStatus::Succeeded, text);
}

void ClipboardServiceX11::finishReads(ClipboardStatus status, const std::string& text)
{
    for (const auto requestID : waitingReads_) {
        pushEvent(ClipboardEventKind::ReadCompleted, requestID, status, text);
    }
    waitingReads_.clear();
    transferState_ = TransferState::Idle;
    requestedTarget_ = None;
    receivedType_ = None;

    // NOTE: Release the memory of a large transfer.
    receivedData_ = std::string{};
}

void ClipboardServiceX11::abandonTransfer()
{
    // NOTE: The owner may still answer or send parts to the window that it
    // knows. A new window keeps them away from later reads.
    ::XDestroyWindow(display_, readWindow_);
    readWindow_ = createHiddenWindow(display_);
    finishReads(ClipboardStatus::Failed, {});
}

::Time ClipboardServiceX11::getServerTime()
{
    // NOTE: Appending nothing to a property still makes the server send a
    // PropertyNotify event, which carries the server's time.
    ::XChangeProperty(display_, window_, atoms_.timestampProperty, atoms_.timestampProperty, 8, PropModeAppend, nullptr, 0);

    PropertyNotifyMatch match;
    match.window = window_;
    match.atom = atoms_.timestampProperty;
    ::XEvent event;
    ::XIfEvent(display_, &event, &isPropertyNotify, reinterpret_cast<::XPointer>(&match));
    return event.xproperty.time;
}

void ClipboardServiceX11::handOverToClipboardManager()
{
    // NOTE: Learn whether another application took the selection.
    processEvents();
    if (!ownsSelection_ || (::XGetSelectionOwner(display_, atoms_.clipboardManager) == None)) {
        return;
    }

    // NOTE: The manager asks the service for the targets and the text, which
    // the loop below answers, and then reports that it saved them.
    ::XConvertSelection(display_, atoms_.clipboardManager, atoms_.saveTargets, None, window_, getServerTime());
    ::XFlush(display_);

    const auto deadline = std::chrono::steady_clock::now() + handOverTimeout;
    for (;;) {
        while (::XPending(display_) > 0) {
            ::XEvent event;
            ::XNextEvent(display_, &event);
            if ((event.type == SelectionNotify) && (event.xselection.selection == atoms_.clipboardManager)) {
                return;
            }
            handleEvent(event);
        }

        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline) {
            return;
        }
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now).count();
        ::pollfd descriptor = {};
        descriptor.fd = ::XConnectionNumber(display_);
        descriptor.events = POLLIN;
        ::poll(&descriptor, 1, static_cast<int>(remaining) + 1);
    }
}

void ClipboardServiceX11::pushEvent(ClipboardEventKind kind, u64 requestID, ClipboardStatus status, const std::string& text)
{
    ClipboardEvent event;
    event.kind = kind;
    event.requestID = requestID;
    event.status = status;
    event.text = text;
    pendingEvents_.push_back(std::move(event));
}

} // namespace pomdog::clipboard::detail::x11

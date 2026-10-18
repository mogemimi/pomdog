// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/win32/text_input_service_win32.h"
#include "pomdog/application/display_metrics.h"
#include "pomdog/application/game_window.h"
#include "pomdog/experimental/text_input/utf16_conversion.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <imm.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail::win32 {
namespace {

// NOTE: Window properties hold the service and the procedure it replaced, because
// the application library already uses GWLP_USERDATA.
constexpr const wchar_t* servicePropertyName = L"pomdog.textinput.service";
constexpr const wchar_t* previousProcedurePropertyName = L"pomdog.textinput.previous_procedure";

[[nodiscard]] bool
isHighSurrogate(char16_t c) noexcept
{
    return (c >= 0xD800) && (c <= 0xDBFF);
}

[[nodiscard]] bool
isLowSurrogate(char16_t c) noexcept
{
    return (c >= 0xDC00) && (c <= 0xDFFF);
}

[[nodiscard]] bool
isControlCharacter(char16_t c) noexcept
{
    return (c < 0x20) || (c == 0x7F);
}

[[nodiscard]] bool
usesInputMethod(TextInputPurpose purpose) noexcept
{
    switch (purpose) {
    case TextInputPurpose::Password:
    case TextInputPurpose::Integer:
    case TextInputPurpose::Number:
        return false;
    case TextInputPurpose::Text:
    case TextInputPurpose::Email:
    case TextInputPurpose::URL:
    case TextInputPurpose::Search:
        break;
    }
    return true;
}

[[nodiscard]] WNDPROC
getPreviousProcedure(HWND windowHandle) noexcept
{
    return reinterpret_cast<WNDPROC>(reinterpret_cast<LONG_PTR>(::GetPropW(windowHandle, previousProcedurePropertyName)));
}

/// Reads the composition string index, such as GCS_COMPSTR, into result.
void readCompositionString(HIMC context, DWORD index, std::u16string& result)
{
    result.clear();
    const auto bytes = ::ImmGetCompositionStringW(context, index, nullptr, 0);
    if (bytes <= 0) {
        return;
    }
    result.resize(static_cast<std::size_t>(bytes) / sizeof(char16_t));
    [[maybe_unused]] const auto copied = ::ImmGetCompositionStringW(context, index, result.data(), static_cast<DWORD>(bytes));
}

/// Reads the attributes of the composition string, one byte per UTF-16 code unit.
void readCompositionAttributes(HIMC context, std::vector<u8>& result)
{
    result.clear();
    const auto bytes = ::ImmGetCompositionStringW(context, GCS_COMPATTR, nullptr, 0);
    if (bytes <= 0) {
        return;
    }
    result.resize(static_cast<std::size_t>(bytes));
    [[maybe_unused]] const auto copied = ::ImmGetCompositionStringW(context, GCS_COMPATTR, result.data(), static_cast<DWORD>(bytes));
}

} // namespace

TextInputServiceWin32::TextInputServiceWin32() noexcept = default;

TextInputServiceWin32::~TextInputServiceWin32()
{
    detach();
}

std::unique_ptr<Error>
TextInputServiceWin32::initialize(const std::shared_ptr<GameWindow>& window, HWND windowHandle) noexcept
{
    POMDOG_ASSERT(window != nullptr);
    POMDOG_ASSERT(windowHandle_ == nullptr);

    if (windowHandle == nullptr) {
        return errors::make("the window handle is null");
    }

    // NOTE: Every service shares the procedure and the property names, so a
    // second one on the same window would forward messages to itself.
    if ((::GetPropW(windowHandle, servicePropertyName) != nullptr) ||
        (::GetPropW(windowHandle, previousProcedurePropertyName) != nullptr)) {
        return errors::make("another TextInputServiceWin32 already subclasses the window");
    }

    // NOTE: Store the previous procedure before replacing it, so that the new
    // procedure can forward the first message it receives.
    const auto previous = ::GetWindowLongPtrW(windowHandle, GWLP_WNDPROC);
    if (previous == 0) {
        return errors::make("GetWindowLongPtrW() failed");
    }
    if (::SetPropW(windowHandle, previousProcedurePropertyName, reinterpret_cast<HANDLE>(previous)) == FALSE) {
        return errors::make("SetPropW() failed");
    }
    if (::SetPropW(windowHandle, servicePropertyName, static_cast<HANDLE>(this)) == FALSE) {
        ::RemovePropW(windowHandle, previousProcedurePropertyName);
        return errors::make("SetPropW() failed");
    }

    ::SetLastError(0);
    const auto replaced = ::SetWindowLongPtrW(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&TextInputServiceWin32::windowProcedure));
    if ((replaced == 0) && (::GetLastError() != 0)) {
        ::RemovePropW(windowHandle, servicePropertyName);
        ::RemovePropW(windowHandle, previousProcedurePropertyName);
        return errors::make("SetWindowLongPtrW() failed");
    }

    window_ = window;
    windowHandle_ = windowHandle;
    previousProcedure_ = reinterpret_cast<WNDPROC>(previous);

    // NOTE: Keys reach the game until a text field takes focus.
    setInputMethodEnabled(false);
    return nullptr;
}

TextInputCapabilities TextInputServiceWin32::getCapabilities() const noexcept
{
    TextInputCapabilities capabilities;
    capabilities.committedText = FeatureSupport::Available;
    capabilities.composition = FeatureSupport::Available;
    capabilities.candidateWindowPosition = FeatureSupport::Available;

    // TODO: Answer IMR_RECONVERTSTRING and IMR_DOCUMENTFEED from the surrounding
    // text, and show the touch keyboard with the text services framework (TSF).
    capabilities.surroundingText = FeatureSupport::NotImplemented;
    capabilities.softwareKeyboard = FeatureSupport::NotImplemented;
    capabilities.softwareKeyboardArea = FeatureSupport::NotImplemented;
    return capabilities;
}

void TextInputServiceWin32::update()
{
    events_.clear();
    std::swap(events_, pendingEvents_);
}

std::span<const TextInputEvent> TextInputServiceWin32::getEvents() const noexcept
{
    return events_;
}

void TextInputServiceWin32::setFocusedField(const TextFieldState& field)
{
    POMDOG_ASSERT(field.id != 0);
    if (windowHandle_ == nullptr) {
        return;
    }

    if (field.id != focusedFieldID_) {
        // NOTE: Cancel while the previous field still has focus, so that the
        // messages the input method sends in response carry its identifier.
        cancelComposition();
        pendingHighSurrogate_ = 0;
        focusedFieldID_ = field.id;
    }

    purpose_ = field.purpose;
    const bool enabled = usesInputMethod(purpose_);
    if (!enabled) {
        cancelComposition();
    }
    const bool wasEnabled = inputMethodEnabled_;
    setInputMethodEnabled(enabled);

    const bool caretMoved = (field.caretRect.x != caretRect_.x) ||
                            (field.caretRect.y != caretRect_.y) ||
                            (field.caretRect.width != caretRect_.width) ||
                            (field.caretRect.height != caretRect_.height);
    caretRect_ = field.caretRect;
    if (caretMoved || (enabled && !wasEnabled)) {
        applyCaretRect();
    }
}

void TextInputServiceWin32::clearFocusedField()
{
    if (focusedFieldID_ == 0) {
        return;
    }
    cancelComposition();
    focusedFieldID_ = 0;
    pendingHighSurrogate_ = 0;
    setInputMethodEnabled(false);
}

void TextInputServiceWin32::showSoftwareKeyboard()
{
}

void TextInputServiceWin32::hideSoftwareKeyboard()
{
}

SoftwareKeyboardState TextInputServiceWin32::getSoftwareKeyboardState() const noexcept
{
    return SoftwareKeyboardState{};
}

LRESULT CALLBACK
TextInputServiceWin32::windowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    const auto previous = getPreviousProcedure(windowHandle);
    if (auto service = static_cast<TextInputServiceWin32*>(::GetPropW(windowHandle, servicePropertyName)); service != nullptr) {
        if (const auto result = service->handleMessage(message, wParam, lParam); result) {
            return *result;
        }
    }
    if (message == WM_NCDESTROY) {
        // NOTE: A service that could not restore the procedure left the
        // property for forwarding. The window is going away now.
        ::RemovePropW(windowHandle, previousProcedurePropertyName);
    }
    if (previous == nullptr) {
        return ::DefWindowProcW(windowHandle, message, wParam, lParam);
    }
    return ::CallWindowProcW(previous, windowHandle, message, wParam, lParam);
}

std::optional<LRESULT>
TextInputServiceWin32::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_IME_SETCONTEXT:
        // NOTE: The GUI draws the composition, so the input method must not show
        // its own composition window. Its candidate window still shows.
        if (wParam != FALSE) {
            lParam &= ~static_cast<LPARAM>(ISC_SHOWUICOMPOSITIONWINDOW);
        }
        return ::DefWindowProcW(windowHandle_, message, wParam, lParam);
    case WM_IME_STARTCOMPOSITION:
        applyCaretRect();
        return 0;
    case WM_IME_COMPOSITION:
        // NOTE: DefWindowProc would turn the result into WM_IME_CHAR and WM_CHAR
        // messages, which would deliver the text twice.
        readComposition(lParam);
        return 0;
    case WM_IME_ENDCOMPOSITION:
        if (composing_) {
            composing_ = false;
            TextInputEvent event;
            event.kind = TextInputEventKind::CompositionEnd;
            event.fieldID = focusedFieldID_;
            pushEvent(std::move(event));
        }
        return 0;
    case WM_IME_CHAR:
        return 0;
    case WM_IME_REQUEST:
        if ((wParam == IMR_QUERYCHARPOSITION) && (focusedFieldID_ != 0)) {
            return queryCharPosition(lParam);
        }
        break;
    case WM_CHAR:
        // NOTE: The application library still receives the character, so that
        // `Keyboard::getTextInput()` keeps working.
        handleCharacter(static_cast<char16_t>(wParam));
        break;
    case WM_NCDESTROY:
        detach();
        break;
    default:
        break;
    }
    return std::nullopt;
}

void TextInputServiceWin32::readComposition(LPARAM flags)
{
    if (focusedFieldID_ == 0) {
        return;
    }
    const auto context = ::ImmGetContext(windowHandle_);
    if (context == nullptr) {
        return;
    }

    if ((flags & GCS_RESULTSTR) != 0) {
        readCompositionString(context, GCS_RESULTSTR, compositionScratch_);

        // NOTE: Remove control characters one code unit at a time; neither half
        // of a surrogate pair is one.
        std::erase_if(compositionScratch_, isControlCharacter);
        if (!compositionScratch_.empty()) {
            TextInputEvent event;
            event.kind = TextInputEventKind::Commit;
            event.fieldID = focusedFieldID_;
            appendUTF16AsUTF8(compositionScratch_, event.text);
            pushEvent(std::move(event));
        }
    }

    constexpr LPARAM compositionFlags = GCS_COMPSTR |
                                        GCS_COMPATTR |
                                        GCS_COMPCLAUSE |
                                        GCS_CURSORPOS |
                                        GCS_DELTASTART;
    constexpr LPARAM allCompositionFlags = compositionFlags |
                                           GCS_COMPREADSTR |
                                           GCS_COMPREADATTR |
                                           GCS_COMPREADCLAUSE |
                                           GCS_RESULTSTR |
                                           GCS_RESULTCLAUSE |
                                           GCS_RESULTREADSTR |
                                           GCS_RESULTREADCLAUSE;
    if ((flags & compositionFlags) != 0) {
        // NOTE: The flags describe what changed, not all available data. A
        // caret or target-clause update can arrive without GCS_COMPSTR, so read
        // the current text and metadata together for each composition update.
        readCompositionString(context, GCS_COMPSTR, compositionScratch_);
        const std::u16string_view text = compositionScratch_;

        auto cursor = text.size();
        const auto position = ::ImmGetCompositionStringW(context, GCS_CURSORPOS, nullptr, 0);
        if (position >= 0) {
            cursor = std::min(static_cast<std::size_t>(position), text.size());
        }

        // NOTE: The target is the clause the user is converting, which input
        // methods mark with one of two attributes.
        std::size_t targetStart = 0;
        std::size_t targetEnd = 0;
        readCompositionAttributes(context, attributeScratch_);
        if (!attributeScratch_.empty()) {
            const auto isTarget = [](u8 attribute) {
                return (attribute == ATTR_TARGET_CONVERTED) || (attribute == ATTR_TARGET_NOTCONVERTED);
            };
            const auto first = std::find_if(attributeScratch_.begin(), attributeScratch_.end(), isTarget);
            const auto last = std::find_if_not(first, attributeScratch_.end(), isTarget);
            targetStart = std::min(static_cast<std::size_t>(first - attributeScratch_.begin()), text.size());
            targetEnd = std::min(static_cast<std::size_t>(last - attributeScratch_.begin()), text.size());
        }

        TextInputEvent event;
        event.kind = TextInputEventKind::CompositionUpdate;
        event.fieldID = focusedFieldID_;
        appendUTF16AsUTF8(text, event.text);
        event.caret = toUTF8Offset(text, cursor);
        event.targetStart = toUTF8Offset(text, targetStart);
        event.targetEnd = toUTF8Offset(text, targetEnd);
        composing_ = true;
        pushEvent(std::move(event));
    }
    else if (((flags & allCompositionFlags) == 0) && composing_) {
        // NOTE: A message with no GCS flags cancels the current composition.
        // Reading-string and result metadata updates must not clear it.
        TextInputEvent event;
        event.kind = TextInputEventKind::CompositionUpdate;
        event.fieldID = focusedFieldID_;
        pushEvent(std::move(event));
    }

    ::ImmReleaseContext(windowHandle_, context);
}

void TextInputServiceWin32::handleCharacter(char16_t c)
{
    if (focusedFieldID_ == 0) {
        pendingHighSurrogate_ = 0;
        return;
    }
    if (isHighSurrogate(c)) {
        pendingHighSurrogate_ = c;
        return;
    }

    std::array<char16_t, 2> units = {{pendingHighSurrogate_, c}};
    std::u16string_view text{units.data(), units.size()};
    if ((pendingHighSurrogate_ == 0) || !isLowSurrogate(c)) {
        // NOTE: A lone high surrogate is dropped, and a lone low surrogate
        // becomes U+FFFD.
        text = text.substr(1);
    }
    pendingHighSurrogate_ = 0;

    if (isControlCharacter(c)) {
        return;
    }
    TextInputEvent event;
    event.kind = TextInputEventKind::Commit;
    event.fieldID = focusedFieldID_;
    appendUTF16AsUTF8(text, event.text);
    pushEvent(std::move(event));
}

LRESULT TextInputServiceWin32::queryCharPosition(LPARAM lParam) noexcept
{
    auto position = reinterpret_cast<IMECHARPOSITION*>(lParam);
    if (position == nullptr) {
        return FALSE;
    }

    // NOTE: The service knows only the caret area, which it reports for every
    // character of the composition.
    const auto caret = getCaretClientRect();
    POINT caretTopLeft = {caret.left, caret.top};
    ::ClientToScreen(windowHandle_, &caretTopLeft);

    RECT client = {0, 0, 0, 0};
    ::GetClientRect(windowHandle_, &client);
    POINT clientTopLeft = {client.left, client.top};
    POINT clientBottomRight = {client.right, client.bottom};
    ::ClientToScreen(windowHandle_, &clientTopLeft);
    ::ClientToScreen(windowHandle_, &clientBottomRight);

    position->pt = caretTopLeft;
    position->cLineHeight = static_cast<UINT>(std::max<LONG>(caret.bottom - caret.top, 0));
    position->rcDocument = RECT{clientTopLeft.x, clientTopLeft.y, clientBottomRight.x, clientBottomRight.y};
    return TRUE;
}

void TextInputServiceWin32::pushEvent(TextInputEvent&& event)
{
    // NOTE: Typing produces one character at a time. Join them into one event.
    if ((event.kind == TextInputEventKind::Commit) && !pendingEvents_.empty()) {
        auto& last = pendingEvents_.back();
        if ((last.kind == TextInputEventKind::Commit) && (last.fieldID == event.fieldID)) {
            last.text.append(event.text);
            return;
        }
    }
    pendingEvents_.push_back(std::move(event));
}

void TextInputServiceWin32::cancelComposition() noexcept
{
    if (!composing_ || (windowHandle_ == nullptr)) {
        return;
    }
    if (const auto context = ::ImmGetContext(windowHandle_); context != nullptr) {
        ::ImmNotifyIME(context, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
        ::ImmReleaseContext(windowHandle_, context);
    }

    // NOTE: The input method usually ends the composition before
    // ImmNotifyIME() returns. End it here when it did not.
    if (composing_) {
        composing_ = false;
        TextInputEvent event;
        event.kind = TextInputEventKind::CompositionEnd;
        event.fieldID = focusedFieldID_;
        pushEvent(std::move(event));
    }
}

void TextInputServiceWin32::setInputMethodEnabled(bool enabled) noexcept
{
    if ((windowHandle_ == nullptr) || (enabled == inputMethodEnabled_)) {
        return;
    }
    inputMethodEnabled_ = enabled;

    // NOTE: Without an input context, the input method stays off and keys
    // arrive as WM_KEYDOWN and WM_CHAR.
    ::ImmAssociateContextEx(windowHandle_, nullptr, enabled ? IACE_DEFAULT : 0);
}

void TextInputServiceWin32::applyCaretRect() noexcept
{
    if ((windowHandle_ == nullptr) || (focusedFieldID_ == 0) || !inputMethodEnabled_) {
        return;
    }
    const auto context = ::ImmGetContext(windowHandle_);
    if (context == nullptr) {
        return;
    }
    const auto caret = getCaretClientRect();

    // NOTE: Some input methods place their candidate window from the
    // composition window's position, others from the candidate form.
    COMPOSITIONFORM composition = {};
    composition.dwStyle = CFS_POINT;
    composition.ptCurrentPos = POINT{caret.left, caret.top};
    ::ImmSetCompositionWindow(context, &composition);

    CANDIDATEFORM candidate = {};
    candidate.dwIndex = 0;
    candidate.dwStyle = CFS_EXCLUDE;
    candidate.ptCurrentPos = POINT{caret.left, caret.top};
    candidate.rcArea = caret;
    ::ImmSetCandidateWindow(context, &candidate);

    ::ImmReleaseContext(windowHandle_, context);
}

RECT TextInputServiceWin32::getCaretClientRect() const noexcept
{
    POMDOG_ASSERT(window_ != nullptr);
    const auto pixelRatio = window_->getDisplayMetrics().pixelRatio;
    const auto toPixels = [pixelRatio](f32 value) -> LONG {
        return static_cast<LONG>(std::lround(value * pixelRatio));
    };
    return RECT{
        toPixels(caretRect_.x),
        toPixels(caretRect_.y),
        toPixels(caretRect_.x + caretRect_.width),
        toPixels(caretRect_.y + caretRect_.height),
    };
}

void TextInputServiceWin32::detach() noexcept
{
    if (windowHandle_ == nullptr) {
        return;
    }
    cancelComposition();

    // NOTE: Give the window back its input method as it had before.
    setInputMethodEnabled(true);

    ::RemovePropW(windowHandle_, servicePropertyName);
    const auto current = ::GetWindowLongPtrW(windowHandle_, GWLP_WNDPROC);
    if (current == reinterpret_cast<LONG_PTR>(&TextInputServiceWin32::windowProcedure)) {
        ::SetWindowLongPtrW(windowHandle_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(previousProcedure_));
        ::RemovePropW(windowHandle_, previousProcedurePropertyName);
    }
    // NOTE: Otherwise another procedure replaced this one, which therefore
    // stays and keeps forwarding every message to the previous procedure.

    windowHandle_ = nullptr;
    previousProcedure_ = nullptr;
    focusedFieldID_ = 0;
}

} // namespace pomdog::textinput::detail::win32

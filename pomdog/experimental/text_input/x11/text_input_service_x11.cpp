// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/x11/text_input_service_x11.h"
#include "pomdog/application/display_metrics.h"
#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/experimental/text_input/utf32_conversion.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xutil.h>
#include <langinfo.h>
#include <locale.h>

#include <algorithm>
#include <clocale>
#include <cmath>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail::x11 {
namespace {

[[nodiscard]] bool
isControlByte(char c) noexcept
{
    const auto byte = static_cast<u8>(c);
    return (byte < 0x20) || (byte == 0x7F);
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

/// Returns true when codeset, such as "UTF-8" or "utf8", names UTF-8.
[[nodiscard]] bool
isUTF8Codeset(const char* codeset) noexcept
{
    if (codeset == nullptr) {
        return false;
    }
    std::string name;
    for (const auto c : std::string_view{codeset}) {
        if (c != '-') {
            name.push_back(static_cast<char>(((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 'a') : c));
        }
    }
    return name == "utf8";
}

/// Returns the styles the input method supports, or an empty list.
[[nodiscard]] std::vector<::XIMStyle>
getSupportedStyles(::XIM inputMethod) noexcept
{
    std::vector<::XIMStyle> result;
    ::XIMStyles* styles = nullptr;
    if ((::XGetIMValues(inputMethod, XNQueryInputStyle, &styles, nullptr) != nullptr) || (styles == nullptr)) {
        return result;
    }
    if (styles->supported_styles != nullptr) {
        const std::span<const ::XIMStyle> supported{styles->supported_styles, styles->count_styles};
        result.assign(supported.begin(), supported.end());
    }
    ::XFree(styles);
    return result;
}

/// Creates a font set for the current locale, or returns null when the X
/// server has no font for it.
[[nodiscard]] ::XFontSet
createFontSet(::Display* display) noexcept
{
    char** missingCharsets = nullptr;
    int missingCount = 0;
    char* defaultString = nullptr;

    // NOTE: Input methods such as fcitx and ibus draw with their own fonts. The
    // last pattern matches any font, so that a server with few core fonts,
    // such as XWayland, still yields a font set.
    auto fontSet = ::XCreateFontSet(display, "-*-*-medium-r-normal--*-*-*-*-*-*-*-*,*", &missingCharsets, &missingCount, &defaultString);
    if (missingCharsets != nullptr) {
        ::XFreeStringList(missingCharsets);
    }
    return fontSet;
}

[[nodiscard]] bool
isConvertedFeedback(::XIMFeedback feedback) noexcept
{
    // NOTE: Input methods mark the clause being converted with reverse video
    // or a highlight, and the rest with an underline.
    return (feedback & (XIMReverse | XIMHighlight)) != 0;
}

/// Locks the display and gives the calling thread locale until the end of the
/// scope, for Xlib calls that can run the preedit callbacks.
class InputMethodScope final {
public:
    InputMethodScope(::Display* display, ::locale_t locale) noexcept
        : display_(display)
    {
        ::XLockDisplay(display_);
        if (locale != ::locale_t{}) {
            previousLocale_ = ::uselocale(locale);
        }
    }

    InputMethodScope(const InputMethodScope&) = delete;
    InputMethodScope& operator=(const InputMethodScope&) = delete;

    ~InputMethodScope()
    {
        if (previousLocale_ != ::locale_t{}) {
            ::uselocale(previousLocale_);
        }
        ::XUnlockDisplay(display_);
    }

private:
    ::Display* display_ = nullptr;
    ::locale_t previousLocale_ = {};
};

} // namespace

TextInputServiceX11::TextInputServiceX11() noexcept = default;

TextInputServiceX11::~TextInputServiceX11()
{
    if (window_ != nullptr) {
        window_->setEventFilter({});
    }
    if (display_ == nullptr) {
        return;
    }

    {
        InputMethodScope scope{display_, inputLocale_};
        if (inputContext_ != nullptr) {
            if (inputContextFocused_) {
                ::XUnsetICFocus(inputContext_);
                if (const auto windowContext = window_->getInputContext(); windowContext != nullptr) {
                    ::XSetICFocus(windowContext);
                }
            }
            ::XDestroyIC(inputContext_);
        }
        if (inputMethod_ != nullptr) {
            ::XCloseIM(inputMethod_);
        }
        if (fontSet_ != nullptr) {
            ::XFreeFontSet(display_, fontSet_);
        }
    }
    if (inputLocale_ != ::locale_t{}) {
        ::freelocale(inputLocale_);
    }
}

std::unique_ptr<Error>
TextInputServiceX11::initialize(const std::shared_ptr<pomdog::detail::x11::GameWindowX11>& window) noexcept
{
    POMDOG_ASSERT(window != nullptr);
    POMDOG_ASSERT(display_ == nullptr);

    // NOTE: Every service filters the events of the window, so a second one
    // would take the keys of the first.
    if (window->hasEventFilter()) {
        return errors::make("another service already filters the events of the window");
    }

    window_ = window;
    display_ = window->getNativeDisplay();
    windowHandle_ = window->getNativeWindow();

    ::XLockDisplay(display_);
    bool utf8 = false;
    openInputMethod(utf8);
    const bool created = (inputMethod_ != nullptr) && createInputContext(utf8);
    ::XUnlockDisplay(display_);

    if (inputMethod_ == nullptr) {
        return errors::make("XOpenIM() failed");
    }
    if (!created) {
        return errors::make("XCreateIC() failed for every input style");
    }

    window_->setEventFilter([this](::XEvent& event) { return filterEvent(event); });
    return nullptr;
}

void TextInputServiceX11::openInputMethod(bool& utf8) noexcept
{
    // NOTE: XOpenIM reaches the input method that XMODIFIERS names only in the
    // environment's locale and with its modifiers. Pomdog leaves the process in
    // its own locale, so the locale changes only while the input method opens.
    // The input method keeps the locale it opened with.
    std::string previousLocale;
    if (const auto current = std::setlocale(LC_CTYPE, nullptr); current != nullptr) {
        previousLocale = current;
    }
    if (std::setlocale(LC_CTYPE, "") != nullptr) {
        utf8 = isUTF8Codeset(::nl_langinfo(CODESET));
        if (::XSupportsLocale() != False) {
            ::XSetLocaleModifiers("");
            inputMethod_ = ::XOpenIM(display_, nullptr, nullptr, nullptr);
            if (inputMethod_ != nullptr) {
                // NOTE: Xlib requires a font set for the over-the-spot style. A font
                // set belongs to the locale it was created in, so it is created
                // with the input method.
                fontSet_ = createFontSet(display_);
                inputLocale_ = ::newlocale(LC_CTYPE_MASK, "", ::locale_t{});
            }
        }
    }
    if (!previousLocale.empty()) {
        std::setlocale(LC_CTYPE, previousLocale.c_str());
    }

    if (inputMethod_ == nullptr) {
        // NOTE: Without the environment's locale, Xlib's own input method still
        // turns keys into committed text.
        utf8 = false;
        inputMethod_ = ::XOpenIM(display_, nullptr, nullptr, nullptr);
    }
}

bool TextInputServiceX11::createInputContext(bool utf8) noexcept
{
    POMDOG_ASSERT(inputMethod_ != nullptr);
    const auto supported = getSupportedStyles(inputMethod_);
    const auto supports = [&](::XIMStyle style) {
        return std::find(supported.begin(), supported.end(), style) != supported.end();
    };

    // NOTE: The preedit callbacks receive text in the locale of the input
    // method, which the service reads as UTF-8 only.
    constexpr ::XIMStyle onTheSpot = XIMPreeditCallbacks | XIMStatusNothing;
    if (utf8 && supports(onTheSpot)) {
        const auto self = reinterpret_cast<::XPointer>(this);
        preeditStartCallback_.client_data = self;
        preeditStartCallback_.callback = &TextInputServiceX11::onPreeditStart;
        preeditDoneCallback_.client_data = self;
        preeditDoneCallback_.callback = &TextInputServiceX11::onPreeditDone;
        preeditDrawCallback_.client_data = self;
        preeditDrawCallback_.callback = &TextInputServiceX11::onPreeditDraw;
        preeditCaretCallback_.client_data = self;
        preeditCaretCallback_.callback = &TextInputServiceX11::onPreeditCaret;

        auto attributes = ::XVaCreateNestedList(
            0,
            XNPreeditStartCallback, &preeditStartCallback_,
            XNPreeditDoneCallback, &preeditDoneCallback_,
            XNPreeditDrawCallback, &preeditDrawCallback_,
            XNPreeditCaretCallback, &preeditCaretCallback_,
            nullptr);
        inputContext_ = ::XCreateIC(
            inputMethod_,
            XNInputStyle, onTheSpot,
            XNClientWindow, windowHandle_,
            XNFocusWindow, windowHandle_,
            XNPreeditAttributes, attributes,
            nullptr);
        ::XFree(attributes);
        if (inputContext_ != nullptr) {
            inputStyle_ = onTheSpot;
        }
    }

    constexpr ::XIMStyle overTheSpot = XIMPreeditPosition | XIMStatusNothing;
    if ((inputContext_ == nullptr) && (fontSet_ != nullptr) && supports(overTheSpot)) {
        ::XPoint spot = {0, 0};
        auto attributes = ::XVaCreateNestedList(0, XNSpotLocation, &spot, XNFontSet, fontSet_, nullptr);
        inputContext_ = ::XCreateIC(
            inputMethod_,
            XNInputStyle, overTheSpot,
            XNClientWindow, windowHandle_,
            XNFocusWindow, windowHandle_,
            XNPreeditAttributes, attributes,
            nullptr);
        ::XFree(attributes);
        if (inputContext_ != nullptr) {
            inputStyle_ = overTheSpot;
        }
    }

    constexpr ::XIMStyle root = XIMPreeditNothing | XIMStatusNothing;
    if ((inputContext_ == nullptr) && supports(root)) {
        inputContext_ = ::XCreateIC(
            inputMethod_,
            XNInputStyle, root,
            XNClientWindow, windowHandle_,
            XNFocusWindow, windowHandle_,
            nullptr);
        if (inputContext_ != nullptr) {
            inputStyle_ = root;
        }
    }

    if (inputContext_ == nullptr) {
        return false;
    }

    // NOTE: Keys reach the game until a text field takes focus.
    ::XUnsetICFocus(inputContext_);
    return true;
}

TextInputCapabilities TextInputServiceX11::getCapabilities() const noexcept
{
    TextInputCapabilities capabilities;
    capabilities.committedText = FeatureSupport::Available;

    // NOTE: An input method without preedit callbacks draws its composition
    // itself, at the caret with the over-the-spot style.
    capabilities.composition = FeatureSupport::Unavailable;
    if ((inputStyle_ & XIMPreeditCallbacks) != 0) {
        capabilities.composition = FeatureSupport::Available;
    }
    capabilities.candidateWindowPosition = FeatureSupport::Unavailable;
    if ((inputStyle_ & (XIMPreeditCallbacks | XIMPreeditPosition)) != 0) {
        capabilities.candidateWindowPosition = FeatureSupport::Available;
    }

    // TODO: Pass the surrounding text with XNStringConversionCallback, which
    // few input methods use.
    capabilities.surroundingText = FeatureSupport::NotImplemented;

    // NOTE: X11 has no interface to show a software keyboard.
    capabilities.softwareKeyboard = FeatureSupport::Unsupported;
    capabilities.softwareKeyboardArea = FeatureSupport::Unsupported;
    return capabilities;
}

void TextInputServiceX11::update()
{
    events_.clear();
    std::swap(events_, pendingEvents_);
}

std::span<const TextInputEvent> TextInputServiceX11::getEvents() const noexcept
{
    return events_;
}

void TextInputServiceX11::setFocusedField(const TextFieldState& field)
{
    POMDOG_ASSERT(field.id != 0);
    if (display_ == nullptr) {
        return;
    }

    if (field.id != focusedFieldID_) {
        // NOTE: Reset while the previous field still has focus, so that the
        // callbacks the reset causes carry its identifier.
        resetComposition();
        focusedFieldID_ = field.id;
    }
    if ((field.purpose != purpose_) && !usesInputMethod(field.purpose)) {
        resetComposition();
    }
    purpose_ = field.purpose;

    const bool caretMoved = (field.caretRect.x != caretRect_.x) ||
                            (field.caretRect.y != caretRect_.y) ||
                            (field.caretRect.width != caretRect_.width) ||
                            (field.caretRect.height != caretRect_.height);
    caretRect_ = field.caretRect;

    const bool wasFocused = inputContextFocused_;
    updateFocus();
    if (caretMoved && wasFocused && inputContextFocused_) {
        applySpotLocation();
    }
}

void TextInputServiceX11::clearFocusedField()
{
    if (focusedFieldID_ == 0) {
        return;
    }
    resetComposition();
    focusedFieldID_ = 0;
    updateFocus();
}

void TextInputServiceX11::setTextFieldAreas([[maybe_unused]] std::span<const TextFieldArea> areas)
{
    // NOTE: A tap needs no help to focus a field on this platform.
}

void TextInputServiceX11::showSoftwareKeyboard()
{
}

void TextInputServiceX11::hideSoftwareKeyboard()
{
}

SoftwareKeyboardState TextInputServiceX11::getSoftwareKeyboardState() const noexcept
{
    SoftwareKeyboardState state;
    state.visibility = SoftwareKeyboardVisibility::Hidden;
    return state;
}

int TextInputServiceX11::onPreeditStart(
    [[maybe_unused]] ::XIC inputContext,
    ::XPointer clientData,
    [[maybe_unused]] ::XPointer callData)
{
    auto self = reinterpret_cast<TextInputServiceX11*>(clientData);
    self->composition_.clear();

    // NOTE: A negative length places no limit on the composition.
    return -1;
}

void TextInputServiceX11::onPreeditDone(
    [[maybe_unused]] ::XIM inputMethod,
    ::XPointer clientData,
    [[maybe_unused]] ::XPointer callData)
{
    auto self = reinterpret_cast<TextInputServiceX11*>(clientData);
    self->endComposition();
}

void TextInputServiceX11::onPreeditDraw(
    [[maybe_unused]] ::XIM inputMethod,
    ::XPointer clientData,
    ::XPointer callData)
{
    auto self = reinterpret_cast<TextInputServiceX11*>(clientData);
    const auto draw = reinterpret_cast<const ::XIMPreeditDrawCallbackStruct*>(callData);
    if (draw == nullptr) {
        return;
    }

    // NOTE: The draw replaces chg_length characters at chg_first with text.
    // Without a string, only the feedback of those characters changes.
    const auto first = static_cast<std::size_t>(std::max(draw->chg_first, 0));
    const auto length = static_cast<std::size_t>(std::max(draw->chg_length, 0));
    std::u32string text;
    std::vector<u8> target;
    bool hasString = false;
    if (draw->text != nullptr) {
        const auto& ximText = *draw->text;
        const auto count = static_cast<std::size_t>(ximText.length);
        if (ximText.encoding_is_wchar != False) {
            if (ximText.string.wide_char != nullptr) {
                // NOTE: wchar_t holds UTF-32 on the platforms with X11.
                const std::span<const wchar_t> characters{ximText.string.wide_char, count};
                for (const auto c : characters) {
                    text.push_back(static_cast<char32_t>(c));
                }
                hasString = true;
            }
        }
        else if (ximText.string.multi_byte != nullptr) {
            appendUTF8AsUTF32(std::string_view{ximText.string.multi_byte}, text);
            hasString = true;
        }
        if (ximText.feedback != nullptr) {
            const std::span<const ::XIMFeedback> feedback{ximText.feedback, count};
            for (const auto f : feedback) {
                target.push_back(isConvertedFeedback(f) ? u8(1) : u8(0));
            }
        }
    }

    if ((draw->text != nullptr) && !hasString) {
        self->composition_.setTarget(first, target);
    }
    else {
        self->composition_.replace(first, length, text, target);
    }
    self->composition_.setCaret(static_cast<std::size_t>(std::max(draw->caret, 0)));
    self->composing_ = true;
    self->pushCompositionUpdate();
}

void TextInputServiceX11::onPreeditCaret(
    [[maybe_unused]] ::XIM inputMethod,
    ::XPointer clientData,
    ::XPointer callData)
{
    auto self = reinterpret_cast<TextInputServiceX11*>(clientData);
    auto caret = reinterpret_cast<::XIMPreeditCaretCallbackStruct*>(callData);
    if (caret == nullptr) {
        return;
    }

    auto position = self->composition_.getCaret();
    switch (caret->direction) {
    case XIMForwardChar:
        position += 1;
        break;
    case XIMBackwardChar:
        position = (position > 0) ? (position - 1) : 0;
        break;
    case XIMLineStart:
        position = 0;
        break;
    case XIMLineEnd:
        position = self->composition_.getLength();
        break;
    case XIMAbsolutePosition:
        position = static_cast<std::size_t>(std::max(caret->position, 0));
        break;
    default:
        break;
    }
    self->composition_.setCaret(position);

    // NOTE: The input method reads the new position back from the structure.
    caret->position = static_cast<int>(self->composition_.getCaret());
    self->pushCompositionUpdate();
}

bool TextInputServiceX11::filterEvent(::XEvent& event)
{
    const bool forWindow = (event.xany.window == windowHandle_);
    const bool isKey = (event.type == KeyPress) || (event.type == KeyRelease);

    // NOTE: The filters of the input method consume the keys it composes with
    // and the messages it exchanges with Xlib, and run the preedit callbacks.
    // The window's own input context also has a filter, for compose sequences,
    // which the host never ran. Keys skip the filters while no field uses the
    // input method, so that the game receives them as before.
    if (!forWindow || !isKey || inputContextFocused_) {
        bool filtered = false;
        {
            InputMethodScope scope{display_, inputLocale_};
            filtered = (::XFilterEvent(&event, None) != False);
        }
        if (filtered) {
            return true;
        }
    }
    if (!forWindow) {
        return false;
    }

    switch (event.type) {
    case FocusIn:
    case FocusOut:
        // NOTE: Focus that only follows the pointer does not move the keyboard.
        if (event.xfocus.detail != NotifyPointer) {
            windowFocused_ = (event.type == FocusIn);
            updateFocus();
        }
        break;
    case KeyPress:
        if (focusedFieldID_ != 0) {
            lookUpCommittedText(event.xkey);
        }
        break;
    default:
        break;
    }
    return false;
}

void TextInputServiceX11::lookUpCommittedText(::XKeyEvent& event)
{
    // NOTE: Committed text arrives as key presses, which the input method
    // creates for the text it committed. Typed characters arrive the same way.
    if (lookupBuffer_.size() < 64) {
        lookupBuffer_.resize(64);
    }
    ::KeySym keySym = 0;
    Status status = 0;
    ::XLockDisplay(display_);
    auto length = ::Xutf8LookupString(inputContext_, &event, lookupBuffer_.data(), static_cast<int>(lookupBuffer_.size()), &keySym, &status);
    if ((status == XBufferOverflow) && (length > 0)) {
        lookupBuffer_.resize(static_cast<std::size_t>(length));
        length = ::Xutf8LookupString(inputContext_, &event, lookupBuffer_.data(), static_cast<int>(lookupBuffer_.size()), &keySym, &status);
    }
    ::XUnlockDisplay(display_);

    if (((status != XLookupChars) && (status != XLookupBoth)) || (length <= 0)) {
        return;
    }
    const std::span<const char> bytes{lookupBuffer_.data(), static_cast<std::size_t>(length)};
    std::string text;
    for (const auto c : bytes) {
        if (!isControlByte(c)) {
            text.push_back(c);
        }
    }
    if (text.empty()) {
        return;
    }

    TextInputEvent commit;
    commit.kind = TextInputEventKind::Commit;
    commit.fieldID = focusedFieldID_;
    commit.text = std::move(text);
    pushEvent(std::move(commit));
}

void TextInputServiceX11::pushCompositionUpdate()
{
    if (focusedFieldID_ == 0) {
        return;
    }
    TextInputEvent event;
    event.kind = TextInputEventKind::CompositionUpdate;
    event.fieldID = focusedFieldID_;
    composition_.fillEvent(event);
    pushEvent(std::move(event));
}

void TextInputServiceX11::pushEvent(TextInputEvent&& event)
{
    if (event.kind != TextInputEventKind::Commit) {
        pendingEvents_.push_back(std::move(event));
        return;
    }

    const auto precedes = [&](std::vector<TextInputEvent>::iterator position, TextInputEventKind kind) {
        if (position == pendingEvents_.begin()) {
            return false;
        }
        const auto& previous = *std::prev(position);
        return (previous.kind == kind) && (previous.fieldID == event.fieldID);
    };

    // NOTE: Xlib queues committed text as a key press, which may arrive after
    // the input method ended the composition. Commit before the end, as the
    // events of other services do.
    auto position = pendingEvents_.end();
    if (precedes(position, TextInputEventKind::CompositionEnd)) {
        position = std::prev(position);
    }

    // NOTE: Typing produces one character at a time. Join them into one event.
    if (precedes(position, TextInputEventKind::Commit)) {
        std::prev(position)->text.append(event.text);
        return;
    }
    pendingEvents_.insert(position, std::move(event));
}

void TextInputServiceX11::endComposition()
{
    composition_.clear();
    if (!composing_) {
        return;
    }
    composing_ = false;
    if (focusedFieldID_ != 0) {
        TextInputEvent event;
        event.kind = TextInputEventKind::CompositionEnd;
        event.fieldID = focusedFieldID_;
        pushEvent(std::move(event));
    }
}

void TextInputServiceX11::resetComposition() noexcept
{
    if (!inputContextFocused_ && !composing_) {
        return;
    }

    // NOTE: The reset discards the composition. It returns the discarded text,
    // which the service does not use.
    {
        InputMethodScope scope{display_, inputLocale_};
        if (auto discarded = ::Xutf8ResetIC(inputContext_); discarded != nullptr) {
            ::XFree(discarded);
        }
    }

    // NOTE: An input method usually ends the composition during the reset.
    // End it here when it did not.
    endComposition();
}

void TextInputServiceX11::updateFocus() noexcept
{
    const bool focused = (focusedFieldID_ != 0) && windowFocused_ && usesInputMethod(purpose_);
    if (focused == inputContextFocused_) {
        return;
    }
    inputContextFocused_ = focused;

    // NOTE: Only one input context takes the keys at a time. The window's own
    // context handles compose sequences for the keyboard's text while no field
    // uses the input method.
    const auto windowContext = window_->getInputContext();
    {
        InputMethodScope scope{display_, inputLocale_};
        if (focused) {
            if (windowContext != nullptr) {
                ::XUnsetICFocus(windowContext);
            }
            ::XSetICFocus(inputContext_);
        }
        else {
            ::XUnsetICFocus(inputContext_);
            if (windowContext != nullptr) {
                ::XSetICFocus(windowContext);
            }
        }
    }

    if (focused) {
        applySpotLocation();
    }
}

void TextInputServiceX11::applySpotLocation() noexcept
{
    if (!inputContextFocused_ || ((inputStyle_ & (XIMPreeditCallbacks | XIMPreeditPosition)) == 0)) {
        return;
    }

    // NOTE: The spot is the baseline of the caret in the window's physical
    // pixels. Input methods open their candidate window below it.
    const auto pixelRatio = window_->getDisplayMetrics().pixelRatio;
    const auto toCoordinate = [](f32 value) -> short {
        return static_cast<short>(std::clamp(std::lround(value), -32768L, 32767L));
    };
    ::XPoint spot;
    spot.x = toCoordinate(caretRect_.x * pixelRatio);
    spot.y = toCoordinate((caretRect_.y + caretRect_.height) * pixelRatio);

    InputMethodScope scope{display_, inputLocale_};
    auto attributes = ::XVaCreateNestedList(0, XNSpotLocation, &spot, nullptr);
    ::XSetICValues(inputContext_, XNPreeditAttributes, attributes, nullptr);
    ::XFree(attributes);
}

} // namespace pomdog::textinput::detail::x11

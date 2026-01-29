// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/text_edit.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/input/keyboard_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/utfcpp_headers.h"

namespace pomdog::gui {
namespace {
constexpr double CursorBlinkInterval = 1.0;
constexpr double CursorMoveMarginInterval = 0.5;
constexpr double CursorMoveInterval = 0.1;

enum class CursorMoveOperation {
    Left,
    Right,
};

void MoveCursor(
    CursorMoveOperation operation,
    const KeyboardState& keys,
    const std::string& text,
    std::optional<int>& cursorPosition,
    std::optional<int>& startSelectionPosition,
    Duration& cursorMoveInterval)
{
    if (cursorPosition == std::nullopt) {
        return;
    }

    const auto oldCursorPosition = *cursorPosition;

    switch (operation) {
    case CursorMoveOperation::Left:
        cursorPosition = std::max(*cursorPosition - 1, 0);
        break;
    case CursorMoveOperation::Right:
        // FIXME: UTF-8
        cursorPosition = std::min(*cursorPosition + 1, static_cast<int>(text.size()));
        break;
    }

    cursorMoveInterval = Duration{-CursorMoveMarginInterval};

    if (keys.isKeyDown(Keys::LeftShift) || keys.isKeyDown(Keys::RightShift)) {
        if ((startSelectionPosition == std::nullopt) && (oldCursorPosition != *cursorPosition)) {
            startSelectionPosition = oldCursorPosition;
        }
    }
    else {
        startSelectionPosition = std::nullopt;
    }
}

void BackspaceText(
    std::string& text,
    int& cursorPosition,
    std::optional<int>& startSelectionPosition)
{
    if (text.empty()) {
        return;
    }
    if (cursorPosition < 0) {
        return;
    }
    if ((cursorPosition == 0) && (startSelectionPosition == std::nullopt)) {
        return;
    }
    auto u32String = utf8::utf8to32(text);

    if (cursorPosition > static_cast<int>(u32String.size())) {
        return;
    }

    int startPos = cursorPosition - 1;
    int count = 1;

    if (startSelectionPosition != std::nullopt) {
        startPos = std::min(cursorPosition, *startSelectionPosition);
        count = std::abs(cursorPosition - *startSelectionPosition);
    }

    u32String.erase(startPos, count);
    cursorPosition = std::clamp(startPos, 0, static_cast<int>(u32String.size()));

    text = utf8::utf32to8(u32String);
    startSelectionPosition = std::nullopt;
}

} // namespace

TextEdit::TextEdit(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , textMargin({4, 4, 4, 4})
    , cursorBlinkInterval(Duration::zero())
    , cursorMoveInterval(Duration::zero())
    , baselineHeight(0.0f)
    , fontWeight(FontWeight::Normal)
    , fontSize(FontSize::Medium)
    , horizontalAlignment(HorizontalAlignment::Stretch)
    , isEnabled(true)
    , isReadOnly(false)
    , isAcceptable(true)
    , needToSubmit(false)
{
    SetSize(50, 22);
    SetCursor(MouseCursor::IBeam);
}

void TextEdit::SetFontWeight(FontWeight fontWeightIn)
{
    fontWeight = fontWeightIn;
}

void TextEdit::SetFontSize(FontSize fontSizeIn)
{
    fontSize = fontSizeIn;
}

bool TextEdit::IsFocused() const
{
    return (cursorPosition != std::nullopt);
}

bool TextEdit::IsEnabled() const
{
    return isEnabled;
}

void TextEdit::SetEnabled(bool enabledIn)
{
    this->isEnabled = enabledIn;
    if (!isEnabled) {
        this->cursorPosition = std::nullopt;
    }
}

bool TextEdit::IsReadOnly() const
{
    return isReadOnly;
}

void TextEdit::SetReadOnly(bool readOnlyIn)
{
    this->isReadOnly = readOnlyIn;
}

bool TextEdit::IsAcceptable() const
{
    return isAcceptable;
}

void TextEdit::SetAcceptable(bool acceptableIn)
{
    this->isAcceptable = acceptableIn;
}

std::string TextEdit::GetText() const
{
    return this->text;
}

void TextEdit::SetText(const std::string& textIn)
{
    this->text = textIn;

    if (startSelectionPosition != std::nullopt) {
        // FIXME: UTF-8
        startSelectionPosition = std::clamp(*startSelectionPosition, 0, static_cast<int>(text.size()));
    }
    if (cursorPosition != std::nullopt) {
        // FIXME: UTF-8
        cursorPosition = std::clamp(*cursorPosition, 0, static_cast<int>(text.size()));
    }
}

std::string TextEdit::GetPlaceholderText() const
{
    return this->placeholderText;
}

void TextEdit::SetPlaceholderText(const std::string& textIn)
{
    this->placeholderText = textIn;
}

Thickness TextEdit::GetTextMargin() const
{
    return textMargin;
}

void TextEdit::SetTextMargin(const Thickness& margin)
{
    this->textMargin = margin;
}

void TextEdit::SetBaselineHeight(float pixelSize)
{
    this->baselineHeight = pixelSize;
}

bool TextEdit::HasSelectedText() const
{
    return this->startSelectionPosition != std::nullopt;
}

void TextEdit::SetSelection(int start, int length)
{
    if (length == 0) {
        startSelectionPosition = std::nullopt;
    }
    else {
        startSelectionPosition = std::clamp(start, 0, static_cast<int>(text.size()));
        cursorPosition = std::clamp(*cursorPosition, 0, start + length);
    }
}

void TextEdit::Deselect()
{
    this->startSelectionPosition = std::nullopt;
}

void TextEdit::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    if (horizontalAlignment == horizontalAlignmentIn) {
        return;
    }
    horizontalAlignment = horizontalAlignmentIn;
    if (auto parent = GetParent()) {
        parent->MarkContentLayoutDirty();
    }
}

HorizontalAlignment TextEdit::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

VerticalAlignment TextEdit::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void TextEdit::OnEnter()
{
}

void TextEdit::OnFocusIn()
{
    if (cursorPosition == std::nullopt) {
        // FIXME: UTF-8
        cursorPosition = static_cast<int>(text.size());
    }
}

void TextEdit::OnFocusOut()
{
    cursorPosition = std::nullopt;

    if (needToSubmit) {
        TextSubmitted();
    }
    needToSubmit = false;

    FocusOut();
}

void TextEdit::OnTextInput([[maybe_unused]] const KeyboardState& keyboardState, const std::string& inputText)
{
    if (cursorPosition == std::nullopt) {
        cursorPosition = 0;
    }
    // FIXME: UTF-8
    cursorPosition = std::clamp(*cursorPosition, 0, static_cast<int>(text.size()));

    auto textIter = std::begin(inputText);
    const auto textEnd = std::end(inputText);

    bool textChanged = false;

    // NOTE: Require to recalculate the start position
    textStartPositionX = std::nullopt;

    while (textIter != textEnd) {
        const auto character = utf8::next(textIter, textEnd);

        // FIXME: UTF-8
        POMDOG_ASSERT(cursorPosition != std::nullopt);
        POMDOG_ASSERT(*cursorPosition >= 0);
        POMDOG_ASSERT(*cursorPosition <= static_cast<int>(text.size()));

        switch (character) {
        case 0x8:
            // NOTE: "Backspace" key
            if (!isReadOnly) {
                BackspaceText(text, *cursorPosition, startSelectionPosition);
                textChanged = true;
            }
            break;
        case 0x7F:
            // NOTE: "Delete" key
            if (!isReadOnly) {
                BackspaceText(text, *cursorPosition, startSelectionPosition);
                textChanged = true;
            }
            break;
        case 0x0D:
            // NOTE: "Carriage Return" (CR)
            if (!isReadOnly) {
                needToSubmit = true;
                auto dispatcher = GetDispatcher();
                dispatcher->ClearFocus(shared_from_this());
                return;
            }
            break;
        case 0x0000F702:
            // NOTE: "Right To Left"
            break;
        case 0x0000F703:
            // NOTE: "Left To Right"
            break;
        default:
            if (!isReadOnly && (character > 31) && (character < 127)) {
                if (HasSelectedText()) {
                    BackspaceText(text, *cursorPosition, startSelectionPosition);
                    Deselect();
                }

                std::u32string utf32String;
                utf32String.push_back(character);
                text.insert(*cursorPosition, utf8::utf32to8(utf32String));

                POMDOG_ASSERT(cursorPosition != std::nullopt);
                cursorPosition = *cursorPosition + 1;
                textChanged = true;
            }
            break;
        }
    }

    if (textChanged) {
        TextChanged();
        needToSubmit = true;
    }
}

void TextEdit::OnKeyDown(const KeyboardState& keyboardState, Keys key)
{
    latestKeys = keyboardState;

    switch (key) {
    case Keys::LeftArrow:
        if (cursorPosition != std::nullopt) {
            MoveCursor(
                CursorMoveOperation::Left,
                latestKeys,
                text,
                cursorPosition,
                startSelectionPosition,
                cursorMoveInterval);
        }
        break;
    case Keys::RightArrow:
        if (cursorPosition != std::nullopt) {
            MoveCursor(
                CursorMoveOperation::Right,
                latestKeys,
                text,
                cursorPosition,
                startSelectionPosition,
                cursorMoveInterval);
        }
        break;
    default:
        break;
    }
}

void TextEdit::OnKeyUp(const KeyboardState& keyboardState, [[maybe_unused]] Keys key)
{
    latestKeys = keyboardState;
}

void TextEdit::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
}

void TextEdit::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void TextEdit::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }
    if (!isEnabled) {
        return;
    }
    if (pointerPoint.ClickCount == 2) {
        return;
    }

    POMDOG_ASSERT(cursorPosition != std::nullopt);
    if (cursorPosition == std::nullopt) {
        return;
    }
    if (text.empty()) {
        return;
    }
    if (spriteFont == nullptr) {
        return;
    }

    const auto oldCursorPosition = *cursorPosition;

    const auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());

    auto startPosX = 0.0f;
    if (textStartPositionX != std::nullopt) {
        startPosX = *textStartPositionX;
    }

    cursorPosition = 0;
    // FIXME: UTF-8
    for (int i = 1; i <= static_cast<int>(text.size()); i++) {
        auto size = spriteFont->measureString(text.substr(0, i));
        if (size.x > (pointInView.x + startPosX)) {
            break;
        }
        cursorPosition = i;
    }

    if ((startSelectionPosition == std::nullopt) && (oldCursorPosition != cursorPosition)) {
        startSelectionPosition = oldCursorPosition;
    }

    // FIXME: UTF-8
    POMDOG_ASSERT(*cursorPosition >= 0);
    POMDOG_ASSERT(*cursorPosition <= static_cast<int>(text.size()));
}

void TextEdit::OnPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    cursorBlinkInterval = Duration::zero();

    if (text.empty()) {
        cursorPosition = 0;
        return;
    }
    if (spriteFont == nullptr) {
        return;
    }

    if (pointerPoint.ClickCount == 2) {
        cursorPosition = 0;

        // FIXME: UTF-8
        startSelectionPosition = static_cast<int>(text.size());
        return;
    }

    const auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());

    auto startPosX = 0.0f;
    if (textStartPositionX != std::nullopt) {
        startPosX = *textStartPositionX;
    }

    cursorPosition = 0;
    // FIXME: UTF-8
    for (int i = 1; i <= static_cast<int>(text.size()); i++) {
        auto size = spriteFont->measureString(text.substr(0, i));
        if (size.x > (pointInView.x + startPosX)) {
            break;
        }
        cursorPosition = i;
    }

    if (latestKeys.isKeyUp(Keys::LeftShift) && latestKeys.isKeyUp(Keys::RightShift)) {
        Deselect();
    }

    // FIXME: UTF-8
    POMDOG_ASSERT(*cursorPosition >= 0);
    POMDOG_ASSERT(*cursorPosition <= static_cast<int>(text.size()));
}

void TextEdit::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void TextEdit::UpdateAnimation(const Duration& frameDuration)
{
    if (!cursorPosition) {
        return;
    }

    cursorBlinkInterval += frameDuration;
    if (cursorBlinkInterval.count() >= CursorBlinkInterval) {
        cursorBlinkInterval = Duration::zero();
    }

    cursorMoveInterval += frameDuration;
    if (cursorMoveInterval.count() >= CursorMoveInterval) {
        if (latestKeys.isKeyDown(Keys::LeftArrow)) {
            POMDOG_ASSERT(cursorPosition != std::nullopt);
            MoveCursor(
                CursorMoveOperation::Left,
                latestKeys,
                text,
                cursorPosition,
                startSelectionPosition,
                cursorMoveInterval);

            cursorBlinkInterval = Duration::zero();
        }
        if (latestKeys.isKeyDown(Keys::RightArrow)) {
            POMDOG_ASSERT(cursorPosition != std::nullopt);
            MoveCursor(
                CursorMoveOperation::Right,
                latestKeys,
                text,
                cursorPosition,
                startSelectionPosition,
                cursorMoveInterval);

            cursorBlinkInterval = Duration::zero();
        }
        cursorMoveInterval = Duration::zero();
    }
}

void TextEdit::Draw(DrawingContext& drawingContext)
{
    spriteFont = drawingContext.GetFont(fontWeight, fontSize);

    const Color textNormalColor = Color{255, 255, 255, 255};
    const Color placeholderTextColor = Color{192, 189, 190, 255};
    const Color borderNormalColor = Color{104, 100, 101, 255};
    const Color borderFocusColor = Color{31, 115, 172, 255};
    const Color borderErrorColor = Color{160, 24, 0, 255};
    const Color selectionBoundsColor = Color{220, 220, 220, 160};

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto textColor = textNormalColor;
    auto cursorColor = textNormalColor;
    auto borderColor = borderNormalColor;

    if (!isAcceptable) {
        borderColor = borderErrorColor;
    }
    else if (this->IsFocused()) {
        borderColor = borderFocusColor;
    }

    const auto marginLeftBottom = Vector2{static_cast<float>(textMargin.left), static_cast<float>(textMargin.bottom)};
    const auto textEditPos = math::toVector2(globalPos);
    const auto textPosition = textEditPos + marginLeftBottom + Vector2{0.0f, baselineHeight};
    const auto innerBoundPos = textEditPos + marginLeftBottom;
    const auto innerBoundSize = Vector2{
        GetWidth() - static_cast<float>(textMargin.left + textMargin.right),
        GetHeight() - static_cast<float>(textMargin.bottom + textMargin.top)};

    // NOTE: Mask scissor
    drawingContext.PushScissorRect(Rectangle{
        static_cast<int>(innerBoundPos.x),
        static_cast<int>(innerBoundPos.y),
        static_cast<int>(innerBoundSize.x),
        static_cast<int>(innerBoundSize.y)});

    constexpr float cursorThickness = 1.0f;

    const auto calculatePositionInSprite = [&](int positionInText) -> float {
        if (text.empty()) {
            return marginLeftBottom.x;
        }

        // FIXME: UTF-8
        POMDOG_ASSERT(positionInText >= 0);
        POMDOG_ASSERT(positionInText <= static_cast<int>(text.size()));
        auto substring = text.substr(0, positionInText);
        auto v = spriteFont->measureString(substring);
        constexpr float offset = 0.2f;
        return marginLeftBottom.x + v.x + offset;
    };

    auto cursorDrawPosition = Vector2::createZero();
    if (cursorPosition != std::nullopt) {
        cursorDrawPosition.x = calculatePositionInSprite(*cursorPosition);

        if (textStartPositionX == std::nullopt) {
            textStartPositionX = std::max(cursorDrawPosition.x - innerBoundSize.x, 0.0f);
        }
        if (*textStartPositionX > 0) {
            cursorDrawPosition.x = cursorDrawPosition.x - *textStartPositionX;
        }
    }

    // NOTE: selection bounds rendering
    if ((startSelectionPosition != std::nullopt) &&
        (cursorPosition != std::nullopt) &&
        (*startSelectionPosition != *cursorPosition)) {

        POMDOG_ASSERT(textStartPositionX != std::nullopt);
        float startSelectionPosX = calculatePositionInSprite(*startSelectionPosition) - *textStartPositionX;

        float selectionWidth = cursorDrawPosition.x - startSelectionPosX;
        Vector2 startPos = Vector2{startSelectionPosX, cursorDrawPosition.y};

        if (cursorDrawPosition.x < startSelectionPosX) {
            startPos.x = cursorDrawPosition.x;
            selectionWidth = startSelectionPosX - cursorDrawPosition.x;
        }

        constexpr float cursorHeightMargin = 2.0f;

        primitiveBatch->drawRectangle(
            Matrix3x2::createIdentity(),
            startPos + Vector2{0.0f, cursorHeightMargin} + math::toVector2(globalPos),
            selectionWidth,
            GetHeight() - cursorHeightMargin * 2.0f - 0.5f,
            selectionBoundsColor);
    }

    // NOTE: cursor rendering
    if (!HasSelectedText() &&
        (cursorPosition != std::nullopt) &&
        ((cursorBlinkInterval.count() / CursorBlinkInterval) <= 0.5)) {
        constexpr float cursorHeightMargin = 2.0f;

        primitiveBatch->drawRectangle(
            Matrix3x2::createIdentity(),
            cursorDrawPosition + Vector2{0.0f, cursorHeightMargin} + math::toVector2(globalPos),
            cursorThickness,
            GetHeight() - cursorHeightMargin * 2.0f - 0.5f,
            cursorColor);
    }

    primitiveBatch->flush();

    if (!text.empty()) {
        // NOTE: Draw input text
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto startPos = Vector2::createZero();
        if (textStartPositionX != std::nullopt) {
            startPos.x = *textStartPositionX;
        }
        spriteFont->draw(
            *spriteBatch,
            text,
            textPosition - startPos,
            textColor,
            0.0f,
            Vector2{0.0f, 0.0f},
            1.0f);

        spriteBatch->flush();
    }
    else if (!placeholderText.empty()) {
        // NOTE: Draw placeholder text
        auto spriteBatch = drawingContext.GetSpriteBatch();
        spriteFont->draw(
            *spriteBatch,
            placeholderText,
            textPosition,
            placeholderTextColor,
            0.0f,
            Vector2{0.0f, 0.0f},
            1.0f);

        spriteBatch->flush();
    }

    drawingContext.PopScissorRect();

    if ((textStartPositionX != std::nullopt) &&
        (cursorPosition != std::nullopt) &&
        (cursorMoveInterval == Duration::zero())) {
        if (cursorDrawPosition.x < marginLeftBottom.x) {
            // NOTE: Recalculate the start position
            textStartPositionX = (*textStartPositionX + cursorDrawPosition.x - marginLeftBottom.x);
        }
        if (cursorDrawPosition.x > innerBoundSize.x) {
            // NOTE: Recalculate the start position
            textStartPositionX = (*textStartPositionX + (cursorDrawPosition.x - innerBoundSize.x));
        }
    }
}

} // namespace pomdog::gui

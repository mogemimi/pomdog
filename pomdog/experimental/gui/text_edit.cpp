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
    const std::string& text_,
    std::optional<int>& cursorPosition_,
    std::optional<int>& startSelectionPosition_,
    Duration& cursorMoveInterval_)
{
    if (cursorPosition_ == std::nullopt) {
        return;
    }

    const auto oldCursorPosition = *cursorPosition_;

    switch (operation) {
    case CursorMoveOperation::Left:
        cursorPosition_ = std::max(*cursorPosition_ - 1, 0);
        break;
    case CursorMoveOperation::Right:
        // FIXME: UTF-8
        cursorPosition_ = std::min(*cursorPosition_ + 1, static_cast<int>(text_.size()));
        break;
    }

    cursorMoveInterval_ = Duration{-CursorMoveMarginInterval};

    if (keys.isKeyDown(Keys::LeftShift) || keys.isKeyDown(Keys::RightShift)) {
        if ((startSelectionPosition_ == std::nullopt) && (oldCursorPosition != *cursorPosition_)) {
            startSelectionPosition_ = oldCursorPosition;
        }
    }
    else {
        startSelectionPosition_ = std::nullopt;
    }
}

void BackspaceText(
    std::string& text_,
    int& cursorPosition_,
    std::optional<int>& startSelectionPosition_)
{
    if (text_.empty()) {
        return;
    }
    if (cursorPosition_ < 0) {
        return;
    }
    if ((cursorPosition_ == 0) && (startSelectionPosition_ == std::nullopt)) {
        return;
    }
    auto u32String = utf8::utf8to32(text_);

    if (cursorPosition_ > static_cast<int>(u32String.size())) {
        return;
    }

    int startPos = cursorPosition_ - 1;
    int count = 1;

    if (startSelectionPosition_ != std::nullopt) {
        startPos = std::min(cursorPosition_, *startSelectionPosition_);
        count = std::abs(cursorPosition_ - *startSelectionPosition_);
    }

    u32String.erase(startPos, count);
    cursorPosition_ = std::clamp(startPos, 0, static_cast<int>(u32String.size()));

    text_ = utf8::utf32to8(u32String);
    startSelectionPosition_ = std::nullopt;
}

} // namespace

TextEdit::TextEdit(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , textMargin_({4, 4, 4, 4})
    , cursorBlinkInterval_(Duration::zero())
    , cursorMoveInterval_(Duration::zero())
    , fontWeight_(FontWeight::Normal)
    , fontSize_(FontSize::Medium)
    , horizontalAlignment_(HorizontalAlignment::Stretch)
    , isEnabled_(true)
    , isReadOnly_(false)
    , isAcceptable_(true)
    , needToSubmit_(false)
{
    setSize(50, 22);
    setCursor(MouseCursor::IBeam);
}

void TextEdit::setFontWeight(FontWeight fontWeightIn)
{
    fontWeight_ = fontWeightIn;
}

void TextEdit::setFontSize(FontSize fontSizeIn)
{
    fontSize_ = fontSizeIn;
}

bool TextEdit::isFocused() const
{
    return (cursorPosition_ != std::nullopt);
}

bool TextEdit::isEnabled() const
{
    return isEnabled_;
}

void TextEdit::setEnabled(bool enabledIn)
{
    isEnabled_ = enabledIn;
    if (!isEnabled_) {
        cursorPosition_ = std::nullopt;
    }
}

bool TextEdit::isReadOnly() const
{
    return isReadOnly_;
}

void TextEdit::setReadOnly(bool readOnlyIn)
{
    isReadOnly_ = readOnlyIn;
}

bool TextEdit::isAcceptable() const
{
    return isAcceptable_;
}

void TextEdit::setAcceptable(bool acceptableIn)
{
    isAcceptable_ = acceptableIn;
}

std::string TextEdit::getText() const
{
    return text_;
}

void TextEdit::setText(const std::string& textIn)
{
    text_ = textIn;

    if (startSelectionPosition_ != std::nullopt) {
        // FIXME: UTF-8
        startSelectionPosition_ = std::clamp(*startSelectionPosition_, 0, static_cast<int>(text_.size()));
    }
    if (cursorPosition_ != std::nullopt) {
        // FIXME: UTF-8
        cursorPosition_ = std::clamp(*cursorPosition_, 0, static_cast<int>(text_.size()));
    }
}

std::string TextEdit::getPlaceholderText() const
{
    return placeholderText_;
}

void TextEdit::setPlaceholderText(const std::string& textIn)
{
    placeholderText_ = textIn;
}

Thickness TextEdit::getTextMargin() const
{
    return textMargin_;
}

void TextEdit::setTextMargin(const Thickness& margin)
{
    textMargin_ = margin;
}

void TextEdit::setBaselineOffset(f32 pixelSize)
{
    baselineOffset_ = pixelSize;
}

bool TextEdit::hasSelectedText() const
{
    return startSelectionPosition_ != std::nullopt;
}

void TextEdit::setSelection(int start, int length)
{
    if (length == 0) {
        startSelectionPosition_ = std::nullopt;
    }
    else {
        startSelectionPosition_ = std::clamp(start, 0, static_cast<int>(text_.size()));
        cursorPosition_ = std::clamp(*cursorPosition_, 0, start + length);
    }
}

void TextEdit::deselect()
{
    startSelectionPosition_ = std::nullopt;
}

void TextEdit::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    if (horizontalAlignment_ == horizontalAlignmentIn) {
        return;
    }
    horizontalAlignment_ = horizontalAlignmentIn;
    if (auto parent = getParent()) {
        parent->markContentLayoutDirty();
    }
}

HorizontalAlignment TextEdit::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

VerticalAlignment TextEdit::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void TextEdit::onEnter()
{
}

void TextEdit::onFocusIn()
{
    if (cursorPosition_ == std::nullopt) {
        // FIXME: UTF-8
        cursorPosition_ = static_cast<int>(text_.size());
    }
}

void TextEdit::onFocusOut()
{
    cursorPosition_ = std::nullopt;

    if (needToSubmit_) {
        TextSubmitted();
    }
    needToSubmit_ = false;

    FocusOut();
}

void TextEdit::onTextInput([[maybe_unused]] const KeyboardState& keyboardState, const std::string& inputText)
{
    if (cursorPosition_ == std::nullopt) {
        cursorPosition_ = 0;
    }
    // FIXME: UTF-8
    cursorPosition_ = std::clamp(*cursorPosition_, 0, static_cast<int>(text_.size()));

    auto textIter = std::begin(inputText);
    const auto textEnd = std::end(inputText);

    bool textChanged = false;

    // NOTE: Require to recalculate the start position
    textStartPositionX_ = std::nullopt;

    while (textIter != textEnd) {
        const auto character = utf8::next(textIter, textEnd);

        // FIXME: UTF-8
        POMDOG_ASSERT(cursorPosition_ != std::nullopt);
        POMDOG_ASSERT(*cursorPosition_ >= 0);
        POMDOG_ASSERT(*cursorPosition_ <= static_cast<int>(text_.size()));

        switch (character) {
        case 0x8:
            // NOTE: "Backspace" key
            if (!isReadOnly_) {
                BackspaceText(text_, *cursorPosition_, startSelectionPosition_);
                textChanged = true;
            }
            break;
        case 0x7F:
            // NOTE: "Delete" key
            if (!isReadOnly_) {
                BackspaceText(text_, *cursorPosition_, startSelectionPosition_);
                textChanged = true;
            }
            break;
        case 0x0D:
            // NOTE: "Carriage Return" (CR)
            if (!isReadOnly_) {
                needToSubmit_ = true;
                auto dispatcher = getDispatcher();
                dispatcher->clearFocus(shared_from_this());
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
            if (!isReadOnly_ && (character > 31) && (character < 127)) {
                if (hasSelectedText()) {
                    BackspaceText(text_, *cursorPosition_, startSelectionPosition_);
                    deselect();
                }

                std::u32string utf32String;
                utf32String.push_back(character);
                text_.insert(*cursorPosition_, utf8::utf32to8(utf32String));

                POMDOG_ASSERT(cursorPosition_ != std::nullopt);
                cursorPosition_ = *cursorPosition_ + 1;
                textChanged = true;
            }
            break;
        }
    }

    if (textChanged) {
        TextChanged();
        needToSubmit_ = true;
    }
}

void TextEdit::onKeyDown(const KeyboardState& keyboardState, Keys key)
{
    latestKeys_ = keyboardState;

    switch (key) {
    case Keys::LeftArrow:
        if (cursorPosition_ != std::nullopt) {
            MoveCursor(
                CursorMoveOperation::Left,
                latestKeys_,
                text_,
                cursorPosition_,
                startSelectionPosition_,
                cursorMoveInterval_);
        }
        break;
    case Keys::RightArrow:
        if (cursorPosition_ != std::nullopt) {
            MoveCursor(
                CursorMoveOperation::Right,
                latestKeys_,
                text_,
                cursorPosition_,
                startSelectionPosition_,
                cursorMoveInterval_);
        }
        break;
    default:
        break;
    }
}

void TextEdit::onKeyUp(const KeyboardState& keyboardState, [[maybe_unused]] Keys key)
{
    latestKeys_ = keyboardState;
}

void TextEdit::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
}

void TextEdit::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void TextEdit::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }
    if (!isEnabled_) {
        return;
    }
    if (pointerPoint.ClickCount == 2) {
        return;
    }

    POMDOG_ASSERT(cursorPosition_ != std::nullopt);
    if (cursorPosition_ == std::nullopt) {
        return;
    }
    if (text_.empty()) {
        return;
    }
    if (spriteFont_ == nullptr) {
        return;
    }

    const auto oldCursorPosition = *cursorPosition_;

    const auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());

    auto startPosX = 0.0f;
    if (textStartPositionX_ != std::nullopt) {
        startPosX = *textStartPositionX_;
    }

    cursorPosition_ = 0;
    // FIXME: UTF-8
    for (int i = 1; i <= static_cast<int>(text_.size()); i++) {
        auto size = spriteFont_->measureString(text_.substr(0, i));
        if (size.x > (pointInView.x + startPosX)) {
            break;
        }
        cursorPosition_ = i;
    }

    if ((startSelectionPosition_ == std::nullopt) && (oldCursorPosition != cursorPosition_)) {
        startSelectionPosition_ = oldCursorPosition;
    }

    // FIXME: UTF-8
    POMDOG_ASSERT(*cursorPosition_ >= 0);
    POMDOG_ASSERT(*cursorPosition_ <= static_cast<int>(text_.size()));
}

void TextEdit::onPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled_) {
        return;
    }

    cursorBlinkInterval_ = Duration::zero();

    if (text_.empty()) {
        cursorPosition_ = 0;
        return;
    }
    if (spriteFont_ == nullptr) {
        return;
    }

    if (pointerPoint.ClickCount == 2) {
        cursorPosition_ = 0;

        // FIXME: UTF-8
        startSelectionPosition_ = static_cast<int>(text_.size());
        return;
    }

    const auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());

    auto startPosX = 0.0f;
    if (textStartPositionX_ != std::nullopt) {
        startPosX = *textStartPositionX_;
    }

    cursorPosition_ = 0;
    // FIXME: UTF-8
    for (int i = 1; i <= static_cast<int>(text_.size()); i++) {
        auto size = spriteFont_->measureString(text_.substr(0, i));
        if (size.x > (pointInView.x + startPosX)) {
            break;
        }
        cursorPosition_ = i;
    }

    if (latestKeys_.isKeyUp(Keys::LeftShift) && latestKeys_.isKeyUp(Keys::RightShift)) {
        deselect();
    }

    // FIXME: UTF-8
    POMDOG_ASSERT(*cursorPosition_ >= 0);
    POMDOG_ASSERT(*cursorPosition_ <= static_cast<int>(text_.size()));
}

void TextEdit::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void TextEdit::updateAnimation(const Duration& frameDuration)
{
    if (!cursorPosition_) {
        return;
    }

    cursorBlinkInterval_ += frameDuration;
    if (cursorBlinkInterval_.count() >= CursorBlinkInterval) {
        cursorBlinkInterval_ = Duration::zero();
    }

    cursorMoveInterval_ += frameDuration;
    if (cursorMoveInterval_.count() >= CursorMoveInterval) {
        if (latestKeys_.isKeyDown(Keys::LeftArrow)) {
            POMDOG_ASSERT(cursorPosition_ != std::nullopt);
            MoveCursor(
                CursorMoveOperation::Left,
                latestKeys_,
                text_,
                cursorPosition_,
                startSelectionPosition_,
                cursorMoveInterval_);

            cursorBlinkInterval_ = Duration::zero();
        }
        if (latestKeys_.isKeyDown(Keys::RightArrow)) {
            POMDOG_ASSERT(cursorPosition_ != std::nullopt);
            MoveCursor(
                CursorMoveOperation::Right,
                latestKeys_,
                text_,
                cursorPosition_,
                startSelectionPosition_,
                cursorMoveInterval_);

            cursorBlinkInterval_ = Duration::zero();
        }
        cursorMoveInterval_ = Duration::zero();
    }
}

void TextEdit::draw(DrawingContext& drawingContext)
{
    spriteFont_ = drawingContext.getFont(fontWeight_, fontSize_);

    const Color textNormalColor = Color{255, 255, 255, 255};
    const Color placeholderTextColor = Color{192, 189, 190, 255};
    const Color borderNormalColor = Color{104, 100, 101, 255};
    const Color borderFocusColor = Color{31, 115, 172, 255};
    const Color borderErrorColor = Color{160, 24, 0, 255};
    const Color selectionBoundsColor = Color{220, 220, 220, 160};

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    auto textColor = textNormalColor;
    auto cursorColor = textNormalColor;
    auto borderColor = borderNormalColor;

    if (!isAcceptable_) {
        borderColor = borderErrorColor;
    }
    else if (this->isFocused()) {
        borderColor = borderFocusColor;
    }

    const auto marginLeftBottom = Vector2{static_cast<float>(textMargin_.left), static_cast<float>(textMargin_.bottom)};
    const auto textEditPos = math::toVector2(globalPos);
    const auto textPosition = textEditPos + marginLeftBottom + Vector2{0.0f, baselineOffset_};
    const auto innerBoundPos = textEditPos + marginLeftBottom;
    const auto innerBoundSize = Vector2{
        getWidth() - static_cast<float>(textMargin_.left + textMargin_.right),
        getHeight() - static_cast<float>(textMargin_.bottom + textMargin_.top)};

    // NOTE: Mask scissor
    drawingContext.pushScissorRect(Rect2D{
        static_cast<int>(innerBoundPos.x),
        static_cast<int>(innerBoundPos.y),
        static_cast<int>(innerBoundSize.x),
        static_cast<int>(innerBoundSize.y)});

    constexpr float cursorThickness = 1.0f;

    const auto calculatePositionInSprite = [&](int positionInText) -> float {
        if (text_.empty()) {
            return marginLeftBottom.x;
        }

        // FIXME: UTF-8
        POMDOG_ASSERT(positionInText >= 0);
        POMDOG_ASSERT(positionInText <= static_cast<int>(text_.size()));
        auto substring = text_.substr(0, positionInText);
        auto v = spriteFont_->measureString(substring);
        constexpr float offset = 0.2f;
        return marginLeftBottom.x + v.x + offset;
    };

    auto cursorDrawPosition = Vector2::createZero();
    if (cursorPosition_ != std::nullopt) {
        cursorDrawPosition.x = calculatePositionInSprite(*cursorPosition_);

        if (textStartPositionX_ == std::nullopt) {
            textStartPositionX_ = std::max(cursorDrawPosition.x - innerBoundSize.x, 0.0f);
        }
        if (*textStartPositionX_ > 0) {
            cursorDrawPosition.x = cursorDrawPosition.x - *textStartPositionX_;
        }
    }

    // NOTE: selection bounds rendering
    if ((startSelectionPosition_ != std::nullopt) &&
        (cursorPosition_ != std::nullopt) &&
        (*startSelectionPosition_ != *cursorPosition_)) {

        POMDOG_ASSERT(textStartPositionX_ != std::nullopt);
        float startSelectionPosX = calculatePositionInSprite(*startSelectionPosition_) - *textStartPositionX_;

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
            getHeight() - cursorHeightMargin * 2.0f - 0.5f,
            selectionBoundsColor);
    }

    // NOTE: cursor rendering
    if (!hasSelectedText() &&
        (cursorPosition_ != std::nullopt) &&
        ((cursorBlinkInterval_.count() / CursorBlinkInterval) <= 0.5)) {
        constexpr float cursorHeightMargin = 2.0f;

        primitiveBatch->drawRectangle(
            Matrix3x2::createIdentity(),
            cursorDrawPosition + Vector2{0.0f, cursorHeightMargin} + math::toVector2(globalPos),
            cursorThickness,
            getHeight() - cursorHeightMargin * 2.0f - 0.5f,
            cursorColor);
    }

    primitiveBatch->flush();

    if (!text_.empty()) {
        // NOTE: draw input text_
        auto spriteBatch = drawingContext.getSpriteBatch();
        auto startPos = Vector2::createZero();
        if (textStartPositionX_ != std::nullopt) {
            startPos.x = *textStartPositionX_;
        }
        spriteFont_->draw(
            *spriteBatch,
            text_,
            textPosition - startPos,
            textColor,
            0.0f,
            Vector2{0.0f, 0.0f},
            1.0f);

        drawingContext.flushSpriteBatch();
    }
    else if (!placeholderText_.empty()) {
        // NOTE: draw placeholder text_
        auto spriteBatch = drawingContext.getSpriteBatch();
        spriteFont_->draw(
            *spriteBatch,
            placeholderText_,
            textPosition,
            placeholderTextColor,
            0.0f,
            Vector2{0.0f, 0.0f},
            1.0f);

        drawingContext.flushSpriteBatch();
    }

    drawingContext.popScissorRect();

    if ((textStartPositionX_ != std::nullopt) &&
        (cursorPosition_ != std::nullopt) &&
        (cursorMoveInterval_ == Duration::zero())) {
        if (cursorDrawPosition.x < marginLeftBottom.x) {
            // NOTE: Recalculate the start position
            textStartPositionX_ = (*textStartPositionX_ + cursorDrawPosition.x - marginLeftBottom.x);
        }
        if (cursorDrawPosition.x > innerBoundSize.x) {
            // NOTE: Recalculate the start position
            textStartPositionX_ = (*textStartPositionX_ + (cursorDrawPosition.x - innerBoundSize.x));
        }
    }
}

} // namespace pomdog::gui

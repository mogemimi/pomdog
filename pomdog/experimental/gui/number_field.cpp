// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/number_field.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/text_edit.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
namespace {

constexpr int prependAreaWidth = 10;

} // namespace

NumberFieldDataContext::~NumberFieldDataContext() = default;

NumberField::NumberField(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    const std::shared_ptr<NumberFieldDataContext>& dataContextIn)
    : Widget(dispatcher)
    , dataContext_(dataContextIn)
    , prependAreaColor_(255, 255, 255, 0)
    , isPointerEntered_(false)
    , isDragging_(false)
{
    setSize(50, 18);
    setCursor(MouseCursor::ResizeHorizontal);

    textEdit_ = std::make_shared<TextEdit>(dispatcher);
    textEdit_->setTextMargin(Thickness{1, 4, 1, 4});
    textEdit_->setBaselineOffset(4.0f);
    textEdit_->setText(dataContext_->toString());

    textBlock_ = std::make_shared<TextBlock>(dispatcher);
    textBlock_->setBaselineOffset(4.0f);
    textBlock_->setTextAlignment(TextAlignment::Center);
    textBlock_->setColor(Color::createWhite());
    textBlock_->setText(dataContext_->toString());

    addButton_ = std::make_shared<PushButton>(dispatcher);
    addButton_->setSize(10, getHeight());
    addButton_->setText(">");

    subButton_ = std::make_shared<PushButton>(dispatcher);
    subButton_->setSize(10, getHeight());
    subButton_->setText("<");
}

void NumberField::setFontWeight(FontWeight fontWeightIn)
{
    textEdit_->setFontWeight(fontWeightIn);
}

void NumberField::setFontSize(FontSize fontSizeIn)
{
    textEdit_->setFontSize(fontSizeIn);
}

bool NumberField::isEnabled() const
{
    return textEdit_->isEnabled();
}

void NumberField::setEnabled(bool enabledIn)
{
    textEdit_->setEnabled(enabledIn);
}

bool NumberField::isReadOnly() const
{
    return textEdit_->isReadOnly();
}

void NumberField::setReadOnly(bool readOnlyIn)
{
    textEdit_->setReadOnly(readOnlyIn);
}

bool NumberField::isAcceptable() const
{
    return textEdit_->isAcceptable();
}

void NumberField::setAcceptable(bool acceptableIn)
{
    textEdit_->setAcceptable(acceptableIn);
}

void NumberField::setText(const std::string& text)
{
    textEdit_->setText(text);
    textBlock_->setText(text);
}

std::string NumberField::getPlaceholderText() const
{
    return textEdit_->getPlaceholderText();
}

void NumberField::setPlaceholderText(const std::string& textIn)
{
    textEdit_->setPlaceholderText(textIn);
}

bool NumberField::hasSelectedText() const
{
    return textEdit_->hasSelectedText();
}

void NumberField::deselect()
{
    textEdit_->deselect();
}

void NumberField::setPrependAreaColor(const std::optional<Color>& color)
{
    if (color != std::nullopt) {
        prependAreaColor_ = Color{color->r, color->g, color->b, 255};
    }
    else {
        prependAreaColor_ = Color{255, 255, 255, 0};
    }
}

void NumberField::setPrependAreaString(const std::string& text)
{
    prependAreaText_ = text;
}

void NumberField::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    textEdit_->setHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment NumberField::getHorizontalAlignment() const noexcept
{
    return textEdit_->getHorizontalAlignment();
}

VerticalAlignment NumberField::getVerticalAlignment() const noexcept
{
    return textEdit_->getVerticalAlignment();
}

void NumberField::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);

    POMDOG_ASSERT(textEdit_ != nullptr);
    POMDOG_ASSERT(textBlock_ != nullptr);
    POMDOG_ASSERT(addButton_ != nullptr);
    POMDOG_ASSERT(subButton_ != nullptr);
    textEdit_->markParentTransformDirty();
    textBlock_->markParentTransformDirty();
    addButton_->markParentTransformDirty();
    subButton_->markParentTransformDirty();
}

void NumberField::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(textEdit_ != nullptr);
    POMDOG_ASSERT(textBlock_ != nullptr);
    POMDOG_ASSERT(addButton_ != nullptr);
    POMDOG_ASSERT(subButton_ != nullptr);
    textEdit_->markParentTransformDirty();
    textBlock_->markParentTransformDirty();
    addButton_->markParentTransformDirty();
    subButton_->markParentTransformDirty();
}

void NumberField::onEnter()
{
    std::array<std::shared_ptr<Widget>, 4> children = {{
        textEdit_,
        textBlock_,
        addButton_,
        subButton_,
    }};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->onEnter();
        child->markParentTransformDirty();
        child->setParent(shared_from_this());
    }

    textSubmittedConn_ = textEdit_->TextSubmitted.connect([this] {
        auto text = textEdit_->getText();
        if (!dataContext_->textSubmitted(text)) {
            textEdit_->setText(dataContext_->toString());
            return;
        }

        textEdit_->setText(dataContext_->toString());
        textBlock_->setText(textEdit_->getText());

        this->ValueChanged();
    });

    focusOutConn_ = textEdit_->FocusOut.connect([this] {
        textEdit_->deselect();
        textEdit_->setVisible(false);
        textEdit_->setText(dataContext_->toString());
        textBlock_->setText(textEdit_->getText());
    });

    addButtonConn_ = addButton_->Click.connect([this] {
        dataContext_->incrementValue();
        textBlock_->setText(dataContext_->toString());
        ValueChanged();
    });

    subButtonConn_ = subButton_->Click.connect([this] {
        dataContext_->decrementValue();
        textBlock_->setText(dataContext_->toString());
        ValueChanged();
    });

    textEdit_->setVisible(false);
    addButton_->setVisible(false);
    subButton_->setVisible(false);
}

void NumberField::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPointerEntered_ = true;

    if (!textEdit_->isFocused()) {
        addButton_->setVisible(true);
        subButton_->setVisible(true);
    }
}

void NumberField::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPointerEntered_ = false;
}

void NumberField::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!textEdit_->isEnabled()) {
        return;
    }

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
    startDragPosition_ = pointInView;
    dataContext_->beginDragging();
}

void NumberField::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!textEdit_->isEnabled()) {
        return;
    }

    if (startDragPosition_ == std::nullopt) {
        return;
    }

    const auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
    const auto amount = (pointInView.x - startDragPosition_->x);

    if (pointInView != startDragPosition_) {
        isDragging_ = true;
    }

    if (isDragging_) {
        dataContext_->updateDragging(amount);
        textBlock_->setText(dataContext_->toString());
        ValueChanged();
    }
}

void NumberField::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!textEdit_->isEnabled()) {
        return;
    }

    if (!isDragging_) {
        textEdit_->setVisible(true);

        auto dispatcher = getDispatcher();
        dispatcher->setFocusWidget(textEdit_);

        // NOTE: Remove trailing zeros from string
        auto s = textBlock_->getText();
        textEdit_->setText(dataContext_->toEditableString(s));

        textEdit_->setSelection(0, static_cast<int>(textEdit_->getText().size()));

        addButton_->setVisible(false);
        subButton_->setVisible(false);
    }

    isDragging_ = false;
    startDragPosition_ = std::nullopt;
}

std::shared_ptr<Widget> NumberField::getChildAt(const Point2D& position)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{addButton_, subButton_, textEdit_}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);

        if (!child->isVisible()) {
            continue;
        }

        auto bounds = child->getBounds();
        if (bounds.contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void NumberField::updateAnimation(const Duration& frameDuration)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{addButton_, subButton_, textEdit_}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->updateAnimation(frameDuration);
    }
}

void NumberField::doLayout()
{
    int prependAreaOffset = 0;
    const bool hasPrependArea = (prependAreaColor_.a == 255);
    if (hasPrependArea) {
        prependAreaOffset = prependAreaWidth;
    }

    POMDOG_ASSERT(textEdit_ != nullptr);
    textEdit_->setSize(getWidth() - prependAreaOffset, getHeight());
    textEdit_->setPosition(Point2D{prependAreaOffset, 0});
    textEdit_->doLayout();

    POMDOG_ASSERT(textBlock_ != nullptr);
    auto margin = textEdit_->getTextMargin();
    textBlock_->setSize(getWidth() - prependAreaOffset, getHeight());
    textBlock_->setPosition(Point2D{prependAreaOffset, margin.bottom});
    textBlock_->doLayout();

    POMDOG_ASSERT(addButton_ != nullptr);
    addButton_->setPosition(Point2D{getWidth() - addButton_->getWidth(), 0});
    addButton_->doLayout();

    POMDOG_ASSERT(subButton_ != nullptr);
    subButton_->setPosition(Point2D{prependAreaOffset, 0});
    subButton_->doLayout();
}

void NumberField::draw(DrawingContext& drawingContext)
{
    if (!isPointerEntered_ && !addButton_->isHovered() && !subButton_->isHovered()) {
        addButton_->setVisible(false);
        subButton_->setVisible(false);
    }

    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    auto fieldColor = colorScheme->TextFieldRectColorBase;
    auto borderColor = colorScheme->TextFieldBorderColorBase;

    if (!textEdit_->isAcceptable()) {
        borderColor = colorScheme->TextFieldBorderColorError;
    }
    else if (textEdit_->isFocused()) {
        borderColor = colorScheme->TextFieldBorderColorFocus;
    }

    // NOTE: draw box border
    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
        borderColor);

    // NOTE: draw text field box
    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x + 1, globalPos.y + 1, getWidth() - 2, getHeight() - 2},
        fieldColor);

    const bool hasPrependArea = (prependAreaColor_.a == 255);
    if (hasPrependArea) {
        // NOTE: draw prepend area box
        primitiveBatch->drawRectangle(
            Rect2D{globalPos.x + 1, globalPos.y + 1, prependAreaWidth, getHeight() - 2},
            prependAreaColor_);
    }

    drawingContext.flushPrimitiveBatch();

    if (hasPrependArea && !prependAreaText_.empty()) {
        auto spriteBatch = drawingContext.getSpriteBatch();
        auto spriteFont = drawingContext.getFont(FontWeight::Bold, FontSize::Small);

        spriteFont->draw(
            *spriteBatch,
            prependAreaText_,
            math::toVector2(globalPos + Point2D{prependAreaWidth / 2, getHeight() / 2}) + Vector2{1.0f, 2.5f},
            Color{231, 229, 230, 255},
            0.0f,
            Vector2{0.5f, 0.5f},
            0.9f);

        drawingContext.flushSpriteBatch();
    }

    drawingContext.pushTransform(globalPos);

    POMDOG_ASSERT(textEdit_ != nullptr);
    if (textEdit_->isVisible()) {
        textEdit_->draw(drawingContext);
    }
    else {
        POMDOG_ASSERT(textBlock_ != nullptr);
        textBlock_->draw(drawingContext);

        drawingContext.flushSpriteBatch();
    }

    POMDOG_ASSERT(addButton_ != nullptr);
    if (addButton_->isVisible()) {
        addButton_->draw(drawingContext);
    }

    POMDOG_ASSERT(subButton_ != nullptr);
    if (subButton_->isVisible()) {
        subButton_->draw(drawingContext);
    }

    drawingContext.popTransform();
}

} // namespace pomdog::gui

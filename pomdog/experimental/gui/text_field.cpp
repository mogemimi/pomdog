// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/text_field.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/text_edit.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

TextField::TextField(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    setSize(50, 22);

    textEdit_ = std::make_shared<TextEdit>(dispatcher);
    textEdit_->setSize(getWidth(), getHeight());
    textEdit_->setBaselineOffset(3.0f);
}

void TextField::setFontWeight(FontWeight fontWeightIn)
{
    textEdit_->setFontWeight(fontWeightIn);
}

void TextField::setFontSize(FontSize fontSizeIn)
{
    textEdit_->setFontSize(fontSizeIn);
}

bool TextField::isEnabled() const
{
    return textEdit_->isEnabled();
}

void TextField::setEnabled(bool enabledIn)
{
    textEdit_->setEnabled(enabledIn);
}

bool TextField::isReadOnly() const
{
    return textEdit_->isReadOnly();
}

void TextField::setReadOnly(bool readOnlyIn)
{
    textEdit_->setReadOnly(readOnlyIn);
}

bool TextField::isAcceptable() const
{
    return textEdit_->isAcceptable();
}

void TextField::setAcceptable(bool acceptableIn)
{
    textEdit_->setAcceptable(acceptableIn);
}

std::string TextField::getText() const
{
    return textEdit_->getText();
}

void TextField::setText(const std::string& textIn)
{
    textEdit_->setText(textIn);
}

std::string TextField::getPlaceholderText() const
{
    return textEdit_->getPlaceholderText();
}

void TextField::setPlaceholderText(const std::string& textIn)
{
    textEdit_->setPlaceholderText(textIn);
}

bool TextField::hasSelectedText() const
{
    return textEdit_->hasSelectedText();
}

void TextField::deselect()
{
    textEdit_->deselect();
}

void TextField::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    textEdit_->setHorizontalAlignment(horizontalAlignmentIn);
}

HorizontalAlignment TextField::getHorizontalAlignment() const noexcept
{
    return textEdit_->getHorizontalAlignment();
}

VerticalAlignment TextField::getVerticalAlignment() const noexcept
{
    return textEdit_->getVerticalAlignment();
}

void TextField::setPosition(const Point2D& positionIn)
{
    Widget::setPosition(positionIn);
    POMDOG_ASSERT(textEdit_ != nullptr);
    textEdit_->markParentTransformDirty();
}

void TextField::markParentTransformDirty()
{
    Widget::markParentTransformDirty();

    POMDOG_ASSERT(textEdit_ != nullptr);
    textEdit_->markParentTransformDirty();
}

std::shared_ptr<Widget> TextField::getChildAt(const Point2D& position)
{
    if (textEdit_ != nullptr) {
        const auto bounds = textEdit_->getBounds();
        if (bounds.contains(position)) {
            return textEdit_;
        }
    }
    return nullptr;
}

void TextField::updateAnimation(const Duration& frameDuration)
{
    if (textEdit_ != nullptr) {
        textEdit_->updateAnimation(frameDuration);
    }
}

void TextField::onEnter()
{
    POMDOG_ASSERT(textEdit_ != nullptr);
    textEdit_->markParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    textEdit_->setParent(shared_from_this());
    textEdit_->onEnter();

    textChangedConn_ = textEdit_->TextChanged.connect([this] { this->TextChanged(); });
}

void TextField::doLayout()
{
    POMDOG_ASSERT(textEdit_ != nullptr);
    textEdit_->setSize(getWidth(), getHeight());
    textEdit_->doLayout();
}

void TextField::draw(DrawingContext& drawingContext)
{
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

    primitiveBatch->flush();

    drawingContext.pushTransform(globalPos);

    POMDOG_ASSERT(textEdit_ != nullptr);
    textEdit_->draw(drawingContext);

    drawingContext.popTransform();
}

} // namespace pomdog::gui

// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/ScrollView.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Experimental/GUI/VerticalScrollBar.hpp"
#include "Pomdog/Math/Math.hpp"

namespace Pomdog::GUI {

ScrollView::ScrollView(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , margin{0, 0, 0, 0}
    , backgroundColor(Color::TransparentBlack)
    , horizontalAlignment(HorizontalAlignment::Stretch)
    , needToUpdateLayout(true)
{
    SetSize(widthIn, heightIn);
    SetWheelFocusEnabled(true);

    scrollBar = std::make_shared<VerticalScrollBar>(dispatcher, 0.0, 100.0);
    scrollBar->SetMinimum(0.0);
    scrollBar->SetMaximum(100.0);
    scrollBar->SetPageStep(30.0);
    scrollBar->SetValue(0.0);
}

void ScrollView::SetMargin(const Thickness& marginIn)
{
    margin = marginIn;
    needToUpdateLayout = true;
}

void ScrollView::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    if (scrollBar != nullptr) {
        scrollBar->MarkParentTransformDirty();
    }
    if (child != nullptr) {
        child->MarkParentTransformDirty();
    }
}

void ScrollView::SetBackgroundColor(const Color& color) noexcept
{
    backgroundColor = color;
}

void ScrollView::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    if (scrollBar != nullptr) {
        scrollBar->MarkParentTransformDirty();
    }
    if (child != nullptr) {
        child->MarkParentTransformDirty();
    }
}

void ScrollView::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool ScrollView::GetSizeToFitContent() const noexcept
{
    return false;
}

void ScrollView::ScrollToTop()
{
    if (child == nullptr) {
        return;
    }

    child->DoLayout();

    if (child->GetHeight() > GetHeight()) {
        scrollBar->SetValue(scrollBar->GetMaximum());
        needToUpdateLayout = true;
    }
}

void ScrollView::ScrollToEnd()
{
    if (child == nullptr) {
        return;
    }

    child->DoLayout();

    if (child->GetHeight() > GetHeight()) {
        scrollBar->SetValue(scrollBar->GetMinimum());
        needToUpdateLayout = true;
    }
}

void ScrollView::ScrollTo(const std::shared_ptr<Widget>& widget)
{
    if (child == nullptr) {
        return;
    }
    if (widget == nullptr) {
        return;
    }

    child->DoLayout();

    if (child->GetHeight() > GetHeight()) {
        const auto w = widget->GetGlobalPosition();
        const auto s = scrollBar->GetGlobalPosition();
        const auto h = scrollBar->GetHeight();
        const auto v = w.Y - (s.Y + h);
        scrollBar->SetValue(std::clamp(static_cast<double>(v), scrollBar->GetMinimum(), scrollBar->GetMaximum()));
        needToUpdateLayout = true;
    }
}

void ScrollView::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    this->horizontalAlignment = horizontalAlignmentIn;
}

HorizontalAlignment ScrollView::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

void ScrollView::OnEnter()
{
    POMDOG_ASSERT(scrollBar != nullptr);
    POMDOG_ASSERT(shared_from_this());
    scrollBar->SetParent(shared_from_this());
    scrollBar->MarkParentTransformDirty();
    scrollBar->OnEnter();
}

void ScrollView::OnPointerWheelChanged(const PointerPoint& pointerPoint)
{
#if WIN32
    // FIXME: Set to appropriate wheel scroll speed for each platform.
    constexpr int divisor = 400;
#else
    // NOTE: The answer to life, universe and everything.
    constexpr int divisor = 42;
#endif
    const auto wheelSpeed = scrollBar->GetPageStep() / divisor;

    const auto v = scrollBar->GetValue() + wheelSpeed * pointerPoint.MouseWheelDelta;
    scrollBar->SetValue(std::clamp(v, scrollBar->GetMinimum(), scrollBar->GetMaximum()));

    needToUpdateLayout = true;
}

void ScrollView::SetWidget(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);
    POMDOG_ASSERT(!widget->GetParent());

    child = widget;
    child->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    child->SetParent(shared_from_this());
    child->OnEnter();

    if (child->GetHeight() <= GetHeight()) {
        scrollBar->SetEnabled(false);
        scrollBar->SetVisible(false);
    }
    else {
        scrollBar->SetEnabled(true);
        scrollBar->SetVisible(true);

        scrollBar->SetMinimum(0.0);
        scrollBar->SetMaximum(static_cast<double>(child->GetHeight() - GetHeight()));
        scrollBar->SetPageStep(static_cast<double>(GetHeight()));
        scrollBar->SetValue(scrollBar->GetMaximum());
    }

    needToUpdateLayout = true;
}

std::shared_ptr<Widget> ScrollView::GetChildAt(const Point2D& position)
{
    if (scrollBar != nullptr) {
        auto bounds = scrollBar->GetBounds();
        if (bounds.Contains(position)) {
            return scrollBar;
        }
    }

    auto clipBounds = Rectangle{0, 0, GetWidth(), GetHeight()};
    if (!clipBounds.Contains(position)) {
        return nullptr;
    }

    if (child != nullptr) {
        auto bounds = child->GetBounds();
        if (bounds.Contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void ScrollView::UpdateAnimation(const Duration& frameDuration)
{
    if (scrollBar != nullptr) {
        scrollBar->UpdateAnimation(frameDuration);
    }
    if (child != nullptr) {
        child->UpdateAnimation(frameDuration);
    }
}

void ScrollView::UpdateLayout()
{
    if (scrollBar != nullptr) {
        scrollBar->DoLayout();
    }
    if (child != nullptr) {
        child->DoLayout();
    }

    if (!needToUpdateLayout) {
        return;
    }
    needToUpdateLayout = false;

    if (child == nullptr) {
        // NOTE: Resizing this panel
        SetSize(GetWidth(), 0);

        if (auto parent = GetParent(); parent != nullptr) {
            parent->MarkContentLayoutDirty();
        }
        return;
    }

    POMDOG_ASSERT(child != nullptr);

    if (auto parent = GetParent(); parent != nullptr) {
        parent->MarkContentLayoutDirty();
    }

    scrollBar->SetSize(scrollBar->GetWidth(), GetHeight());
    scrollBar->SetPosition(Point2D{GetWidth() - scrollBar->GetWidth(), 0});
    scrollBar->SetMinimum(0.0);
    scrollBar->SetPageStep(static_cast<double>(GetHeight()));
    scrollBar->MarkContentLayoutDirty();

    // NOTE: Update layout for child
    if (child != nullptr) {
        POMDOG_ASSERT(scrollBar != nullptr);

        const auto scrollBarVisible = child->GetHeight() > GetHeight();

        if (scrollBarVisible) {
            child->SetSize(GetWidth() - scrollBar->GetWidth(), child->GetHeight());
            child->SetPosition(Point2D{0, static_cast<int>(-scrollBar->GetValue())});
        }
        else {
            child->SetSize(GetWidth(), child->GetHeight());
            child->SetPosition(Point2D{0, GetHeight() - child->GetHeight()});
        }

        child->MarkContentLayoutDirty();
        child->DoLayout();

        if (!scrollBarVisible) {
            scrollBar->SetEnabled(false);
            scrollBar->SetVisible(false);
            SetWheelFocusEnabled(false);
        }
        else {
            scrollBar->SetEnabled(true);
            scrollBar->SetVisible(true);
            SetWheelFocusEnabled(true);

            const auto oldMaxValue = scrollBar->GetMaximum();
            scrollBar->SetMaximum(static_cast<double>(child->GetHeight() - GetHeight()));
            scrollBar->SetPageStep(static_cast<double>(GetHeight()));

            if (oldMaxValue < scrollBar->GetMaximum()) {
                scrollBar->SetValue(scrollBar->GetMaximum() - (oldMaxValue - scrollBar->GetValue()));
            }
            else if (oldMaxValue > scrollBar->GetMaximum()) {
                scrollBar->SetValue(std::min(scrollBar->GetValue(), scrollBar->GetMaximum()));
            }
        }
    }
    else {
        scrollBar->SetMaximum(static_cast<double>(GetHeight()));
    }

    // NOTE: Update layout for scroll bar
    scrollBar->DoLayout();
}

void ScrollView::DoLayout()
{
    UpdateLayout();
}

void ScrollView::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    if (backgroundColor.A > 0) {
        auto primitiveBatch = drawingContext.GetPrimitiveBatch();

        primitiveBatch->DrawRectangle(
            Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
            backgroundColor);

        primitiveBatch->Flush();
    }

    auto innerBoundPos = Math::ToVector2(globalPos);

    // NOTE: Mask scissor
    drawingContext.PushScissorRect(Rectangle{
        static_cast<int>(innerBoundPos.X),
        static_cast<int>(innerBoundPos.Y),
        GetWidth(),
        GetHeight()});

    drawingContext.PushTransform(globalPos);

    if (child != nullptr) {
        child->Draw(drawingContext);
    }

    drawingContext.PopScissorRect();

    if ((scrollBar != nullptr) && scrollBar->IsVisible()) {
        scrollBar->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI

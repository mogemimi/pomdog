// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/horizontal_splitter.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

class HorizontalSplitterHandle final
    : public Widget
    , public std::enable_shared_from_this<HorizontalSplitterHandle> {
public:
    explicit HorizontalSplitterHandle(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const noexcept;
    void SetEnabled(bool enabled) noexcept;

    bool IsVisible() const noexcept;
    void SetVisible(bool visible) noexcept;

    void SetColor(const Color&) noexcept;

    void SetBorderWidth(int width) noexcept;

    int GetBorderWidth() const noexcept;

    int GetBorderWidthOffset() const noexcept;

    Point2D GetBorderPosition() const noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void(int)> Resizing;

private:
    std::optional<Point2D> grabStartPosition;
    Color color;
    int borderWidth = 1;
    bool isEnabled;
    bool isVisible;
};

HorizontalSplitter::HorizontalSplitter(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , margin{0, 0, 0, 0}
    , backgroundColor(Color::TransparentBlack())
    , needToUpdateLayout(true)
{
    SetSize(widthIn, heightIn);
    SetInteractable(false);

    splitterHandle = std::make_shared<HorizontalSplitterHandle>(dispatcher);
    splitterHandle->SetBorderWidth(1);
    splitterHandle->SetSize(5, GetHeight());
    splitterHandle->SetPosition(Point2D{GetWidth() / 2, 0});
}

void HorizontalSplitter::SetMargin(const Thickness& marginIn)
{
    margin = marginIn;
    needToUpdateLayout = true;
}

void HorizontalSplitter::SetLayoutSpacing(int spacing)
{
    POMDOG_ASSERT(splitterHandle);
    POMDOG_ASSERT(spacing >= 0);
    spacing = std::max(spacing, 0);

    constexpr int handleCollisionOffset = 4;
    const auto minimumCollisionWidth = handleCollisionOffset + (spacing % 2);

    splitterHandle->SetBorderWidth(spacing);
    splitterHandle->SetSize(std::max(spacing, minimumCollisionWidth), GetHeight());
    if (childSplitter) {
        childSplitter->SetLayoutSpacing(spacing);
    }
    needToUpdateLayout = true;
}

void HorizontalSplitter::SetBackgroundColor(const Color& color)
{
    backgroundColor = color;
    if (childSplitter) {
        childSplitter->SetBackgroundColor(color);
    }
}

void HorizontalSplitter::SetHandleColor(const Color& color)
{
    POMDOG_ASSERT(splitterHandle);
    splitterHandle->SetColor(color);
    if (childSplitter) {
        childSplitter->SetHandleColor(color);
    }
}

void HorizontalSplitter::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    for (auto& child : children) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->MarkParentTransformDirty();
    }
}

void HorizontalSplitter::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();
    for (auto& child : children) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->MarkParentTransformDirty();
    }
}

void HorizontalSplitter::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool HorizontalSplitter::GetSizeToFitContent() const noexcept
{
    return true;
}

void HorizontalSplitter::OnEnter()
{
    splitterHandle->SetParent(shared_from_this());
    splitterHandle->OnEnter();

    resizingConn = splitterHandle->Resizing.Connect([this](int offset) {
        auto pos = splitterHandle->GetPosition();
        if (offset <= 0) {
            auto& left = children[0];
            pos.X = std::max(pos.X + offset, left.minimumWidth);
        }
        else {
            auto& right = children[1];
            pos.X = std::min(
                pos.X + offset,
                GetWidth() - (right.minimumWidth + splitterHandle->GetBorderWidth()));
        }

        splitterHandle->SetPosition(pos);
        needToUpdateLayout = true;
    });
}

void HorizontalSplitter::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget->GetParent() == nullptr);
    POMDOG_ASSERT(splitterHandle);

    Item item;
    item.widget = widget;
    item.minimumWidth = 0;

    if (children[0].widget == nullptr) {
        children[0] = std::move(item);

        const auto offset = splitterHandle->GetBorderWidthOffset();
        splitterHandle->SetPosition(Point2D{widget->GetWidth() - offset, 0});
    }
    else if (children[1].widget == nullptr) {
        children[1] = std::move(item);
    }
    else if (childSplitter == nullptr) {
        UpdateLayout();

        auto& right = children[1];
        auto dispatcher = GetDispatcher();
        auto w = std::max(right.widget->GetWidth(), right.minimumWidth);
        auto h = GetHeight();
        childSplitter = std::make_shared<HorizontalSplitter>(dispatcher, w, h);
        childSplitter->SetParent(shared_from_this());
        childSplitter->OnEnter();

        right.widget->SetPosition(Point2D{0, 0});
        right.widget->SetParent(childSplitter);
        right.widget->MarkParentTransformDirty();

        childSplitter->children[0].widget = right.widget;
        childSplitter->children[0].minimumWidth = right.minimumWidth;
        childSplitter->splitterHandle->SetPosition(Point2D{childSplitter->GetWidth() / 2, 0});
        childSplitter->splitterHandle->SetSize(splitterHandle->GetWidth(), GetHeight());
        childSplitter->backgroundColor = backgroundColor;

        childSplitter->AddChild(widget);

        right.widget = childSplitter;
        needToUpdateLayout = true;
        return;
    }
    else {
        POMDOG_ASSERT(children[0].widget != nullptr);
        POMDOG_ASSERT(children[1].widget != nullptr);
        POMDOG_ASSERT(childSplitter != nullptr);
        childSplitter->AddChild(widget);
        needToUpdateLayout = true;
        return;
    }

    widget->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    widget->SetParent(shared_from_this());
    widget->OnEnter();

    needToUpdateLayout = true;
}

std::shared_ptr<Widget> HorizontalSplitter::GetChildAt(const Point2D& position)
{
    {
        auto bounds = splitterHandle->GetBounds();
        if (bounds.Contains(position)) {
            return splitterHandle;
        }
    }

    for (auto& child : children) {
        if (child.widget == nullptr) {
            continue;
        }
        auto bounds = child.widget->GetBounds();
        if (bounds.Contains(position)) {
            return child.widget;
        }
    }
    return nullptr;
}

void HorizontalSplitter::UpdateAnimation(const Duration& frameDuration)
{
    for (auto& child : children) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->UpdateAnimation(frameDuration);
    }
}

void HorizontalSplitter::SetMinimumWidth(int index, int minimumWidth)
{
    POMDOG_ASSERT(minimumWidth >= 0);
    POMDOG_ASSERT(!children.empty());

    if (index < 0) {
        return;
    }
    const auto count = static_cast<int>(children.size());
    if (index < count) {
        auto& child = children[static_cast<size_t>(index)];
        child.minimumWidth = minimumWidth;
    }
    else if (childSplitter != nullptr) {
        POMDOG_ASSERT(index >= static_cast<int>(children.size()));
        childSplitter->SetMinimumWidth(index - count, minimumWidth);
    }
}

void HorizontalSplitter::SetMinimumWidth(const std::shared_ptr<Widget>& widget, int minimumWidth)
{
    POMDOG_ASSERT(minimumWidth >= 0);
    POMDOG_ASSERT(!children.empty());

    if (widget->GetParent() != shared_from_this()) {
        if (childSplitter != nullptr) {
            childSplitter->SetMinimumWidth(widget, minimumWidth);
            auto& right = children[1];
            right.minimumWidth = splitterHandle->GetBorderWidth();
            for (auto& child : childSplitter->children) {
                right.minimumWidth += child.minimumWidth;
            }
        }
        return;
    }

    POMDOG_ASSERT(widget->GetParent() == shared_from_this());

    auto iter = std::find_if(
        std::begin(children),
        std::end(children),
        [&](const auto& c) -> bool { return c.widget == widget; });

    if (iter != std::end(children)) {
        iter->minimumWidth = minimumWidth;
    }
}

void HorizontalSplitter::UpdateLayout()
{
    if (childSplitter != nullptr) {
        childSplitter->SetSize(childSplitter->GetWidth(), GetHeight());
    }

    for (auto& child : children) {
        if (child.widget != nullptr) {
            child.widget->DoLayout();
        }
    }

    if (!needToUpdateLayout) {
        return;
    }

    {
        int maxHeight = 0;
        for (auto& child : children) {
            if (child.widget == nullptr) {
                continue;
            }
            if (child.widget->GetVerticalAlignment() == VerticalAlignment::Stretch) {
                continue;
            }
            if (child.widget->GetSizeToFitContent()) {
                continue;
            }
            maxHeight = std::max(child.widget->GetHeight(), maxHeight);
        }

        const auto requiredHeight = maxHeight + margin.Top + margin.Bottom;
        if (requiredHeight > GetHeight()) {
            // NOTE: Resizing this panel
            SetSize(GetWidth(), requiredHeight);

            auto parent = GetParent();
            if (parent) {
                parent->MarkContentLayoutDirty();
            }
        }
    }

    {
        auto position = splitterHandle->GetPosition();
        position.X = std::clamp(
            position.X,
            0,
            GetWidth() - (children[1].minimumWidth + splitterHandle->GetBorderWidth()));
        splitterHandle->SetPosition(position);
        splitterHandle->SetSize(splitterHandle->GetWidth(), GetHeight());
    }

    POMDOG_ASSERT(!children.empty());

    int offsetX = margin.Left;

    // NOTE: Update layout for children
    for (int i = 0; i < static_cast<int>(children.size()); i++) {
        auto& child = children[i];
        if (child.widget == nullptr) {
            break;
        }

        auto handle = splitterHandle;
        if (i > 0) {
            handle = nullptr;
        }

        auto position = Point2D{offsetX, 0};
        auto requiredWidth = [&]() -> int {
            if (handle == nullptr) {
                return GetWidth() - offsetX;
            }
            auto handlePosition = handle->GetBorderPosition();
            return std::max(child.minimumWidth, handlePosition.X - position.X);
        }();

        switch (child.widget->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto h = child.widget->GetHeight();
            if (child.widget->GetVerticalAlignment() == VerticalAlignment::Stretch) {
                h = GetHeight() - (margin.Bottom + margin.Top);
            }
            child.widget->SetSize(requiredWidth, h);
            child.widget->MarkContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Right:
            position.X = offsetX + requiredWidth - child.widget->GetWidth();
            break;
        case HorizontalAlignment::Left:
            break;
        }

        switch (child.widget->GetVerticalAlignment()) {
        case VerticalAlignment::Stretch: {
            position.Y = margin.Bottom;
            break;
        }
        case VerticalAlignment::Top:
            position.Y = GetHeight() - child.widget->GetHeight() - margin.Top;
            break;
            // case VerticalAlignment::Center:
            //     position.Y = (GetHeight() - child.widget->GetHeight()) / 2;
            //     break;
        }

        child.widget->SetPosition(position);

        if (handle != nullptr) {
            auto handlePosition = handle->GetPosition();
            offsetX = handlePosition.X + handle->GetBorderWidth();
        }
    }

    needToUpdateLayout = false;
}

void HorizontalSplitter::DoLayout()
{
    UpdateLayout();
}

void HorizontalSplitter::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    if (backgroundColor.A > 0) {
        auto primitiveBatch = drawingContext.GetPrimitiveBatch();

        primitiveBatch->DrawRectangle(
            Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
            backgroundColor);

        primitiveBatch->Flush();
    }

    drawingContext.PushTransform(globalPos);

    for (auto& child : children) {
        if (child.widget == nullptr) {
            continue;
        }
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->Draw(drawingContext);
    }

    splitterHandle->Draw(drawingContext);

    drawingContext.PopTransform();
}

HorizontalSplitterHandle::HorizontalSplitterHandle(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , color(Color{45, 45, 48, 255})
    , borderWidth(1)
    , isEnabled(true)
    , isVisible(true)
{
    SetSize(10, 1);
    SetCursor(MouseCursor::ResizeHorizontal);
}

bool HorizontalSplitterHandle::IsEnabled() const noexcept
{
    return isEnabled;
}

void HorizontalSplitterHandle::SetEnabled(bool enabledIn) noexcept
{
    isEnabled = enabledIn;
}

bool HorizontalSplitterHandle::IsVisible() const noexcept
{
    return isVisible;
}

void HorizontalSplitterHandle::SetVisible(bool visibleIn) noexcept
{
    isVisible = visibleIn;
}

void HorizontalSplitterHandle::SetColor(const Color& colorIn) noexcept
{
    color = colorIn;
}

void HorizontalSplitterHandle::SetBorderWidth(int widthIn) noexcept
{
    POMDOG_ASSERT(widthIn >= 0);
    borderWidth = widthIn;

    if (borderWidth > GetWidth()) {
        SetSize(borderWidth, GetHeight());
    }
}

int HorizontalSplitterHandle::GetBorderWidth() const noexcept
{
    return borderWidth;
}

int HorizontalSplitterHandle::GetBorderWidthOffset() const noexcept
{
    return std::max((GetWidth() - borderWidth) / 2, 0);
}

Point2D HorizontalSplitterHandle::GetBorderPosition() const noexcept
{
    auto pos = GetPosition();
    pos.X = pos.X + GetBorderWidthOffset();
    return pos;
}

HorizontalAlignment HorizontalSplitterHandle::GetHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment HorizontalSplitterHandle::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void HorizontalSplitterHandle::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
    grabStartPosition = pointInView;
}

void HorizontalSplitterHandle::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (grabStartPosition) {
        auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
        Resizing(pointInView.X - grabStartPosition->X);
    }
}

void HorizontalSplitterHandle::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    grabStartPosition = std::nullopt;
}

void HorizontalSplitterHandle::Draw(DrawingContext& drawingContext)
{
    if (!isVisible) {
        return;
    }

    auto globalPos = UIHelper::ProjectToWorldSpace(GetBorderPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X, globalPos.Y, borderWidth, GetHeight()},
        color);

    primitiveBatch->Flush();
}

} // namespace pomdog::gui

// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/ScenePanel.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Math/Math.hpp"
#include <cmath>

namespace Pomdog::GUI {
namespace {
//
//Radian<float> SampleTumbleGesture(const Vector2& position, Vector2 & startPosition, const Rectangle& viewportSize)
//{
//    constexpr float threshold = 1.0f;
//
//    Radian<float> delta = 0.0f;
//
//    auto const distance = Vector2::Distance(startPosition, position);
//
//    if (threshold < distance)
//    {
//        auto const centerPoint = viewportSize.GetCenter();
//        Vector2 const screenOffset(centerPoint.X, centerPoint.Y);
//
//        auto vecInScreen1 = (startPosition - screenOffset);
//        auto vecInScreen2 = (position - screenOffset);
//        auto const cross = Vector2::Cross(vecInScreen2, vecInScreen1);
//
//        if (std::abs(cross) > 1.0f)
//        {
//            auto const cosAngle = Vector2::Dot(vecInScreen1, vecInScreen2)/(vecInScreen1.Length() * vecInScreen2.Length());
//            delta = std::acos(Math::Clamp(cosAngle, -1.0f, 1.0f)) * (cross > 0.0f ? 1.0f: -1.0f);
//            POMDOG_ASSERT(delta != std::numeric_limits<float>::signaling_NaN());
//
//            if ((vecInScreen1.Length() < 24.0f || vecInScreen2.Length() < 24.0f)) {
//                delta *= std::min(std::abs(cross/vecInScreen2.Length())*0.01f, 1.0f);
//            }
//
//            startPosition = std::move(position);
//        }
//    }
//
//    return delta;
//}
//
//Vector2 SampleTrackGesture(const Vector2& position, Vector2 & startPosition)
//{
//    constexpr float threshold = 2.0f;
//
//    Vector2 delta = Vector2::Zero;
//
//    if (threshold < Vector2::Distance(startPosition, position)) {
//        delta = position - startPosition;
//        startPosition = position;
//    }
//
//    return delta;
//}

static const auto ZoomAnimationInterval = std::chrono::milliseconds(400);

} // namespace

ScenePanel::ScenePanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , timer(Duration::zero())
    , normalizedScrollDirection(0.0f)
    , scrollAcceleration(0.0f)
    , cameraZoom(1.0)
    , isFocused(false)
    , isEnabled(true)
{
    SetSize(widthIn, heightIn);
    SetHierarchySortOrder(HierarchySortOrder::Back);
}

bool ScenePanel::IsEnabled() const
{
    return isEnabled;
}

void ScenePanel::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
}

bool ScenePanel::IsFocused() const
{
    return this->isFocused;
}

bool ScenePanel::GetSizeToFitContent() const noexcept
{
    return true;
}

Vector2 ScenePanel::ConvertToPanelSpace(const Point2D& point) const
{
    return Vector2(static_cast<float>(point.X), static_cast<float>(GetHeight() - point.Y));
}

void ScenePanel::OnEnter()
{
}

void ScenePanel::OnPointerWheelChanged(const PointerPoint& pointerPoint)
{
    scrollWheelSampler.AddWheelDelta(pointerPoint.MouseWheelDelta);
    auto wheelDeltaUnit = scrollWheelSampler.GetScrollWheelDeltaAverage();

    constexpr float accel = 1.2f;

    if (timer <= Duration::zero()) {
        timer = ZoomAnimationInterval;
        scrollAcceleration = accel;
    }
    else {
        scrollAcceleration += std::abs(pointerPoint.MouseWheelDelta / wheelDeltaUnit);
    }

    if (pointerPoint.MouseWheelDelta != 0) {
        auto direction = (pointerPoint.MouseWheelDelta >= 0 ? 1.0f : -1.0f);

        if (direction != normalizedScrollDirection) {
            normalizedScrollDirection = direction;
            timer = ZoomAnimationInterval;
            scrollAcceleration = accel;
        }
    }
}

void ScenePanel::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isFocused = true;
}

void ScenePanel::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isFocused = false;
}

void ScenePanel::OnPointerPressed(const PointerPoint& pointerPoint)
{
    POMDOG_ASSERT(pointerPoint.MouseEvent);

    switch (*pointerPoint.MouseEvent) {
    case PointerMouseEvent::LeftButtonPressed:
        OnMouseLeftButtonPressed(pointerPoint);
        break;
    case PointerMouseEvent::MiddleButtonPressed:
        OnMouseMiddleButtonPressed(pointerPoint);
        break;
    case PointerMouseEvent::RightButtonPressed:
        OnMouseRightButtonPressed(pointerPoint);
        break;
    default:
        break;
    }
}

void ScenePanel::OnPointerMoved(const PointerPoint& pointerPoint)
{
    POMDOG_ASSERT(pointerPoint.MouseEvent);

    switch (*pointerPoint.MouseEvent) {
    case PointerMouseEvent::LeftButtonPressed:
        OnMouseLeftButtonMoved(pointerPoint);
        break;
    case PointerMouseEvent::MiddleButtonPressed:
        OnMouseMiddleButtonMoved(pointerPoint);
        break;
    case PointerMouseEvent::RightButtonPressed:
        OnMouseRightButtonMoved(pointerPoint);
        break;
    default:
        break;
    }
}

void ScenePanel::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    tumbleStartPosition = std::nullopt;
    trackStartPosition = std::nullopt;
}

void ScenePanel::OnMouseLeftButtonPressed(const PointerPoint& pointerPoint)
{
    if (!tumbleStartPosition) {
        tumbleStartPosition = ConvertToPanelSpace(pointerPoint.Position);
    }
}

void ScenePanel::OnMouseLeftButtonMoved([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }

    if (!tumbleStartPosition) {
        return;
    }

    //    POMDOG_ASSERT(cameraObject);
    //    auto transform = cameraObject.Component<Transform2D>();
    //    auto camera = cameraObject.Component<Camera2D>();
    //
    //    if (!transform || !camera) {
    //        return;
    //    }
    //
    //    {
    //        // Tumble Gesture
    //        transform->Rotation += SampleTumbleGesture(ConvertToPanelSpace(pointerPoint.Position), *tumbleStartPosition,
    //            Rectangle{0, 0, Width(), Height()});
    //        if (transform->Rotation > Math::TwoPi<float>) {
    //            transform->Rotation -= Math::TwoPi<float>;
    //        }
    //        else if (transform->Rotation < -Math::TwoPi<float>) {
    //            transform->Rotation += Math::TwoPi<float>;
    //        }
    //        POMDOG_ASSERT(transform->Rotation != std::numeric_limits<float>::quiet_NaN());
    //        POMDOG_ASSERT(transform->Rotation != std::numeric_limits<float>::infinity());
    //    }
}

void ScenePanel::OnMouseMiddleButtonPressed(const PointerPoint& pointerPoint)
{
    trackStartPosition = ConvertToPanelSpace(pointerPoint.Position);
}

void ScenePanel::OnMouseMiddleButtonMoved([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }

    if (!trackStartPosition) {
        return;
    }

    //    POMDOG_ASSERT(cameraObject);
    //    auto transform = cameraObject.Component<Transform2D>();
    //    auto camera = cameraObject.Component<Camera2D>();
    //
    //    if (!transform || !camera) {
    //        return;
    //    }
    //
    //    // Track Gesture
    //    auto delta = SampleTrackGesture(ConvertToPanelSpace(pointerPoint.Position), *trackStartPosition);
    //
    //    POMDOG_ASSERT(camera->Zoom > 0);
    //    auto matrix = (Matrix3x3::CreateTranslation(delta)
    //        * Matrix3x3::CreateScale(1.0f / camera->Zoom)
    //        * Matrix3x3::CreateRotationZ(transform->Rotation));
    //
    //    transform->Position -= {matrix(2, 0), matrix(2, 1)};
}

void ScenePanel::OnMouseRightButtonPressed(const PointerPoint& pointerPoint)
{
    SceneTouch(ConvertToPanelSpace(pointerPoint.Position));
}

void ScenePanel::OnMouseRightButtonMoved(const PointerPoint& pointerPoint)
{
    SceneTouch(ConvertToPanelSpace(pointerPoint.Position));
}

void ScenePanel::UpdateAnimation(const Duration& frameDuration)
{
    timer = std::max(timer - frameDuration, Duration::zero());

    if (timer <= Duration::zero()) {
        scrollAcceleration = 1.0f;
        return;
    }

    const auto duration = std::min<double>(frameDuration.count(), 1.0);
    const auto scroll = duration * normalizedScrollDirection * scrollAcceleration * (timer.count() / ZoomAnimationInterval.count());

    POMDOG_ASSERT(cameraZoom > 0);
    cameraZoom = Math::Saturate(cameraZoom + (cameraZoom * scroll * 1000));
}

void ScenePanel::Draw(DrawingContext&)
{
}

} // namespace Pomdog::GUI

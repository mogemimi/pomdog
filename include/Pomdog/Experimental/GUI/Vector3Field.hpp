// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/FontSize.hpp"
#include "Pomdog/Experimental/GUI/FontWeight.hpp"
#include "Pomdog/Experimental/GUI/Thickness.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>
#include <optional>
#include <string>

namespace Pomdog::GUI {

class FloatField;
class TextBlock;

class Vector3Field final
    : public Widget
    , public std::enable_shared_from_this<Vector3Field> {
public:
    explicit Vector3Field(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsReadOnly() const;
    void SetReadOnly(bool readOnly);

    bool IsAcceptable() const;
    void SetAcceptable(bool acceptable);

    Vector3 GetValue() const;
    void SetValue(const Vector3& value);

    Vector3 GetMinimum() const noexcept;
    void SetMinimum(const Vector3& minimum);

    Vector3 GetMaximum() const noexcept;
    void SetMaximum(const Vector3& maximum);

    int GetDecimals() const;
    void SetDecimals(int precision);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void OnEnter() override;

    void DoLayout() override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void(const Vector3&)> ValueChanged;

private:
    std::shared_ptr<FloatField> xField;
    std::shared_ptr<FloatField> yField;
    std::shared_ptr<FloatField> zField;

    ScopedConnection xValueChangedConn;
    ScopedConnection yValueChangedConn;
    ScopedConnection zValueChangedConn;
    bool needToUpdateLayout;
};

} // namespace Pomdog::GUI

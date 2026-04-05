// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/color.h"
#include "pomdog/math/vector3.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
class FloatField;
class TextBlock;
} // namespace pomdog::gui

namespace pomdog::gui {

class Vector3Field final
    : public Widget
    , public std::enable_shared_from_this<Vector3Field> {
public:
    explicit Vector3Field(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] bool
    isEnabled() const;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] bool
    isReadOnly() const;

    void
    setReadOnly(bool readOnly);

    [[nodiscard]] bool
    isAcceptable() const;

    void
    setAcceptable(bool acceptable);

    [[nodiscard]] Vector3
    getValue() const;

    void
    setValue(const Vector3& value);

    [[nodiscard]] Vector3
    getMinimum() const noexcept;

    void
    setMinimum(const Vector3& minimum);

    [[nodiscard]] Vector3
    getMaximum() const noexcept;

    void
    setMaximum(const Vector3& maximum);

    [[nodiscard]] int
    getDecimals() const;

    void
    setDecimals(int precision);

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    onEnter() override;

    void
    doLayout() override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void(const Vector3&)> ValueChanged;

private:
    std::shared_ptr<FloatField> xField_;
    std::shared_ptr<FloatField> yField_;
    std::shared_ptr<FloatField> zField_;

    ScopedConnection xValueChangedConn_;
    ScopedConnection yValueChangedConn_;
    ScopedConnection zValueChangedConn_;
    bool needToUpdateLayout_;
};

} // namespace pomdog::gui

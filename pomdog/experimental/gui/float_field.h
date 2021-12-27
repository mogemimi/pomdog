// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/font_size.hpp"
#include "pomdog/experimental/gui/font_weight.hpp"
#include "pomdog/experimental/gui/number_field.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui::detail {

class FloatFieldDataContext final : public NumberFieldDataContext {
public:
    std::string ToString() const override;

    std::string ToEditableString(const std::string& text) const override;

    double GetValue() const noexcept;

    void SetValue(double valueIn);

    double GetMinimum() const noexcept;

    void SetMinimum(double minimumIn);

    double GetMaximum() const noexcept;

    void SetMaximum(double maximumIn);

    int GetDecimals() const noexcept;

    void SetDecimals(int precisionIn);

    double GetUnit() const noexcept;

    void IncrementValue() override;

    void DecrementValue() override;

    void BeginDragging() override;

    void UpdateDragging(int amount) override;

    bool TextSubmitted(const std::string& text) override;

private:
    double value = 0.0;
    double startDragValue = 0.0;
    double minimum = std::numeric_limits<double>::lowest();
    double maximum = std::numeric_limits<double>::max();
    int precision = 3;
};

} // namespace pomdog::gui::detail

namespace pomdog::gui {

class FloatField final
    : public Widget
    , public std::enable_shared_from_this<FloatField> {
public:
    explicit FloatField(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsReadOnly() const;
    void SetReadOnly(bool readOnly);

    bool IsAcceptable() const;
    void SetAcceptable(bool acceptable);

    double GetValue() const;
    void SetValue(double value);

    double GetMinimum() const noexcept;
    void SetMinimum(double minimum);

    double GetMaximum() const noexcept;
    void SetMaximum(double maximum);

    int GetDecimals() const;
    void SetDecimals(int precision);

    std::string GetPlaceholderText() const;
    void SetPlaceholderText(const std::string& text);

    bool HasSelectedText() const;

    void Deselect();

    void SetPrependAreaColor(const std::optional<Color>& color);

    void SetPrependAreaString(const std::string& text);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void OnEnter() override;

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void(double)> ValueChanged;

private:
    std::shared_ptr<NumberField> numberField;
    std::shared_ptr<detail::FloatFieldDataContext> dataContext;
    ScopedConnection valueChangedConn;
};

} // namespace pomdog::gui

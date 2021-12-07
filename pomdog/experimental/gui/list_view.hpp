// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class ScrollView;
class VerticalLayout;

class ListView final
    : public Widget
    , public std::enable_shared_from_this<ListView> {
public:
    ListView(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void Draw(DrawingContext& drawingContext) override;

    void AddChild(const std::shared_ptr<Widget>& widget);

    void RemoveChild(const std::shared_ptr<Widget>& widget);

    void RemoveChild(int index);

    int GetItemCount() const noexcept;

    std::optional<int> GetCurrentIndex() const noexcept;

    void SetCurrentIndex(int index);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    std::shared_ptr<Widget> GetChildAt(int index);

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

    void SetPadding(const Thickness& padding);

    Signal<void(std::optional<int> index)> CurrentIndexChanged;

private:
    void UpdateLayout();

private:
    std::shared_ptr<VerticalLayout> verticalLayout;
    Thickness padding;
    std::optional<int> currentIndex;
    bool needToUpdateLayout;
};

} // namespace Pomdog::GUI

// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/Thickness.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <list>

namespace Pomdog {
namespace UI {

class HorizontalLayout final
    : public UIElement
    , public std::enable_shared_from_this<HorizontalLayout> {
public:
    HorizontalLayout(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetMargin(const Thickness& margin);

    void SetLayoutSpacing(int spacing);

    void SetStackedLayout(bool isStackedLayout);

    void SetTransform(const Matrix3x2& matrix) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool SizeToFitContent() const override;

    void OnEnter() override;

    void Draw(DrawingContext & drawingContext) override;

    void AddChild(const std::shared_ptr<UIElement>& element);

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    using UIElementCollection = std::list<std::shared_ptr<UIElement>>;
    UIElementCollection children;
    Detail::UIEventConnection connection;
    Rendering::PrimitiveFunctionCommand renderCommand;
    Thickness margin;
    int layoutSpacing;
    bool needToUpdateLayout;
    bool isStackedLayout;
};

} // namespace UI
} // namespace Pomdog

//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "UIElementHierarchy.hpp"
#include "DrawingContext.hpp"
#include "UIHelper.hpp"
#include "UIView.hpp"
#include <list>

namespace Pomdog {
namespace UI {

class GameEditorView: public UIView, public std::enable_shared_from_this<GameEditorView> {
public:
	GameEditorView(std::weak_ptr<UIEventDispatcher> const& dispatcherIn)
		: UIView(Matrix3x2::Identity, 0, 0)
		, dispatcher(dispatcherIn)
	{}

	void Draw(DrawingContext & drawingContext) override
	{
		drawingContext.Push(Matrix3x2::Identity);

		for (auto & child: children)
		{
			POMDOG_ASSERT(child);
			child->Draw(drawingContext);
		}

		drawingContext.Pop();
	}

	void AddChild(std::shared_ptr<UIView> const& element)
	{
		POMDOG_ASSERT(element->Parent().expired());
		children.push_back(element);

		POMDOG_ASSERT(shared_from_this());
		element->Parent(shared_from_this());
		element->OnParentChanged();
	}

	std::weak_ptr<UIEventDispatcher> Dispatcher() const override final
	{
		return dispatcher;
	}

	void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) override
	{
		for (auto & child: children)
		{
			POMDOG_ASSERT(child);
			if (child->SizeToFitContent())
			{
				child->OnRenderSizeChanged(width, height);
			}
		}
	}

	void UpdateTransform() override
	{
		UIView::UpdateTransform();
		for (auto & child: children)
		{
			POMDOG_ASSERT(child);
			child->UpdateTransform();
		}
	}

private:
	using UIElementCollection = std::list<std::shared_ptr<UIElement>>;
	UIElementCollection children;
	std::weak_ptr<UIEventDispatcher> dispatcher;
};

//-----------------------------------------------------------------------
UIElementHierarchy::UIElementHierarchy(std::shared_ptr<GameWindow> const& window)
	: dispatcher(std::make_shared<UIEventDispatcher>(window))
{
	root = std::make_shared<GameEditorView>(dispatcher);
}
//-----------------------------------------------------------------------
std::shared_ptr<UIEventDispatcher> UIElementHierarchy::Dispatcher() const
{
	POMDOG_ASSERT(dispatcher);
	return dispatcher;
}
//-----------------------------------------------------------------------
void UIElementHierarchy::Touch(MouseState const& mouseState)
{
	POMDOG_ASSERT(dispatcher);
	dispatcher->Touch(mouseState);
	dispatcher->UpdateChildren();
}
//-----------------------------------------------------------------------
void UIElementHierarchy::AddChild(std::shared_ptr<UIView> const& view)
{
	POMDOG_ASSERT(view);
	POMDOG_ASSERT(root);
	root->AddChild(view);
}
//-----------------------------------------------------------------------
void UIElementHierarchy::AddChild(std::shared_ptr<UIView> && view)
{
	POMDOG_ASSERT(view);
	POMDOG_ASSERT(root);
	root->AddChild(view);
}
//-----------------------------------------------------------------------
void UIElementHierarchy::UpdateAnimation(DurationSeconds const& frameDuration)
{
	POMDOG_ASSERT(dispatcher);
	dispatcher->UpdateAnimation(frameDuration);
}
//-----------------------------------------------------------------------
void UIElementHierarchy::Draw(DrawingContext & drawingContext)
{
	POMDOG_ASSERT(root);
	root->UpdateTransform();
	root->Draw(drawingContext);
}
//-----------------------------------------------------------------------
void UIElementHierarchy::RenderSizeChanged(std::uint32_t width, std::uint32_t height)
{
	POMDOG_ASSERT(root);
	root->OnRenderSizeChanged(width, height);
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog

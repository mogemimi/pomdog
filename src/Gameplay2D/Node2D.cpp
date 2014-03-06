//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Gameplay2D/Node2D.hpp>
#include <algorithm>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Gameplay/GameObject.hpp>

namespace Pomdog {
namespace {

static void RemoveNode(std::vector<std::shared_ptr<GameObject>> & children, std::shared_ptr<GameObject> const& gameObject)
{
	children.erase(std::remove_if(std::begin(children), std::end(children),
		[gameObject](std::shared_ptr<GameObject> const& child)
		{
			POMDOG_ASSERT(child);
			return child == gameObject;
		}), std::end(children));
}

}// unnamed namespace
//-----------------------------------------------------------------------
Node2D::Node2D(std::weak_ptr<GameObject> const& weakSelfIn)
	: weakSelf(weakSelfIn)
{
	POMDOG_ASSERT(!weakSelf.expired());
}
//-----------------------------------------------------------------------
Node2D::Node2D(std::weak_ptr<GameObject> && weakSelfIn)
	: weakSelf(std::move(weakSelfIn))
{
	POMDOG_ASSERT(!weakSelf.expired());
}
//-----------------------------------------------------------------------
Node2D::~Node2D()
{
	for (auto child: children)
	{
		if (auto node = child->Component<Node2D>())
		{
			node->weakParent.reset();
			node->weakSelf.reset();
		}
	}

	if (auto parent = weakParent.lock())
	{
		if (auto parentNode = parent->Component<Node2D>())
		{
			RemoveNode(parentNode->children, weakSelf.lock());
		}
	}
}
//-----------------------------------------------------------------------
void Node2D::AddChild(std::shared_ptr<GameObject> const& gameObject)
{
	POMDOG_ASSERT(gameObject);

	auto node = gameObject->Component<Node2D>();

	if (nullptr == node)
	{
		node = gameObject->AddComponent<Node2D>(gameObject);
	}
	
	POMDOG_ASSERT(node);
	POMDOG_ASSERT(node->weakParent.expired());
	POMDOG_ASSERT(!weakSelf.expired());
	
	node->weakParent = weakSelf;
	
	children.push_back(gameObject);
}
//-----------------------------------------------------------------------
void Node2D::AddChild(std::shared_ptr<GameObject> && gameObject)
{
	POMDOG_ASSERT(gameObject);
	POMDOG_ASSERT(gameObject->HasComponent<Node2D>());
	
	auto node = gameObject->Component<Node2D>();

	if (nullptr == node)
	{
		node = gameObject->AddComponent<Node2D>(gameObject);
	}
	
	POMDOG_ASSERT(node);
	POMDOG_ASSERT(node->weakParent.expired());
	POMDOG_ASSERT(!weakSelf.expired());
	
	node->weakParent = weakSelf;
		
	children.push_back(std::move(gameObject));
}
//-----------------------------------------------------------------------
void Node2D::RemoveChild(std::shared_ptr<GameObject> const& gameObject)
{
	POMDOG_ASSERT(gameObject);
	POMDOG_ASSERT(gameObject->HasComponent<Node2D>());
	
	if (auto childNode = gameObject->Component<Node2D>())
	{
		POMDOG_ASSERT(childNode->Parent().lock() == weakSelf.lock());
		POMDOG_ASSERT(childNode->Parent().lock()->Component<Node2D>() == this);
		childNode->weakParent.reset();
	}
	
	RemoveNode(children, gameObject);
}
//-----------------------------------------------------------------------
std::weak_ptr<GameObject> Node2D::Parent() const
{
	return weakParent;
}
//-----------------------------------------------------------------------
std::vector<std::shared_ptr<GameObject>> Node2D::Children() const
{
	return children;
}
//-----------------------------------------------------------------------
}// namespace Pomdog

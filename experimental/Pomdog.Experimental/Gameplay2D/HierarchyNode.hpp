//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_HIERARCHYNODE_65A32766_DE9A_40C5_9F1A_2FA6F042E424_HPP
#define POMDOG_HIERARCHYNODE_65A32766_DE9A_40C5_9F1A_2FA6F042E424_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class GameObject;
class GameWorld;

class POMDOG_EXPORT HierarchyNode: public std::enable_shared_from_this<HierarchyNode> {
public:
	HierarchyNode() = default;
	
	explicit HierarchyNode(std::shared_ptr<GameObject> const& object);

	virtual ~HierarchyNode();

	void AddChild(std::shared_ptr<HierarchyNode> const& child);

	void RemoveChild(std::shared_ptr<HierarchyNode> const& child);
	
	void RemoveChildren();
	
	void EnumerateChildren(std::function<void(std::shared_ptr<HierarchyNode> const& child)> const& callback);
	
	std::shared_ptr<HierarchyNode> Parent() const;

	std::weak_ptr<HierarchyNode> WeakParent() const;

	std::shared_ptr<GameObject> Object;
	
private:
	std::weak_ptr<HierarchyNode> weakParent;
	std::shared_ptr<HierarchyNode> firstChild;
	std::shared_ptr<HierarchyNode> sibling;
};

}// namespace Pomdog

#endif // !defined(POMDOG_HIERARCHYNODE_65A32766_DE9A_40C5_9F1A_2FA6F042E424_HPP)

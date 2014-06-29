//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_HIERARCHYNODE_65A32766_DE9A_40C5_9F1A_2FA6F042E424_HPP
#define POMDOG_HIERARCHYNODE_65A32766_DE9A_40C5_9F1A_2FA6F042E424_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Config/Export.hpp>
#include "GameObject.hpp"

namespace Pomdog {

class GameObject;
class GameWorld;

class POMDOG_EXPORT HierarchyNode: public std::enable_shared_from_this<HierarchyNode> {
public:
	HierarchyNode() = default;
	
	HierarchyNode(std::shared_ptr<GameObject> const& objectIn);

	~HierarchyNode();

	void AddChild(std::shared_ptr<HierarchyNode> const& child);

	void RemoveChild(std::shared_ptr<HierarchyNode> const& child);
	
	std::shared_ptr<GameObject> Object;
	
private:
	std::weak_ptr<HierarchyNode> weakParent;
	std::shared_ptr<HierarchyNode> firstChild;
	std::shared_ptr<HierarchyNode> sibling;
};

}// namespace Pomdog

#endif // !defined(POMDOG_HIERARCHYNODE_65A32766_DE9A_40C5_9F1A_2FA6F042E424_HPP)

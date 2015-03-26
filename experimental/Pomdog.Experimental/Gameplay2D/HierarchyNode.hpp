// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_HIERARCHYNODE_65A32766_HPP
#define POMDOG_HIERARCHYNODE_65A32766_HPP

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

#endif // POMDOG_HIERARCHYNODE_65A32766_HPP

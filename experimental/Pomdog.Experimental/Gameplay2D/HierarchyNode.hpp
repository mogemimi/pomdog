// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class Entity;
class EntityManager;

class POMDOG_EXPORT HierarchyNode: public std::enable_shared_from_this<HierarchyNode> {
public:
    HierarchyNode() = default;

    explicit HierarchyNode(std::shared_ptr<Entity> const& object);

    virtual ~HierarchyNode();

    void AddChild(std::shared_ptr<HierarchyNode> const& child);

    void RemoveChild(std::shared_ptr<HierarchyNode> const& child);

    void RemoveChildren();

    void EnumerateChildren(std::function<void(std::shared_ptr<HierarchyNode> const& child)> const& callback);

    std::shared_ptr<HierarchyNode> Parent() const;

    std::weak_ptr<HierarchyNode> WeakParent() const;

    std::shared_ptr<Entity> Object;

private:
    std::weak_ptr<HierarchyNode> weakParent;
    std::shared_ptr<HierarchyNode> firstChild;
    std::shared_ptr<HierarchyNode> sibling;
};

} // namespace Pomdog

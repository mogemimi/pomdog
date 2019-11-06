// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <functional>
#include <memory>

namespace Pomdog {

class Entity;
class EntityManager;

class POMDOG_EXPORT HierarchyNode final
    : public Component
    , public std::enable_shared_from_this<HierarchyNode> {
public:
    HierarchyNode() = default;

    ~HierarchyNode() final;

    void AddChild(const std::shared_ptr<HierarchyNode>& child);

    void RemoveChild(const std::shared_ptr<HierarchyNode>& child);

    void RemoveChildren();

    void RemoveFromParent();

    void EnumerateChildren(const std::function<void(const std::shared_ptr<HierarchyNode>& child)>& callback);

    std::shared_ptr<HierarchyNode> GetParent() const noexcept;

    std::weak_ptr<HierarchyNode> GetWeakParent() const noexcept;

private:
    std::weak_ptr<HierarchyNode> weakParent;
    std::shared_ptr<HierarchyNode> firstChild;
    std::shared_ptr<HierarchyNode> sibling;
};

} // namespace Pomdog

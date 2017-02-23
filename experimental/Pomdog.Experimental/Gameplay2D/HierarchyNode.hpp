// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class Entity;
class EntityManager;

class POMDOG_EXPORT HierarchyNode final
    : public Component
    , public std::enable_shared_from_this<HierarchyNode> {
public:
    HierarchyNode() = default;

    ~HierarchyNode() final;

    void AddChild(std::shared_ptr<HierarchyNode> const& child);

    void RemoveChild(std::shared_ptr<HierarchyNode> const& child);

    void RemoveChildren();

    void RemoveFromParent();

    void EnumerateChildren(std::function<void(std::shared_ptr<HierarchyNode> const& child)> const& callback);

    std::shared_ptr<HierarchyNode> GetParent() const noexcept;

    std::weak_ptr<HierarchyNode> GetWeakParent() const noexcept;

private:
    std::weak_ptr<HierarchyNode> weakParent;
    std::shared_ptr<HierarchyNode> firstChild;
    std::shared_ptr<HierarchyNode> sibling;
};

} // namespace Pomdog

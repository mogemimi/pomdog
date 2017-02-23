// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "HierarchyNode.hpp"

namespace Pomdog {

HierarchyNode::~HierarchyNode()
{
    RemoveChildren();

    if (auto parent = weakParent.lock()) {
        if (parent->firstChild.get() == this) {
            parent->firstChild = sibling;
        }
        else if (parent->firstChild) {
            auto prevSibling = parent->firstChild;
            POMDOG_ASSERT(prevSibling);

            while (prevSibling->sibling.get() != this) {
                prevSibling = prevSibling->sibling;
            }

            if (prevSibling->sibling.get() == this) {
                prevSibling->sibling = sibling;
            }
        }
    }
}

void HierarchyNode::AddChild(std::shared_ptr<HierarchyNode> const& child)
{
    POMDOG_ASSERT(child);
    child->weakParent = shared_from_this();

    if (!firstChild) {
        firstChild = child;
        return;
    }

    auto prevSibling = firstChild;
    POMDOG_ASSERT(prevSibling);

    while (prevSibling->sibling) {
        prevSibling = prevSibling->sibling;
    }

    POMDOG_ASSERT(prevSibling);
    POMDOG_ASSERT(!prevSibling->sibling);
    prevSibling->sibling = child;
}

void HierarchyNode::RemoveChild(std::shared_ptr<HierarchyNode> const& child)
{
    POMDOG_ASSERT(child);
    POMDOG_ASSERT(!child->weakParent.expired());

    if (child->weakParent.lock() != shared_from_this()) {
        return;
    }

    if (firstChild == child) {
        child->weakParent.reset();
        firstChild = child->sibling;
        POMDOG_ASSERT(child->weakParent.expired());
        POMDOG_ASSERT(child->sibling == this->firstChild);
        return;
    }

    auto prevSibling = firstChild;
    while (prevSibling->sibling) {
        if (prevSibling->sibling == child) {
            child->weakParent.reset();
            prevSibling->sibling = child->sibling;
            POMDOG_ASSERT(child->weakParent.expired());
            POMDOG_ASSERT(child->sibling == this->firstChild);
            return;
        }
        prevSibling = prevSibling->sibling;
    }
}

void HierarchyNode::RemoveChildren()
{
    auto child = firstChild;
    while (child) {
        child->weakParent.reset();
        child = child->sibling;
    }
    firstChild.reset();
}

void HierarchyNode::RemoveFromParent()
{
    if (auto parent = weakParent.lock()) {
        parent->RemoveChild(shared_from_this());
    }
    weakParent.reset();
}

void HierarchyNode::EnumerateChildren(
    std::function<void(std::shared_ptr<HierarchyNode> const& child)> const& callback)
{
    auto child = firstChild;
    while (child) {
        POMDOG_ASSERT(!child->weakParent.expired());
        POMDOG_ASSERT(child->weakParent.lock() == shared_from_this());

        POMDOG_ASSERT(callback);
        callback(child);
        child = child->sibling;
    }
}

std::shared_ptr<HierarchyNode> HierarchyNode::GetParent() const noexcept
{
    return weakParent.lock();
}

std::weak_ptr<HierarchyNode> HierarchyNode::GetWeakParent() const noexcept
{
    return weakParent;
}

} // namespace Pomdog

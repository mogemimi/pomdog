//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_NODE2D_4DC2EF5D_33C3_45C8_9C80_190DBC5662C5_HPP
#define POMDOG_NODE2D_4DC2EF5D_33C3_45C8_9C80_190DBC5662C5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <vector>
#include <Pomdog/Config/Export.hpp>

namespace Pomdog {

class GameObject;

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay2D
/// @{

class POMDOG_EXPORT Node2D
{
public:
	Node2D() = delete;
	
	explicit Node2D(std::weak_ptr<GameObject> const& self);
	explicit Node2D(std::weak_ptr<GameObject> && self);
	
	Node2D(Node2D const&) = delete;
	Node2D(Node2D &&) = default;

	Node2D & operator=(Node2D const&) = delete;
	Node2D & operator=(Node2D &&) = default;
	
	~Node2D();
	
	void AddChild(std::shared_ptr<GameObject> const& gameObject);
	void AddChild(std::shared_ptr<GameObject> && gameObject);

	void RemoveChild(std::shared_ptr<GameObject> const& gameObject);
	
	std::weak_ptr<GameObject> Parent() const;
	
	std::vector<std::shared_ptr<GameObject>> Children() const;
	
private:
	std::vector<std::shared_ptr<GameObject>> children;
	std::weak_ptr<GameObject> weakParent;
	std::weak_ptr<GameObject> weakSelf;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_NODE2D_4DC2EF5D_33C3_45C8_9C80_190DBC5662C5_HPP)

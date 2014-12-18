//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SKELETON_015B5D6C_C3A5_4AC7_B9BB_501EF76F5C7B_HPP
#define POMDOG_SKELETON_015B5D6C_C3A5_4AC7_B9BB_501EF76F5C7B_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Joint.hpp"
#include "JointPose.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {

///@~Japanese
/// @brief ジョイント（間接）またはボーンから構成されるスケルトンです。
class Skeleton {
public:
	Skeleton() = default;
	explicit Skeleton(std::vector<Joint> && joints);

	Skeleton(Skeleton const&) = delete;
	Skeleton & operator=(Skeleton const&) = delete;
	Skeleton(Skeleton &&) = default;
	Skeleton & operator=(Skeleton &&) = default;

	Joint const& Root() const;
	Joint const& Joints(JointIndex const& jointIndex) const;
	std::uint16_t JointCount() const;
	
	typedef std::vector<Joint>::const_iterator const_iterator;
	typedef std::vector<Joint>::iterator iterator;
	
	const_iterator begin() const;
	const_iterator end() const;
	iterator begin();
	iterator end();

private:
	std::vector<Joint> joints;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETON_015B5D6C_C3A5_4AC7_B9BB_501EF76F5C7B_HPP)

//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEXTURE_3A7EAD6D_145C_4F90_8749_568FC6DD4338_HPP
#define POMDOG_TEXTURE_3A7EAD6D_145C_4F90_8749_568FC6DD4338_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

///@~Japanese
/// @brief テクスチャです。
class POMDOG_EXPORT Texture {
public:
	Texture() = default;
	Texture(Texture const&) = delete;
	Texture(Texture &&) = default;

	Texture & operator=(Texture const&) = delete;
	Texture & operator=(Texture &&) = default;

	virtual ~Texture() = default;
};

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURE_3A7EAD6D_145C_4F90_8749_568FC6DD4338_HPP)

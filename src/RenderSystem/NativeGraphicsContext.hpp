//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_NATIVEGRAPHICSCONTEXT_HPP
#define POMDOG_DETAIL_NATIVEGRAPHICSCONTEXT_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Utility/Noncopyable.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext: Noncopyable
{
public:
	virtual ~NativeGraphicsContext() = default;
	
	virtual void Clear(Color const& color) = 0;
	
	virtual void Present() = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_NATIVEGRAPHICSCONTEXT_HPP)

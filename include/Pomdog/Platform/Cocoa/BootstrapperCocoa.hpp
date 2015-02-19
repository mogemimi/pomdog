//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP
#define POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Basic/Export.hpp"
#import <AppKit/AppKit.h>
#include <memory>

namespace Pomdog {

class GameHost;

namespace Details {
namespace Cocoa {

class POMDOG_EXPORT BootstrapperCocoa final {
public:
	BootstrapperCocoa & SetSurfaceFormat(SurfaceFormat surfaceFormat);

	BootstrapperCocoa & SetDepthFormat(DepthFormat depthFormat);

	BootstrapperCocoa & SetPresentationInterval(std::uint32_t presentationInterval);

	std::shared_ptr<GameHost> CreateGameHost(NSWindow* nativeWindow);

private:
	std::uint32_t presentationInterval = 60;
	SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
	DepthFormat depthFormat = DepthFormat::Depth24Stencil8;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP)

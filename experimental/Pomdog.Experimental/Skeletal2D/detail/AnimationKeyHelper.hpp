//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ANIMATIONKEYHELPER_1AC67582_2235_4181_93A6_6E414A9520F5_HPP
#define POMDOG_ANIMATIONKEYHELPER_1AC67582_2235_4181_93A6_6E414A9520F5_HPP

#if _MSC_VER > 1000
#pragma once
#endif

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

namespace AnimationKeyHelper {
	template <class Keyframe>
	bool Less(Keyframe const& a, Keyframe const& b)
	{
		return a.Time < b.Time;
	}
}

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONKEYHELPER_1AC67582_2235_4181_93A6_6E414A9520F5_HPP)

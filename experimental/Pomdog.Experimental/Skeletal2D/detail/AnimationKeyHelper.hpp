//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONKEYHELPER_1AC67582_2235_4181_93A6_6E414A9520F5_HPP
#define POMDOG_ANIMATIONKEYHELPER_1AC67582_2235_4181_93A6_6E414A9520F5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

namespace AnimationKeyHelper {
	template <class Keyframe>
	bool Less(Keyframe const& a, Keyframe const& b)
	{
		return a.Time < b.Time;
	}
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONKEYHELPER_1AC67582_2235_4181_93A6_6E414A9520F5_HPP)

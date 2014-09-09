//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKINNEDEFFECT_3BADC686_76B4_4F06_94E7_1A8775F1D00D_HPP
#define POMDOG_SKINNEDEFFECT_3BADC686_76B4_4F06_94E7_1A8775F1D00D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>

namespace Pomdog {

class SkinnedEffect {
public:
	explicit SkinnedEffect(GraphicsDevice & graphicsDevice);

	~SkinnedEffect();

	void SetWorldViewProjection(Matrix4x4 const& worldViewProjection);

	void SetTexture(std::shared_ptr<Texture2D> const& texture);
	
	void SetBoneTransforms(Matrix3x2 const* boneTransforms, size_t count);
	
	void Apply(GraphicsContext & graphicsContext);
	
	static constexpr size_t MaxBones = 96;
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDEFFECT_3BADC686_76B4_4F06_94E7_1A8775F1D00D_HPP)

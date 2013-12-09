//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_SAMPLERSTATEGL4_46408BFA_6030_11E3_82CF_A8206655A22B_HPP
#define POMDOG_SRC_GL4_SAMPLERSTATEGL4_46408BFA_6030_11E3_82CF_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include "../RenderSystem/NativeSamplerState.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class SamplerStateGL4 final: public NativeSamplerState
{
public:
	SamplerStateGL4() = delete;
	
	explicit SamplerStateGL4(SamplerDescription const& description);
	
	~SamplerStateGL4();
	
	///@copydoc NativeSamplerState
	void Apply(std::size_t index) override;
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_SAMPLERSTATEGL4_46408BFA_6030_11E3_82CF_A8206655A22B_HPP)

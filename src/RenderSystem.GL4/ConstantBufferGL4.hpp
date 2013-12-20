//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_CONSTANTBUFFERGL4_98395F08_1611_4948_9C35_81FE756DE978_HPP
#define POMDOG_SRC_GL4_CONSTANTBUFFERGL4_98395F08_1611_4948_9C35_81FE756DE978_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class ConstantBufferGL4
{
public:
	ConstantBufferGL4() = delete;
	
	//ConstantBufferGL4();
	
	~ConstantBufferGL4();

	void Apply();
	
private:
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_CONSTANTBUFFERGL4_98395F08_1611_4948_9C35_81FE756DE978_HPP)

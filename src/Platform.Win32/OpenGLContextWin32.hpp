//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_OPENGLCONTEXTWIN32_A7C60FF2_4CA1_4345_80A7_2496218BC0EA_HPP
#define POMDOG_OPENGLCONTEXTWIN32_A7C60FF2_4CA1_4345_80A7_2496218BC0EA_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <functional>
#include <memory>
#include <type_traits>

namespace Pomdog {

class PresentationParameters;

namespace Details {
namespace Win32 {

class OpenGLContextWin32 final: public RenderSystem::GL4::OpenGLContext {
public:
	OpenGLContextWin32() = delete;

	OpenGLContextWin32(HWND windowHandle, PresentationParameters const& presentationParameters);

	~OpenGLContextWin32();

	void MakeCurrent() override;

	void ClearCurrent() override;

	void SwapBuffers() override;

private:
	HWND windowHandle;

	std::unique_ptr<
		std::remove_pointer<HDC>::type,
		std::function<void(HDC)>
	> hdc;

	std::unique_ptr<
		std::remove_pointer<HGLRC>::type,
		std::function<void(HGLRC)>
	> glrc;
};

}// namespace Win32
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_OPENGLCONTEXTWIN32_A7C60FF2_4CA1_4345_80A7_2496218BC0EA_HPP)

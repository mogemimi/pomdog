//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "OpenGLContextWin32.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <sstream>

namespace Pomdog {
namespace Details {
namespace Win32 {
//-----------------------------------------------------------------------
OpenGLContextWin32::OpenGLContextWin32(HWND windowHandleIn)
	: windowHandle(windowHandleIn)
	, hdc(nullptr, [this](HDC hdcIn) {
		ReleaseDC(windowHandle, hdcIn);
	})
	, glrc(nullptr, [](HGLRC glrcIn) {
		wglDeleteContext(glrcIn);
	})
{
	hdc.reset(GetDC(windowHandle));

	PIXELFORMATDESCRIPTOR formatDescriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32, // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24, // Number of bits for the depthbuffer
		8,  // Number of bits for the stencilbuffer
		0,  // Number of Aux buffers in the framebuffer
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	auto const pixelFormat = ChoosePixelFormat(hdc.get(), &formatDescriptor);

	if (0 == pixelFormat)
	{
		auto const errorCode = ::GetLastError();

		std::stringstream ss;
		ss << "Failed to call ChoosePixelFormat function."
			<< "The calling thread's last-error code is "
			<< std::hex << errorCode;

		POMDOG_THROW_EXCEPTION(std::runtime_error, ss.str());
	}

	if (!SetPixelFormat(hdc.get(), pixelFormat, &formatDescriptor))
	{
		auto const errorCode = ::GetLastError();

		std::stringstream ss;
		ss << "Failed to call SetPixelFormat function."
			<< "The calling thread's last-error code is "
			<< std::hex << errorCode;

		POMDOG_THROW_EXCEPTION(std::runtime_error, ss.str());
	}

	// Create gl context
	glrc.reset(wglCreateContext(hdc.get()));

	if (!wglMakeCurrent(hdc.get(), glrc.get()))
	{
		auto const errorCode = ::GetLastError();

		std::stringstream ss;
		ss << "Failed to call wglMakeCurrent function."
			<< "The calling thread's last-error code is "
			<< std::hex << errorCode;

		POMDOG_THROW_EXCEPTION(std::runtime_error, ss.str());
	}
}
//-----------------------------------------------------------------------
OpenGLContextWin32::~OpenGLContextWin32()
{
	glrc.reset();
	hdc.reset();
}
//-----------------------------------------------------------------------
void OpenGLContextWin32::MakeCurrentContext()
{
	POMDOG_ASSERT(hdc);
	POMDOG_ASSERT(glrc);
	wglMakeCurrent(hdc.get(), glrc.get());
}
//-----------------------------------------------------------------------
void OpenGLContextWin32::ClearCurrentContext()
{
	wglMakeCurrent(nullptr, nullptr);
}
//-----------------------------------------------------------------------
void OpenGLContextWin32::SwapBuffers()
{
	POMDOG_ASSERT(hdc);
	::SwapBuffers(hdc.get());
}
//-----------------------------------------------------------------------
}// namespace Win32
}// namespace Details
}// namespace Pomdog

// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Detail::GL4 {

class OpenGLContext {
public:
    virtual ~OpenGLContext();

    virtual void MakeCurrent() = 0;

    virtual void ClearCurrent() = 0;

    virtual void SwapBuffers() = 0;
};

} // namespace Pomdog::Detail::GL4

// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ConstantLayoutGL4.hpp"
#include "ErrorChecker.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
namespace {

static void SetBufferToTarget(GLuint slotIndex, ConstantBufferGL4 const& buffer)
{
    POMDOG_ASSERT(slotIndex >= 0);

#if defined(DEBUG) && !defined(NDEBUG)
    {
        static const GLuint maxUniformBufferBindings = ([] {
            GLint value = 0;
            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);
            return value;
        })();

        POMDOG_ASSERT(slotIndex < maxUniformBufferBindings);
    }
#endif

    glBindBufferBase(GL_UNIFORM_BUFFER, slotIndex, buffer.GetBuffer());
    POMDOG_CHECK_ERROR_GL4("glBindBufferBase");
}

} // unnamed namespace
//-----------------------------------------------------------------------
ConstantLayoutGL4::ConstantLayoutGL4(std::vector<ConstantBufferBindingGL4> && bindingsIn)
    : bindings(std::move(bindingsIn))
{}
//-----------------------------------------------------------------------
void ConstantLayoutGL4::SetConstantBuffer(std::string const& constantName,
    std::shared_ptr<NativeBuffer> const& constantBuffer)
{
    auto iter = std::find_if(std::begin(bindings), std::end(bindings),
        [&](ConstantBufferBindingGL4 const& binding) {
            return binding.Name == constantName;
        });

    POMDOG_ASSERT(std::end(bindings) != iter);

    if (std::end(bindings) != iter) {
        auto nativeConstantBuffer = std::dynamic_pointer_cast<ConstantBufferGL4>(constantBuffer);
        POMDOG_ASSERT(nativeConstantBuffer);
        iter->ConstantBuffer = nativeConstantBuffer;
    }
}
//-----------------------------------------------------------------------
void ConstantLayoutGL4::SetConstantBuffer(std::string const& constantName)
{
    auto iter = std::find_if(std::begin(bindings), std::end(bindings),
        [&](ConstantBufferBindingGL4 const& binding) {
            return binding.Name == constantName;
        });

    POMDOG_ASSERT(std::end(bindings) != iter);

    if (std::end(bindings) != iter) {
        iter->ConstantBuffer.reset();
    }
}
//-----------------------------------------------------------------------
void ConstantLayoutGL4::Apply()
{
    for (auto & binding: bindings) {
        if (binding.ConstantBuffer) {
            SetBufferToTarget(binding.SlotIndex, *binding.ConstantBuffer);
        }
    }
}
//-----------------------------------------------------------------------
} // namespace GL4
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

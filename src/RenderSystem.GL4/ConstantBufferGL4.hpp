// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTBUFFERGL4_98395F08_HPP
#define POMDOG_CONSTANTBUFFERGL4_98395F08_HPP

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstddef>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

namespace Tags {

struct ConstantBufferObjectTag {};

}// namespace Tags

using ConstantBufferObjectGL4 = Tagged<GLuint, Tags::ConstantBufferObjectTag>;

class ConstantBufferGL4 final : public NativeBuffer {
public:
    ConstantBufferGL4() = delete;

    ConstantBufferGL4(std::size_t sizeInBytes, BufferUsage bufferUsage);

    ConstantBufferGL4(void const* sourceData, std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    ~ConstantBufferGL4();

    void GetData(std::size_t offsetInBytes,
        void* destination, std::size_t sizeInBytes) const override;

    void SetData(std::size_t offsetInBytes,
        void const* source, std::size_t sizeInBytes) override;

    void Apply(int slotIndex);

private:
    Optional<ConstantBufferObjectGL4> bufferObject;
};

} // namespace GL4
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_CONSTANTBUFFERGL4_98395F08_HPP

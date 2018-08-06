// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {
namespace Metal {

// NOTE: Pomdog reserves the buffer slots 20 to 30 for Metal. If you
// specify the slot of a vertex buffer in Metal shader, you can use attribute,
// such as [[buffer(20)]], [[buffer(21)]] ... [[buffer(30)]] or [[stage_in]].
// For more details, please see the following link:
// https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
constexpr int VertexBufferSlotOffset = 20;
constexpr int MaxVertexBufferSlotCount = 31;

} // namespace Metal
} // namespace Detail
} // namespace Pomdog

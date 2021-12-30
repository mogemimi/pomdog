// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/pixel_format.h"

namespace pomdog::gpu::detail::metal {

MTLPixelFormat ToPixelFormat(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::A8_UNorm:
        return MTLPixelFormatA8Unorm;
    case PixelFormat::B8G8R8A8_UNorm:
        return MTLPixelFormatBGRA8Unorm;
    case PixelFormat::R10G10B10A2_UNorm:
        return MTLPixelFormatRGB10A2Unorm;
    case PixelFormat::R16G16B16A16_Float:
        return MTLPixelFormatRGBA16Float;
    case PixelFormat::R11G11B10_Float:
        return MTLPixelFormatRG11B10Float;
    case PixelFormat::R16G16_Float:
        return MTLPixelFormatRG16Float;
    case PixelFormat::R32G32B32A32_Float:
        return MTLPixelFormatRGBA32Float;
    case PixelFormat::R32_Float:
        return MTLPixelFormatR32Float;
    case PixelFormat::R8G8B8A8_UNorm:
        return MTLPixelFormatRGBA8Unorm;
    case PixelFormat::R8G8_UNorm:
        return MTLPixelFormatRG8Unorm;
    case PixelFormat::R8_UNorm:
        return MTLPixelFormatR8Unorm;
    case PixelFormat::BlockComp1_UNorm:
        return MTLPixelFormatBC1_RGBA;
    case PixelFormat::BlockComp2_UNorm:
        return MTLPixelFormatBC2_RGBA;
    case PixelFormat::BlockComp3_UNorm:
        return MTLPixelFormatBC3_RGBA;
    case PixelFormat::Depth16:
        return MTLPixelFormatDepth16Unorm;
    case PixelFormat::Depth32:
        return MTLPixelFormatDepth32Float;
    case PixelFormat::Depth24Stencil8:
        return MTLPixelFormatDepth24Unorm_Stencil8;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return MTLPixelFormatDepth32Float_Stencil8;
    case PixelFormat::Invalid:
        return MTLPixelFormatInvalid;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

MTLCompareFunction ToComparisonFunction(ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never:
        return MTLCompareFunctionNever;
    case ComparisonFunction::Less:
        return MTLCompareFunctionLess;
    case ComparisonFunction::Equal:
        return MTLCompareFunctionEqual;
    case ComparisonFunction::LessEqual:
        return MTLCompareFunctionLessEqual;
    case ComparisonFunction::Greater:
        return MTLCompareFunctionGreater;
    case ComparisonFunction::NotEqual:
        return MTLCompareFunctionNotEqual;
    case ComparisonFunction::GreaterEqual:
        return MTLCompareFunctionGreaterEqual;
    case ComparisonFunction::Always:
        return MTLCompareFunctionAlways;
    }
    POMDOG_UNREACHABLE("Unsupported comparison function");
}

} // namespace pomdog::gpu::detail::metal

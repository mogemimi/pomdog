// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ShaderMetal.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#import <Metal/Metal.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

ShaderMetal::ShaderMetal(
    id<MTLDevice> device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(device != nil);
    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    NSError* compileError = nil;

    auto sourceString = reinterpret_cast<const char*>(shaderBytecode.Code);

    id<MTLLibrary> library = [device
        newLibraryWithSource:[NSString stringWithUTF8String:sourceString]
        options:nil
        error:&compileError];

    if (compileError != nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, StringHelper::Format(
                "Failed to compile shader.\n"
                "error: %s",
                [[compileError localizedDescription] UTF8String]));
    }

    if (library == nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "MTLLibrary is nil");
    }

    POMDOG_ASSERT(!compileOptions.EntryPoint.empty());

    this->shader = [library newFunctionWithName:
        [NSString stringWithUTF8String:compileOptions.EntryPoint.c_str()]];

    if (shader == nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "MTLFunction is nil");
    }
}

ShaderMetal::ShaderMetal(
    id<MTLDevice> device,
    id<MTLLibrary> library,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(device != nil);
    POMDOG_ASSERT(library != nil);

    if (library == nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "MTLLibrary is nil");
    }

    POMDOG_ASSERT(!compileOptions.EntryPoint.empty());

    NSString* entryPoint = [NSString stringWithUTF8String:compileOptions.EntryPoint.c_str()];
    this->shader = [library newFunctionWithName:entryPoint];
}

id<MTLFunction> ShaderMetal::GetShader() const
{
    return shader;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog

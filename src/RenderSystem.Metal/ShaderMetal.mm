// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "ShaderMetal.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#import <Metal/Metal.h>

namespace Pomdog::Detail::Metal {

ShaderMetal::ShaderMetal(
    id<MTLDevice> device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    NSError* compileError = nullptr;

    id<MTLLibrary> library = nullptr;

    if (compileOptions.Precompiled) {
        dispatch_data_t libraryData = dispatch_data_create(
            shaderBytecode.Code,
            shaderBytecode.ByteLength,
            dispatch_get_main_queue(),
            ^{});

        library = [device newLibraryWithData:libraryData error:&compileError];
    }
    else {
        // NOTE: `shaderBytecode.Code` must be null-terminated string.
        NSString* sourceString = [NSString stringWithUTF8String:reinterpret_cast<const char*>(shaderBytecode.Code)];
        library = [device newLibraryWithSource:sourceString options:nullptr error:&compileError];
    }

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

    auto funcName = [NSString stringWithUTF8String:compileOptions.EntryPoint.data()];
    if (funcName == nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "funcName is nil");
    }
    this->shader = [library newFunctionWithName:funcName];

    if (shader == nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "MTLFunction is nil");
    }
}

ShaderMetal::ShaderMetal(
    id<MTLDevice> device,
    id<MTLLibrary> library,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(library != nullptr);

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

} // namespace Pomdog::Detail::Metal

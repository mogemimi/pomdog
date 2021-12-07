// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/metal/shader_metal.hpp"
#include "pomdog/graphics/backends/shader_bytecode.hpp"
#include "pomdog/graphics/backends/shader_compile_options.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/string_helper.hpp"
#import <Metal/Metal.h>

namespace Pomdog::Detail::Metal {

ShaderMetal::~ShaderMetal() = default;

std::unique_ptr<Error>
ShaderMetal::Initialize(
    id<MTLDevice> device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
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
            ^{
            });

        library = [device newLibraryWithData:libraryData error:&compileError];
    }
    else {
        // NOTE: `shaderBytecode.Code` must be null-terminated string.
        NSString* sourceString = [NSString stringWithUTF8String:reinterpret_cast<const char*>(shaderBytecode.Code)];
        library = [device newLibraryWithSource:sourceString options:nullptr error:&compileError];
    }

    if (compileError != nullptr) {
        return Errors::New(StringHelper::Format(
            "Failed to compile shader.\n"
            "error: %s",
            [[compileError localizedDescription] UTF8String]));
    }

    if (library == nullptr) {
        return Errors::New("MTLLibrary must be != nullptr");
    }

    POMDOG_ASSERT(!compileOptions.EntryPoint.empty());

    auto funcName = [NSString stringWithUTF8String:compileOptions.EntryPoint.data()];
    if (funcName == nullptr) {
        return Errors::New("funcName must be != nullptr");
    }
    this->shader = [library newFunctionWithName:funcName];

    if (shader == nullptr) {
        return Errors::New("MTLFunction must be != nullptr");
    }

    return nullptr;
}

std::unique_ptr<Error>
ShaderMetal::Initialize(
    id<MTLDevice> device,
    id<MTLLibrary> library,
    const ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(library != nullptr);

    if (library == nullptr) {
        return Errors::New("MTLLibrary must be != nullptr");
    }

    POMDOG_ASSERT(!compileOptions.EntryPoint.empty());

    NSString* entryPoint = [NSString stringWithUTF8String:compileOptions.EntryPoint.c_str()];
    this->shader = [library newFunctionWithName:entryPoint];

    return nullptr;
}

id<MTLFunction> ShaderMetal::GetShader() const noexcept
{
    return shader;
}

} // namespace Pomdog::Detail::Metal

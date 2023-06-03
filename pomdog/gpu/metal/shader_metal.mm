// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/shader_metal.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_helper.h"
#import <Metal/Metal.h>

namespace pomdog::gpu::detail::metal {

ShaderMetal::~ShaderMetal() = default;

std::unique_ptr<Error>
ShaderMetal::Initialize(
    id<MTLDevice> device,
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(shaderBytecode.code != nullptr);
    POMDOG_ASSERT(shaderBytecode.byteLength > 0);

    NSError* compileError = nullptr;

    id<MTLLibrary> library = nullptr;

    if (compileOptions.precompiled) {
        dispatch_data_t libraryData = dispatch_data_create(
            shaderBytecode.code,
            shaderBytecode.byteLength,
            dispatch_get_main_queue(),
            ^{
            });

        library = [device newLibraryWithData:libraryData error:&compileError];
    }
    else {
        // NOTE: `shaderBytecode.Code` must be null-terminated string.
        NSString* sourceString = [NSString stringWithUTF8String:reinterpret_cast<const char*>(shaderBytecode.code)];
        library = [device newLibraryWithSource:sourceString options:nullptr error:&compileError];
    }

    if (compileError != nullptr) {
        return errors::make(strings::format(
            "Failed to compile shader.\n"
            "error: %s",
            [[compileError localizedDescription] UTF8String]));
    }

    if (library == nullptr) {
        return errors::make("MTLLibrary must be != nullptr");
    }

    POMDOG_ASSERT(!compileOptions.entryPoint.empty());

    auto funcName = [NSString stringWithUTF8String:compileOptions.entryPoint.data()];
    if (funcName == nullptr) {
        return errors::make("funcName must be != nullptr");
    }
    this->shader = [library newFunctionWithName:funcName];

    if (shader == nullptr) {
        return errors::make("MTLFunction must be != nullptr");
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
        return errors::make("MTLLibrary must be != nullptr");
    }

    POMDOG_ASSERT(!compileOptions.entryPoint.empty());

    NSString* entryPoint = [NSString stringWithUTF8String:compileOptions.entryPoint.c_str()];
    this->shader = [library newFunctionWithName:entryPoint];

    return nullptr;
}

id<MTLFunction> ShaderMetal::GetShader() const noexcept
{
    return shader;
}

} // namespace pomdog::gpu::detail::metal

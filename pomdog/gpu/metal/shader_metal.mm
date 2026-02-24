// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/shader_metal.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/Metal.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {

ShaderMetal::~ShaderMetal() = default;

std::unique_ptr<Error>
ShaderMetal::initialize(
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
        return errors::make(pomdog::format(
            "failed to compile shader: error: {}",
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
    shader_ = [library newFunctionWithName:static_cast<NSString* _Nonnull>(funcName)];

    if (shader_ == nullptr) {
        return errors::make("MTLFunction must be != nullptr");
    }

    return nullptr;
}

std::unique_ptr<Error>
ShaderMetal::initialize(
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
    shader_ = [library newFunctionWithName:entryPoint];

    return nullptr;
}

id<MTLFunction>
ShaderMetal::getShader() const noexcept
{
    return shader_;
}

} // namespace pomdog::gpu::detail::metal

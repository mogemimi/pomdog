// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/shader_metal.h"
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
    std::span<const u8> shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(shaderBytecode.data() != nullptr);
    POMDOG_ASSERT(shaderBytecode.size() > 0);

    NSError* compileError = nullptr;

    id<MTLLibrary> library = nullptr;

    if (compileOptions.precompiled) {
        // FIXME: The empty destructor makes the dispatch data refer to
        // `shaderBytecode` without copying it, which requires the bytes to outlive
        // the data. The library can keep the data after this function returns,
        // while callers such as the shader loader release the bytes then, because
        // they pass the view of a vfs::File. Pass DISPATCH_DATA_DESTRUCTOR_DEFAULT
        // to copy the bytes before the shader loader loads .metallib files here.
        dispatch_data_t libraryData = dispatch_data_create(
            shaderBytecode.data(),
            shaderBytecode.size(),
            dispatch_get_main_queue(),
            ^{
            });

        library = [device newLibraryWithData:libraryData error:&compileError];
    }
    else {
        // FIXME: Every launch compiles the shader source here again. Make the asset
        // pipeline ship .metallib files and load them in the precompiled path
        // above. Then return an error for source as ShaderDirect3D11 does, and
        // remove newLibraryWithSource here.
        // NOTE: initWithBytes:length:encoding: reads the source by its length, so
        // `shaderBytecode` needs no null terminator, and callers can pass the view
        // of a vfs::File without copying it.
        NSString* sourceString = [[NSString alloc]
            initWithBytes:shaderBytecode.data()
                   length:shaderBytecode.size()
                 encoding:NSUTF8StringEncoding];
        if (sourceString == nullptr) {
            return errors::make("shader source is not valid UTF-8");
        }
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

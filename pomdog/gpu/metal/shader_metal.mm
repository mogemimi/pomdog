// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/shader_metal.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/Metal.h>
#include <string>
#include <utility>
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

    if (shaderBytecode.empty()) {
        return errors::make("shader bytecode is empty");
    }
    if (!compileOptions.precompiled) {
        return errors::make("shader bytecode is not precompiled");
    }
    if (compileOptions.entryPoint.empty()) {
        return errors::make("entry point is empty");
    }

    // NOTE: DISPATCH_DATA_DESTRUCTOR_DEFAULT copies the bytes. The library can keep
    // the data after this function returns, while callers such as the shader loader
    // release `shaderBytecode` then, because they pass the view of a vfs::File.
    dispatch_data_t libraryData = dispatch_data_create(
        shaderBytecode.data(),
        shaderBytecode.size(),
        nullptr,
        DISPATCH_DATA_DESTRUCTOR_DEFAULT);

    NSError* libraryError = nullptr;
    id<MTLLibrary> library = [device newLibraryWithData:libraryData error:&libraryError];
    if (library == nullptr) {
        std::string message = "newLibraryWithData() failed";
        if (libraryError != nullptr) {
            if (const char* description = [[libraryError localizedDescription] UTF8String]; description != nullptr) {
                message += ": ";
                message += description;
            }
        }
        return errors::make(std::move(message));
    }

    auto funcName = [NSString stringWithUTF8String:compileOptions.entryPoint.c_str()];
    if (funcName == nullptr) {
        return errors::make("entry point is not valid UTF-8");
    }
    shader_ = [library newFunctionWithName:static_cast<NSString* _Nonnull>(funcName)];

    if (shader_ == nullptr) {
        return errors::make(pomdog::format(
            "the Metal library has no function named '{}'",
            compileOptions.entryPoint));
    }

    return nullptr;
}

id<MTLFunction>
ShaderMetal::getShader() const noexcept
{
    return shader_;
}

} // namespace pomdog::gpu::detail::metal

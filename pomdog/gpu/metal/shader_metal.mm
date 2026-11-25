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

    // NOTE: The shader pipeline ships one .metallib per shader and checks that its
    // SPIR-V declares one entry point, so the library has exactly one function.
    // The backend takes that function without its name, as the other backends need
    // no entry point name either.
    NSArray<NSString*>* functionNames = [library functionNames];
    if ([functionNames count] != 1) {
        return errors::make(pomdog::format(
            "the Metal library has {} functions, want exactly one",
            static_cast<u64>([functionNames count])));
    }
    shader_ = [library newFunctionWithName:static_cast<NSString* _Nonnull>([functionNames firstObject])];

    if (shader_ == nullptr) {
        return errors::make("newFunctionWithName() failed");
    }

    return nullptr;
}

id<MTLFunction>
ShaderMetal::getShader() const noexcept
{
    return shader_;
}

} // namespace pomdog::gpu::detail::metal

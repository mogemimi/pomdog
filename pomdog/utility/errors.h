// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Error represents a error message.
class POMDOG_EXPORT Error {
public:
    virtual ~Error() noexcept;

    Error() noexcept = default;
    Error(const Error&) noexcept = delete;
    Error& operator=(const Error&) noexcept = delete;

    [[nodiscard]] virtual bool operator==(const char* s) noexcept = 0;
    [[nodiscard]] virtual bool operator!=(const char* s) noexcept = 0;
    [[nodiscard]] virtual bool operator==(const std::string& s) noexcept = 0;
    [[nodiscard]] virtual bool operator!=(const std::string& s) noexcept = 0;
    [[nodiscard]] virtual bool operator==(std::string_view s) noexcept = 0;
    [[nodiscard]] virtual bool operator!=(std::string_view s) noexcept = 0;

    /// Returns the name or the description for this error.
    [[nodiscard]] virtual std::string
    toString() const noexcept = 0;

    /// Creates a new object that is a copy of the error.
    [[nodiscard]] virtual std::unique_ptr<Error>
    clone() const noexcept = 0;
};

} // namespace pomdog

namespace pomdog::errors {

/// Returns an error with the given message.
[[nodiscard]] std::unique_ptr<Error>
make(std::string&& message) noexcept;

/// Returns a wrapped error with the additional information.
[[nodiscard]] std::unique_ptr<Error>
wrap(std::unique_ptr<Error>&& err, std::string&& message) noexcept;

} // namespace pomdog::errors

// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT Error {
public:
    virtual ~Error() noexcept;

    /// Returns a string representation of the error.
    [[nodiscard]] virtual std::string
    toString() const noexcept = 0;

    /// Creates a new object that is a copy of the error.
    [[nodiscard]] virtual std::unique_ptr<Error>
    clone() const noexcept = 0;
};

} // namespace pomdog

namespace pomdog::errors {

class POMDOG_EXPORT IOError final : public Error {
public:
    std::errc kind;
    std::string reason;

    /// Returns a string representation of the error.
    [[nodiscard]] std::string
    toString() const noexcept override;

    /// Creates a new object that is a copy of the error.
    [[nodiscard]] std::unique_ptr<Error>
    clone() const noexcept override;
};

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<IOError>
makeIOError(std::errc kind, std::string&& reason) noexcept;

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
make(std::string&& message) noexcept;

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
wrap(std::unique_ptr<Error>&& err, std::string&& message) noexcept;

} // namespace pomdog::errors

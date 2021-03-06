// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT Error {
public:
    virtual ~Error() noexcept;

    /// Returns a string representation of the error.
    [[nodiscard]] virtual std::string ToString() const noexcept = 0;

    /// Creates a new object that is a copy of the error.
    [[nodiscard]] virtual std::unique_ptr<Error> Clone() const noexcept = 0;
};

} // namespace Pomdog

namespace Pomdog::Errors {

class POMDOG_EXPORT IOError final : public Error {
public:
    std::errc Kind;
    std::string Reason;

    /// Returns a string representation of the error.
    [[nodiscard]] std::string ToString() const noexcept override;

    /// Creates a new object that is a copy of the error.
    [[nodiscard]] std::unique_ptr<Error> Clone() const noexcept override;
};

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<IOError>
New(std::errc kind, std::string&& reason) noexcept;

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
New(std::string&& message) noexcept;

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
Wrap(std::unique_ptr<Error>&& err, std::string&& message) noexcept;

} // namespace Pomdog::Errors

// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <string>
#include <system_error>

namespace Pomdog {

class POMDOG_EXPORT Error {
public:
    virtual ~Error() noexcept = default;

    [[nodiscard]] virtual std::string ToString() const noexcept = 0;
};

} // namespace Pomdog

namespace Pomdog::Errors {

class POMDOG_EXPORT IOError final : public Error {
public:
    std::errc Kind;
    std::string Reason;

    [[nodiscard]] std::string ToString() const noexcept;
};

[[nodiscard]] POMDOG_EXPORT
std::shared_ptr<IOError>
New(std::errc kind, std::string&& reason);

[[nodiscard]] POMDOG_EXPORT
std::shared_ptr<Error>
New(std::string&& message);

[[nodiscard]] POMDOG_EXPORT
std::shared_ptr<Error>
Wrap(const std::shared_ptr<Error>& err, std::string&& message);

[[nodiscard]] POMDOG_EXPORT
std::shared_ptr<Error>
Wrap(std::shared_ptr<Error>&& err, std::string&& message);

} // namespace Pomdog::Errors

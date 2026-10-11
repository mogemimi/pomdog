// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/not_found_error.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
namespace {

/// NotFoundError reports that no file exists. isNotFound() tells it apart from
/// other errors by its type.
class NotFoundError final : public Error {
public:
    std::string message;

public:
    [[nodiscard]] bool operator==(const char* s) noexcept override
    {
        return message == s;
    }

    [[nodiscard]] bool operator!=(const char* s) noexcept override
    {
        return message != s;
    }

    [[nodiscard]] bool operator==(const std::string& s) noexcept override
    {
        return message == s;
    }

    [[nodiscard]] bool operator!=(const std::string& s) noexcept override
    {
        return message != s;
    }

    [[nodiscard]] bool operator==(std::string_view s) noexcept override
    {
        return message == s;
    }

    [[nodiscard]] bool operator!=(std::string_view s) noexcept override
    {
        return message != s;
    }

    [[nodiscard]] std::string
    toString() const noexcept override
    {
        return message;
    }

    [[nodiscard]] std::unique_ptr<Error>
    clone() const noexcept override
    {
        auto err = std::make_unique<NotFoundError>();
        err->message = message;
        return err;
    }
};

} // namespace

std::unique_ptr<Error>
makeNotFoundError(std::string&& message) noexcept
{
    auto err = std::make_unique<NotFoundError>();
    err->message = std::move(message);
    return err;
}

bool isNotFound(const std::unique_ptr<Error>& err) noexcept
{
    for (const Error* e = err.get(); e != nullptr; e = e->unwrap()) {
        if (dynamic_cast<const NotFoundError*>(e) != nullptr) {
            return true;
        }
    }
    return false;
}

} // namespace pomdog::vfs

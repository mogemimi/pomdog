// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
Error::~Error() noexcept = default;
} // namespace pomdog

namespace pomdog::errors {
namespace {

class StringError final : public Error {
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
        auto err = std::make_unique<StringError>();
        err->message = message;
        return err;
    }
};

class WrappedError final : public Error {
public:
    std::unique_ptr<Error> err;
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
        if (err != nullptr) {
            return message + ": " + err->toString();
        }
        return message;
    }

    [[nodiscard]] std::unique_ptr<Error>
    clone() const noexcept override
    {
        auto wrapped = std::make_unique<WrappedError>();
        wrapped->err = err->clone();
        wrapped->message = message;
        return wrapped;
    }
};

} // namespace

[[nodiscard]] std::unique_ptr<Error>
make(std::string&& message) noexcept
{
    auto err = std::make_unique<StringError>();
    err->message = std::move(message);
    return err;
}

[[nodiscard]] std::unique_ptr<Error>
wrap(std::unique_ptr<Error>&& err, std::string&& message) noexcept
{
    auto wrapped = std::make_unique<WrappedError>();
    wrapped->err = std::move(err);
    wrapped->message = std::move(message);
    return wrapped;
}

} // namespace pomdog::errors

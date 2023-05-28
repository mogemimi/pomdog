// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/errors.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
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

    [[nodiscard]] std::string toString() const noexcept override
    {
        return message;
    }

    [[nodiscard]] std::unique_ptr<Error> clone() const noexcept override
    {
        auto err = std::make_unique<StringError>();
        err->message = message;
#if defined(__GNUC__) && !defined(__clang__)
        return err;
#else
        return std::move(err);
#endif
    }
};

class WrappedError final : public Error {
public:
    std::unique_ptr<Error> err;
    std::string message;

    [[nodiscard]] std::string toString() const noexcept override
    {
        if (err != nullptr) {
            return message + ": " + err->toString();
        }
        return message;
    }

    [[nodiscard]] std::unique_ptr<Error> clone() const noexcept override
    {
        auto wrapped = std::make_unique<WrappedError>();
        wrapped->err = err->clone();
        wrapped->message = message;
#if defined(__GNUC__) && !defined(__clang__)
        return wrapped;
#else
        return std::move(wrapped);
#endif
    }
};

} // namespace

std::string IOError::toString() const noexcept
{
    return reason + ": " + std::make_error_code(kind).message();
}

std::unique_ptr<Error> IOError::clone() const noexcept
{
    auto err = std::make_unique<IOError>();
    err->kind = kind;
    err->reason = reason;
#if defined(__GNUC__) && !defined(__clang__)
    return err;
#else
    return std::move(err);
#endif
}

std::unique_ptr<IOError> makeIOError(std::errc kind, std::string&& reason) noexcept
{
    auto err = std::make_unique<IOError>();
    err->kind = kind;
    err->reason = std::move(reason);
    return err;
}

std::unique_ptr<Error> make(std::string&& message) noexcept
{
    auto err = std::make_unique<StringError>();
    err->message = std::move(message);
#if defined(__GNUC__) && !defined(__clang__)
    return err;
#else
    return std::move(err);
#endif
}

std::unique_ptr<Error> wrap(std::unique_ptr<Error>&& err, std::string&& message) noexcept
{
    auto wrapped = std::make_unique<WrappedError>();
    wrapped->err = std::move(err);
    wrapped->message = std::move(message);
#if defined(__GNUC__) && !defined(__clang__)
    return wrapped;
#else
    return std::move(wrapped);
#endif
}

} // namespace pomdog::errors

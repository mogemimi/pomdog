// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/errors.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/string_helper.hpp"

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
    std::string Message;

    [[nodiscard]] std::string ToString() const noexcept override
    {
        return this->Message;
    }

    [[nodiscard]] std::unique_ptr<Error> Clone() const noexcept override
    {
        auto err = std::make_unique<StringError>();
        err->Message = this->Message;
#if defined(__GNUC__) && !defined(__clang__)
        return err;
#else
        return std::move(err);
#endif
    }
};

class WrappedError final : public Error {
public:
    std::unique_ptr<Error> Err;
    std::string Message;

    [[nodiscard]] std::string ToString() const noexcept override
    {
        if (this->Err != nullptr) {
            return this->Message + ": " + this->Err->ToString();
        }
        return this->Message;
    }

    [[nodiscard]] std::unique_ptr<Error> Clone() const noexcept override
    {
        auto err = std::make_unique<WrappedError>();
        err->Err = this->Err->Clone();
        err->Message = this->Message;
#if defined(__GNUC__) && !defined(__clang__)
        return err;
#else
        return std::move(err);
#endif
    }
};

} // namespace

std::string IOError::ToString() const noexcept
{
    return this->Reason + ": " + std::make_error_code(this->Kind).message();
}

std::unique_ptr<Error> IOError::Clone() const noexcept
{
    auto err = std::make_unique<IOError>();
    err->Kind = this->Kind;
    err->Reason = this->Reason;
#if defined(__GNUC__) && !defined(__clang__)
    return err;
#else
    return std::move(err);
#endif
}

std::unique_ptr<IOError> New(std::errc kind, std::string&& reason) noexcept
{
    auto err = std::make_unique<IOError>();
    err->Kind = kind;
    err->Reason = std::move(reason);
    return err;
}

std::unique_ptr<Error> New(std::string&& message) noexcept
{
    auto err = std::make_unique<StringError>();
    err->Message = std::move(message);
#if defined(__GNUC__) && !defined(__clang__)
    return err;
#else
    return std::move(err);
#endif
}

std::unique_ptr<Error> Wrap(std::unique_ptr<Error>&& err, std::string&& message) noexcept
{
    auto wrapped = std::make_unique<WrappedError>();
    wrapped->Err = std::move(err);
    wrapped->Message = std::move(message);
#if defined(__GNUC__) && !defined(__clang__)
    return wrapped;
#else
    return std::move(wrapped);
#endif
}

} // namespace pomdog::errors

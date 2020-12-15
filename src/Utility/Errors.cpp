// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/Errors.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <string>
#include <utility>

namespace Pomdog {
Error::~Error() noexcept = default;
} // namespace Pomdog

namespace Pomdog::Errors {

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
        return err;
    }
};

class WrappedError final : public Error {
public:
    std::shared_ptr<Error> Err;
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
        return err;
    }
};

std::string IOError::ToString() const noexcept
{
    return this->Reason + ": " + std::make_error_code(this->Kind).message();
}

std::unique_ptr<Error> IOError::Clone() const noexcept
{
    auto err = std::make_unique<IOError>();
    err->Kind = this->Kind;
    err->Reason = this->Reason;
    return err;
}

std::shared_ptr<IOError> New(std::errc kind, std::string&& reason) noexcept
{
    auto err = std::make_shared<IOError>();
    err->Kind = kind;
    err->Reason = std::move(reason);
    return err;
}

std::shared_ptr<Error> New(std::string&& message) noexcept
{
    auto err = std::make_shared<StringError>();
    err->Message = std::move(message);
    return std::move(err);
}

std::shared_ptr<Error> Wrap(const std::shared_ptr<Error>& err, std::string&& message) noexcept
{
    auto wrapped = std::make_shared<WrappedError>();
    wrapped->Err = err;
    wrapped->Message = std::move(message);
    return std::move(wrapped);
}

std::shared_ptr<Error> Wrap(std::shared_ptr<Error>&& err, std::string&& message) noexcept
{
    auto wrapped = std::make_shared<WrappedError>();
    wrapped->Err = std::move(err);
    wrapped->Message = std::move(message);
    return std::move(wrapped);
}

} // namespace Pomdog::Errors

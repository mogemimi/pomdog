// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/Errors.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <string>
#include <utility>

namespace Pomdog::Errors {

class StringError final : public Error {
public:
    std::string Message;

    [[nodiscard]] std::string ToString() const noexcept
    {
        return this->Message;
    }
};

class WrappedError final : public Error {
public:
    std::shared_ptr<Error> Err;
    std::string Message;

    [[nodiscard]] std::string ToString() const noexcept
    {
        if (this->Err != nullptr) {
            return this->Message + ": " + this->Err->ToString();
        }
        return this->Message;
    }
};

std::string IOError::ToString() const noexcept
{
    return this->Reason + ": " + std::make_error_code(this->Kind).message();
}

std::shared_ptr<IOError> New(std::errc kind, std::string&& reason)
{
    auto err = std::make_shared<IOError>();
    err->Kind = kind;
    err->Reason = std::move(reason);
    return err;
}

std::shared_ptr<Error> New(std::string&& message)
{
    auto err = std::make_shared<StringError>();
    err->Message = std::move(message);
    return err;
}

std::shared_ptr<Error> Wrap(const std::shared_ptr<Error>& err, std::string&& message)
{
    auto wrapped = std::make_shared<WrappedError>();
    wrapped->Err = err;
    wrapped->Message = std::move(message);
    return wrapped;
}

std::shared_ptr<Error> Wrap(std::shared_ptr<Error>&& err, std::string&& message)
{
    auto wrapped = std::make_shared<WrappedError>();
    wrapped->Err = std::move(err);
    wrapped->Message = std::move(message);
    return wrapped;
}

} // namespace Pomdog::Errors

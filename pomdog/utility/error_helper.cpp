// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/errors.h"

namespace pomdog::errors {

[[nodiscard]] std::errc
toErrc(int err) noexcept
{
    static_assert(std::errc::bad_address == static_cast<std::errc>(EFAULT));
    static_assert(std::errc::bad_file_descriptor == static_cast<std::errc>(EBADF));
    static_assert(std::errc::invalid_argument == static_cast<std::errc>(EINVAL));
    static_assert(std::errc::not_a_socket == static_cast<std::errc>(ENOTSOCK));
    static_assert(std::errc::no_protocol_option == static_cast<std::errc>(ENOPROTOOPT));
    static_assert(std::errc::operation_in_progress == static_cast<std::errc>(EINPROGRESS));
    static_assert(std::errc::resource_unavailable_try_again == static_cast<std::errc>(EAGAIN));
    static_assert(std::errc::operation_would_block == static_cast<std::errc>(EWOULDBLOCK));
    static_assert(std::errc::interrupted == static_cast<std::errc>(EINTR));
    static_assert(std::errc::timed_out == static_cast<std::errc>(ETIMEDOUT));

    return static_cast<std::errc>(err);
}

[[nodiscard]] std::unique_ptr<Error>
fromErrc(const std::errc& errorCode) noexcept
{
    return errors::make(std::make_error_code(errorCode).message());
}

[[nodiscard]] std::unique_ptr<Error>
fromErrc(const std::errc& errorCode, std::string&& message) noexcept
{
    auto err = errors::make(std::make_error_code(errorCode).message());
    return errors::wrap(std::move(err), std::move(message));
}

} // namespace pomdog::errors

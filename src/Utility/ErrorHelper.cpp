// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "ErrorHelper.hpp"

namespace Pomdog::Detail {

std::errc ToErrc(int err) noexcept
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

} // namespace Pomdog::Detail

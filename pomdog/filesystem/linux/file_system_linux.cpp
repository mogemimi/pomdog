// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/linux/file_system_linux.h"
#include "pomdog/filesystem/posix/file_system_posix.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::linux {

[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectory(const std::string& path) noexcept
{
    return posix::createDirectory(path, S_IRWXU);
}

[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectories(const std::string& path) noexcept
{
    return posix::createDirectories(path, S_IRWXU);
}

[[nodiscard]] bool exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return ::access(path.data(), F_OK) != -1;
}

[[nodiscard]] bool isDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    struct stat st;
    if (::stat(path.data(), &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept
{
    struct ::stat st;
    if (::stat(path.data(), &st) != 0) {
        const auto err = errors::toErrc(errno);
        return std::make_tuple(0, errors::fromErrc(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept
{
    return std::make_tuple("", errors::make("not implemented yet"));
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept
{
    return std::make_tuple("", errors::make("not implemented yet"));
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept
{
    std::array<char, PATH_MAX + 1> buf;
    std::fill(std::begin(buf), std::end(buf), 0);
    auto size = ::readlink("/proc/self/exe", buf.data(), PATH_MAX);
    if (size < 0) {
        return std::make_tuple("", errors::make("readlink() failed"));
    }

    std::string_view executablePath{buf.data(), static_cast<std::size_t>(size)};
    std::string dir{filepaths::getDirectoryName(executablePath)};
    return std::make_tuple(std::move(dir), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept
{
    return std::make_tuple(P_tmpdir, nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept
{
    char dir[PATH_MAX];
    if (::getcwd(dir, sizeof(dir)) == nullptr) {
        const auto err = errors::toErrc(errno);
        return std::make_tuple("", errors::fromErrc(err, "::getcwd() failed"));
    }
    return std::make_tuple(dir, nullptr);
}

} // namespace pomdog::detail::linux

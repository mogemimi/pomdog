// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/linux/file_system_linux.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/path_helper.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>

namespace pomdog::detail::linux {

std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept
{
    if (path.empty()) {
        return errors::New("path is empty");
    }
    if (::mkdir(path.data(), S_IRWXU) != 0) {
        auto err = detail::ToErrc(errno);
        return errors::New(err, "::mkdir() failed");
    }
    return nullptr;
}

std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept
{
    if (path.empty()) {
        return errors::New("path is empty");
    }

    auto tmp = path;
    if (tmp.back() == '/') {
        tmp.pop_back();
    }
    if (tmp.empty()) {
        return errors::New("tmp is empty");
    }

    for (auto iter = std::next(std::begin(tmp), 1); iter != std::end(tmp); iter++) {
        if (*iter == '/') {
            *iter = 0;
            if (::mkdir(tmp.data(), S_IRWXU) != 0) {
                auto err = detail::ToErrc(errno);
                return errors::New(err, "::mkdir() failed");
            }
            *iter = '/';
        }
    }

    if (::mkdir(tmp.data(), S_IRWXU) != 0) {
        auto err = detail::ToErrc(errno);
        return errors::New(err, "::mkdir() failed");
    }
    return nullptr;
}

bool Exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return ::access(path.data(), F_OK) != -1;
}

bool IsDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    struct stat st;
    if (::stat(path.data(), &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

std::tuple<std::size_t, std::unique_ptr<Error>>
GetFileSize(const std::string& path) noexcept
{
    struct ::stat st;
    if (::stat(path.data(), &st) != 0) {
        auto err = detail::ToErrc(errno);
        return std::make_tuple(0, errors::New(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept
{
    return std::make_tuple("", errors::New("not implemented yet"));
}

std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept
{
    return std::make_tuple("", errors::New("not implemented yet"));
}

std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept
{
    std::array<char, PATH_MAX + 1> buf;
    std::fill(std::begin(buf), std::end(buf), 0);
    auto size = ::readlink("/proc/self/exe", buf.data(), PATH_MAX);
    if (size < 0) {
        return std::make_tuple("", errors::New("readlink() failed"));
    }

    std::string_view executablePath{buf.data(), static_cast<std::size_t>(size)};
    std::string dir{PathHelper::GetDirectoryName(executablePath)};
    return std::make_tuple(std::move(dir), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetTempDirectoryPath() noexcept
{
    return std::make_tuple(P_tmpdir, nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetCurrentWorkingDirectory() noexcept
{
    char dir[PATH_MAX];
    if (::getcwd(dir, sizeof(dir)) == nullptr) {
        auto err = detail::ToErrc(errno);
        return std::make_tuple("", errors::New(err, "::getcwd() failed"));
    }
    return std::make_tuple(dir, nullptr);
}

} // namespace pomdog::detail::linux

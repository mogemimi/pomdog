// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/emscripten/file_system_emscripten.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/error_helper.hpp"
#include "pomdog/utility/path_helper.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>

namespace Pomdog::Detail::Emscripten {

std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept
{
    if (path.empty()) {
        return Errors::New("path is empty");
    }
    if (::mkdir(path.data(), S_IRWXU) != 0) {
        auto err = Detail::ToErrc(errno);
        return Errors::New(err, "::mkdir() failed");
    }
    return nullptr;
}

std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept
{
    if (path.empty()) {
        return Errors::New("path is empty");
    }

    auto tmp = path;
    if (tmp.back() == '/') {
        tmp.pop_back();
    }
    if (tmp.empty()) {
        return Errors::New("tmp is empty");
    }

    for (auto iter = std::next(std::begin(tmp), 1); iter != std::end(tmp); iter++) {
        if (*iter == '/') {
            *iter = 0;
            if (::mkdir(tmp.data(), S_IRWXU) != 0) {
                auto err = Detail::ToErrc(errno);
                return Errors::New(err, "::mkdir() failed");
            }
            *iter = '/';
        }
    }

    if (::mkdir(tmp.data(), S_IRWXU) != 0) {
        auto err = Detail::ToErrc(errno);
        return Errors::New(err, "::mkdir() failed");
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
        auto err = Detail::ToErrc(errno);
        return std::make_tuple(0, Errors::New(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept
{
    return std::make_tuple("", Errors::New("not implemented yet"));
}

std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept
{
    return std::make_tuple("", Errors::New("not implemented yet"));
}

std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept
{
    std::array<char, PATH_MAX + 1> buf;
    std::fill(std::begin(buf), std::end(buf), 0);
    auto size = ::readlink("/proc/self/exe", buf.data(), PATH_MAX);
    if (size < 0) {
        return std::make_tuple("", Errors::New("readlink() failed"));
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
        auto err = Detail::ToErrc(errno);
        return std::make_tuple("", Errors::New(err, "::getcwd() failed"));
    }
    return std::make_tuple(dir, nullptr);
}

} // namespace Pomdog::Detail::Emscripten

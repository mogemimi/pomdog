// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/emscripten/file_system_emscripten.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {

[[nodiscard]] std::unique_ptr<Error>
createNewDirectory(const std::string& path) noexcept
{
    if (path.empty()) {
        return errors::make("path is empty");
    }
    if (::mkdir(path.data(), S_IRWXU) != 0) {
        const auto err = errors::toErrc(errno);
        return errors::fromErrc(err, "::mkdir() failed");
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
createDirectories(const std::string& path) noexcept
{
    if (path.empty()) {
        return errors::make("path is empty");
    }

    auto tmp = path;
    if (tmp.back() == '/') {
        tmp.pop_back();
    }
    if (tmp.empty()) {
        return errors::make("tmp is empty");
    }

    for (auto iter = std::next(std::begin(tmp), 1); iter != std::end(tmp); iter++) {
        if (*iter == '/') {
            *iter = 0;
            if (::mkdir(tmp.data(), S_IRWXU) != 0 && errno != EEXIST) {
                // NOTE: If the directory already exists, we ignore the error
                //       and continue creating the next directory.
                const auto err = errors::toErrc(errno);
                return errors::fromErrc(err, "::mkdir() failed");
            }
            *iter = '/';
        }
    }

    if (::mkdir(tmp.data(), S_IRWXU) != 0 && errno != EEXIST) {
        // NOTE: If the directory already exists, we ignore the error and continue.
        const auto err = errors::toErrc(errno);
        return errors::fromErrc(err, "::mkdir() failed");
    }
    return nullptr;
}

[[nodiscard]] bool
exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return ::access(path.data(), F_OK) != -1;
}

[[nodiscard]] bool
isDirectory(const std::string& path) noexcept
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
    return std::make_tuple(".", nullptr);
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

void mountSaveDataFS() noexcept
{
    // NOTE: Mounting is handled in preload.js to ensure it happens
    // before main() runs. See preload.js for the actual IDBFS mount.
}

void unmountSaveDataFS() noexcept
{
    EM_ASM(
        Module.syncDone = 0;
        FS.syncfs(
            function(err) {
                if (err) {
                    console.log('Error: FS.syncfs failed', err);
                }
                console.log('unmountSaveDataFS syncfs');
                Module.syncDone = 1;
            });
        FS.unmount('/savedata'););
}

void flushSaveDataFS() noexcept
{
    EM_ASM(
        Module.syncDone = 0;
        FS.syncfs(
            function(err) {
                if (err) {
                    console.log('Error: FS.syncfs failed', err);
                }
                console.log('flushSaveDataFS syncfs');
                Module.syncDone = 1;
            }););
}

[[nodiscard]] bool
isSyncFS() noexcept
{
    return (emscripten_run_script_int("Module.syncDone") == 1);
}

} // namespace pomdog::detail::emscripten

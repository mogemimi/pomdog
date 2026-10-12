// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/posix/file_system_posix.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <sys/stat.h>
#include <cerrno>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::posix {

using platformfs::DirectoryCreation;

std::tuple<DirectoryCreation, std::unique_ptr<Error>>
createDirectory(const std::string& path, ::mode_t mode) noexcept
{
    if (path.empty()) {
        return {DirectoryCreation{}, errors::make("path is empty")};
    }

    if (::mkdir(path.c_str(), mode) == 0) {
        return {DirectoryCreation::Created, nullptr};
    }
    if (const auto errnum = errno; errnum != EEXIST) {
        return {DirectoryCreation{}, errors::fromErrc(errors::toErrc(errnum), "::mkdir() failed: " + path)};
    }

    // NOTE: mkdir() reports EEXIST for a file of any type at the path, so tell
    // a directory apart from the others.
    struct ::stat st = {};
    if (::stat(path.c_str(), &st) != 0) {
        return {DirectoryCreation{}, errors::fromErrc(errors::toErrc(errno), "::stat() failed: " + path)};
    }
    if (!S_ISDIR(st.st_mode)) {
        return {DirectoryCreation{}, errors::make("a file that is not a directory exists at the path: " + path)};
    }
    return {DirectoryCreation::AlreadyExists, nullptr};
}

std::tuple<DirectoryCreation, std::unique_ptr<Error>>
createDirectories(const std::string& path, ::mode_t mode) noexcept
{
    if (path.empty()) {
        return {DirectoryCreation{}, errors::make("path is empty")};
    }

    // NOTE: Remove trailing slashes except for the root directory itself.
    std::string_view target = path;
    while ((target.size() > 1) && target.ends_with('/')) {
        target.remove_suffix(1);
    }

    // NOTE: Create each parent directory in turn, starting after a leading
    // slash. A parent that already exists as a directory is fine.
    for (auto pos = target.find('/', 1); pos != std::string_view::npos; pos = target.find('/', pos + 1)) {
        if (auto [creation, err] = createDirectory(std::string{target.substr(0, pos)}, mode); err != nullptr) {
            return {DirectoryCreation{}, errors::wrap(std::move(err), "failed to create a parent directory of " + path)};
        }
    }

    // NOTE: The result for the directory itself tells whether this call created it.
    return createDirectory(std::string{target}, mode);
}

} // namespace pomdog::detail::posix

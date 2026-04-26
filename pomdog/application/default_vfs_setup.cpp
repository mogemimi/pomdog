// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/default_vfs_setup.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/vfs/file_archive.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

std::tuple<std::shared_ptr<vfs::FileSystemContext>, std::unique_ptr<Error>>
setupDefaultVFS(const DefaultVFSSetupConfig& config) noexcept
{
    std::string archiveFile = config.archiveFile;

    if (archiveFile.empty()) {
        auto [resourceDir, resourceDirErr] = platformfs::getResourceDirectoryPath();
        if (resourceDirErr != nullptr) {
            return {nullptr, errors::wrap(std::move(resourceDirErr), "failed to get resource directory path")};
        }
        archiveFile = filepaths::join(resourceDir, "content.idx");
    }

    auto [fs, fsErr] = vfs::create();
    if (fsErr != nullptr) {
        return {nullptr, errors::wrap(std::move(fsErr), "failed to create VFS")};
    }

    if (!archiveFile.empty()) {
        const auto replaceExtension = [](std::string_view filename, std::string_view newExtension) -> std::string {
            auto [base, ext] = filepaths::splitExtensionAsView(filename);
            auto baseStr = std::string(base);
            baseStr += newExtension;
            return baseStr;
        };

        auto [vol, volErr] = vfs::openArchiveFile(archiveFile, replaceExtension(archiveFile, ".pak"));
        if (volErr != nullptr) {
            return {nullptr, errors::wrap(std::move(volErr), "failed to open archive file")};
        }
        if (auto mountErr = vfs::mount(fs, config.mountPoint, std::move(vol), {.readOnly = true, .hashKeyLookup = true}); mountErr != nullptr) {
            return {nullptr, errors::wrap(std::move(mountErr), "failed to mount archive")};
        }
    }

    if (!config.assetsDir.empty()) {
        if (auto mountErr = vfs::mount(fs, config.mountPoint, config.assetsDir, {.readOnly = true, .overlayFS = true}); mountErr != nullptr) {
            return {nullptr, errors::wrap(std::move(mountErr), "failed to mount assets directory")};
        }
    }

    return {std::move(fs), nullptr};
}

} // namespace pomdog

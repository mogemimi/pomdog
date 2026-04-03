// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/game_controller_db_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/game_controller_db.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

std::tuple<std::shared_ptr<GameControllerDB>, std::unique_ptr<Error>>
loadGameControllerDB(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept
{
    const std::string path{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "failed to open file, " + path));
    }

    auto [fileInfo, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(statErr), "failed to get file info, " + path));
    }

    if (fileInfo.size == 0) {
        return std::make_tuple(nullptr, errors::make("the game controller DB file is empty: " + path));
    }

    constexpr std::size_t maxFileSize = 64 * 1024 * 1024; // 64 MB
    if (fileInfo.size > maxFileSize) {
        return std::make_tuple(nullptr, errors::make("the game controller DB file is too large: " + path));
    }

    auto storage = std::make_unique<u8[]>(fileInfo.size);
    auto [bytesRead, readErr] = file->read(std::span<u8>{storage.get(), fileInfo.size});
    if (readErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(readErr), "failed to read the file " + path));
    }

    auto [db, err] = createGameControllerDB(std::move(storage), bytesRead);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create game controller DB: " + path));
    }
    return std::make_tuple(std::move(db), nullptr);
}

} // namespace pomdog

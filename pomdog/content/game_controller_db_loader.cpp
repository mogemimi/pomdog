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

    constexpr std::size_t maxFileSize = 64 * 1024 * 1024; // 64 MB
    auto [size, sizeErr] = file->getSize();
    if (sizeErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(sizeErr), "failed to get file size: " + path));
    }
    if (size > maxFileSize) {
        return std::make_tuple(nullptr, errors::make("the game controller DB file is too large: " + path));
    }

    auto [data, viewErr] = file->view();
    if (viewErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(viewErr), "failed to read the file " + path));
    }

    if (data.empty()) {
        return std::make_tuple(nullptr, errors::make("the game controller DB file is empty: " + path));
    }

    if (data.size() > maxFileSize) {
        return std::make_tuple(nullptr, errors::make("the game controller DB file is too large: " + path));
    }

    // NOTE: The DB keeps the file open and reads its view in place.
    auto [db, err] = createGameControllerDB(std::move(file), data);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create game controller DB: " + path));
    }
    return std::make_tuple(std::move(db), nullptr);
}

} // namespace pomdog

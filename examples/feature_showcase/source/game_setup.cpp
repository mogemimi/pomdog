#include "game_setup.h"
#include "game_main.h"
#include "pomdog/application/default_vfs_setup.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/graphics_backend_helper.h"
#include "pomdog/utility/cli_parser.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog;

class GameSetupImpl final : public GameSetup {
private:
    std::shared_ptr<vfs::FileSystemContext> fs_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    configure(GameHostOptions& options, std::span<const char* const> args) override
    {
        // NOTE: Parse command-line arguments and set up VFS.
        DefaultVFSSetupConfig vfsConfig = {};
        std::string graphicsBackend = {};
        {
            CLIParser cli;
            cli.add(&vfsConfig.assetsDir, "assets-dir", "path to the assets directory");
            cli.add(&vfsConfig.archiveFile, "archive-file", "path to the archive file (without extension)");
            cli.add(&graphicsBackend, "graphics-backend", "graphics backend to use (e.g. d3d11, gl4, vulkan, metal, webgl)");
            if (auto err = cli.parse(args); err != nullptr) {
                return errors::wrap(std::move(err), "failed to parse command-line arguments");
            }
        }
        if (auto [fs, err] = setupDefaultVFS(vfsConfig); err != nullptr) {
            return errors::wrap(std::move(err), "failed to set up VFS");
        }
        else {
            fs_ = std::move(fs);
        }

        // NOTE: Load game controller database from VFS.
        if (auto [db, err] = loadGameControllerDB(fs_, "/assets/game_controller_db/gamecontrollerdb.gcdb"); err == nullptr) {
            options.gameControllerDB = std::move(db);
        }

        // NOTE: Set graphics backend if specified in command-line arguments.
        if (!graphicsBackend.empty()) {
            if (auto backend = parseGraphicsBackend(graphicsBackend); backend.has_value()) {
                options.graphicsBackend = *backend;
            }
        }

        // NOTE: Configure GameHost options
        options.backBufferWidth = 800;
        options.backBufferHeight = 480;

        return nullptr;
    }

    [[nodiscard]] std::unique_ptr<Game>
    createGame() override
    {
        return std::make_unique<GameMain>(fs_);
    }
};

} // namespace

[[nodiscard]] std::unique_ptr<pomdog::GameSetup>
createGameSetup()
{
    return std::make_unique<GameSetupImpl>();
}

} // namespace feature_showcase

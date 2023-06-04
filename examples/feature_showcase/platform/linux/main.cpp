#include "game_main.h"
#include "pomdog/platform/x11/bootstrap_x11.h"
#include "pomdog/pomdog.h"

int main(int argc, char** argv)
{
    using namespace pomdog;

#if defined(DEBUG) && !defined(NDEBUG)
    Log::SetLevel(LogLevel::Internal);
    ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
        std::printf("%s\n", entry.Message.c_str());
    });
#endif

    pomdog::x11::Bootstrap bootstrap;
    bootstrap.setBackBufferSize(800, 480);

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->toString());
    });

    bootstrap.run([](std::shared_ptr<GameHost> const& gameHost) {
        return std::make_unique<feature_showcase::GameMain>(gameHost);
    });

    return 0;
}

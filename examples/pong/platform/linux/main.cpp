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
    bootstrap.SetBackBufferSize(800, 480);

    bootstrap.OnError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->ToString());
    });

    bootstrap.Run([](std::shared_ptr<GameHost> const& gameHost) {
        return std::make_unique<pong::GameMain>(gameHost);
    });

    return 0;
}

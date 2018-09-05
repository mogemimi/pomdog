#include "../Source/FeatureShowcaseGame.hpp"
#include <Pomdog/Platform/X11/Bootstrap.hpp>
#include <Pomdog/Pomdog.hpp>

int main(int argc, char** argv)
{
    using namespace Pomdog;

#if defined(DEBUG) && !defined(NDEBUG)
    Log::SetLevel(LogLevel::Internal);
    ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
        std::printf("%s\n", entry.Message.c_str());
    });
#endif

    Pomdog::X11::Bootstrap bootstrap;
    bootstrap.SetBackBufferSize(800, 480);

    bootstrap.OnError([](std::exception const& e) {
        Log::Critical("Pomdog", e.what());
    });

    bootstrap.Run([](std::shared_ptr<GameHost> const& gameHost) {
        return std::make_unique<FeatureShowcase::FeatureShowcaseGame>(gameHost);
    });

    return 0;
}

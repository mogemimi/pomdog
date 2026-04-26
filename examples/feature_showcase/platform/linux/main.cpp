#include "game_setup.h"
#include "pomdog/application/x11/bootstrap_x11.h"
#include "pomdog/pomdog.h"

int main(int argc, char** argv)
{
    using namespace pomdog;

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::SetLevel(LogLevel::Internal);
    ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
        std::printf("%s\n", entry.Message.c_str());
    });
#endif

    pomdog::x11::Bootstrap bootstrap;
    bootstrap.setCommandLineArgs(argc, const_cast<const char* const*>(argv));

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->toString());
    });

    bootstrap.run(feature_showcase::createGameSetup());

    return 0;
}

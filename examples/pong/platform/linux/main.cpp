#include "game_setup.h"
#include "pomdog/application/x11/bootstrap_x11.h"
#include "pomdog/console/console.h"
#include "pomdog/pomdog.h"

int main(int argc, char** argv)
{
    using namespace pomdog;

#if defined(POMDOG_DEBUG_BUILD)
    Log::SetLevel(LogLevel::Internal);
    ScopedConnection connection = Log::Connect([](const LogEntry& entry) {
        pomdog::console::write_line(entry.Message);
    });
#endif

    pomdog::x11::Bootstrap bootstrap;
    bootstrap.setCommandLineArgs(argc, const_cast<const char* const*>(argv));

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        pomdog::console::write_error_line(err->toString());
    });

    bootstrap.run(pong::createGameSetup());

    return 0;
}

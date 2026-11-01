#import "AppDelegate.h"
#include "game_setup.h"
#include "pomdog/application/cocoa/bootstrap_cocoa.h"
#include "pomdog/console/console.h"
#include "pomdog/pomdog.h"
#include <memory>
#if defined(POMDOG_DEBUG_BUILD)
#include <iostream>
#endif

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow* window;

@end

@implementation AppDelegate {
    pomdog::cocoa::Bootstrap bootstrap;
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
    bootstrap.setWindow(self.window);

    bootstrap.onCompleted([=] {
        [self.window close];

        // Shutdown your application
        [NSApp terminate:nil];
    });

    bootstrap.onDiagnostic([](std::unique_ptr<pomdog::Error> diagnostic) {
        pomdog::console::write_error_line(diagnostic->toString());
    });

    if (auto err = bootstrap.run(feature_showcase::createGameSetup()); err != nullptr) {
#if defined(POMDOG_DEBUG_BUILD)
        pomdog::console::write_error_line(err->toString());
#endif
        // Shutdown your application
        [NSApp terminate:nil];
    }
}

- (void)applicationWillTerminate:(NSNotification*)aNotification
{
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

@end

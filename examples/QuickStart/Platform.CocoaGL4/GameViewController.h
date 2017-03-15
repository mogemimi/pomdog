#import <Cocoa/Cocoa.h>
#import <Pomdog/Platform/Cocoa/PomdogOpenGLView.hpp>

@interface GameViewController : NSViewController

@property (assign) IBOutlet PomdogOpenGLView *gameView;

@end

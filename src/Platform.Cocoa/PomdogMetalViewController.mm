// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#import "Pomdog/Platform/Cocoa/PomdogMetalViewController.hpp"

#include "GameHostMetal.hpp"
#include "GameWindowCocoa.hpp"
#include "../Application/SystemEvents.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#include "../RenderSystem.Metal/GraphicsContextMetal.hpp"
#include "../RenderSystem.Metal/GraphicsDeviceMetal.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

using Pomdog::SurfaceFormat;
using Pomdog::DepthFormat;
using Pomdog::PresentationParameters;
using Pomdog::GraphicsCommandQueue;
using Pomdog::GraphicsDevice;
using Pomdog::EventQueue;
using Pomdog::Detail::Cocoa::GameHostMetal;
using Pomdog::Detail::Cocoa::GameWindowCocoa;

@implementation PomdogMetalViewController
{
    std::shared_ptr<GameHostMetal> gameHost;
    std::shared_ptr<EventQueue> eventQueue;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    ///@todo MSAA is not implemented yet
    constexpr int multiSampleCount = 1;

    // Setup presentation parameters
    PresentationParameters presentationParameters;
    presentationParameters.BackBufferWidth = self.view.bounds.size.width;
    presentationParameters.BackBufferHeight = self.view.bounds.size.height;
    presentationParameters.PresentationInterval = 60;
    presentationParameters.MultiSampleCount = multiSampleCount;
    presentationParameters.BackBufferFormat = SurfaceFormat::B8G8R8A8_UNorm;
    presentationParameters.DepthStencilFormat = DepthFormat::Depth32_Float_Stencil8_Uint;
    presentationParameters.IsFullScreen = false;

    [self _setupMetal:presentationParameters];

    if (gameHost->IsMetalSupported()) {
        MTKView* metalView = static_cast<MTKView *>(self.view);
        metalView.delegate = self;
        [self loadAssetsPomdog:gameHost];
    }
    else {
        // Fallback to a blank NSView, an application could also fallback to OpenGL here.
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
    }
}

- (void)_setupMetal:(const PresentationParameters&)presentationParameters
{
    MTKView* metalView = static_cast<MTKView *>(self.view);
    NSWindow* nativeWindow = [metalView window];
    POMDOG_ASSERT(nativeWindow != nil);

    eventQueue = std::make_shared<EventQueue>();
    auto gameWindow = std::make_shared<GameWindowCocoa>(nativeWindow, eventQueue);

    gameHost = std::make_shared<GameHostMetal>(
        metalView, gameWindow, eventQueue, presentationParameters);
}

- (void)loadAssetsPomdog:(std::shared_ptr<Pomdog::GameHost>)gameHost
{
}

- (void)startGame:(std::shared_ptr<Pomdog::Game>)game
{
    std::function<void()> onCompleted = [=] {
        [[self.view window] close];

        // Shutdown your application
        [NSApp terminate:nil];
    };

    gameHost->InitializeGame(game, std::move(onCompleted));
}

- (void)_render
{
    gameHost->GameLoop();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
//    using Pomdog::Detail::ViewWillStartLiveResizeEvent;
    using Pomdog::Detail::ViewDidEndLiveResizeEvent;
//    if (eventQueue) {
//        eventQueue->Enqueue<ViewWillStartLiveResizeEvent>();
//    }
    if (eventQueue) {
        eventQueue->Enqueue<ViewDidEndLiveResizeEvent>();
    }
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    @autoreleasepool {
        [self _render];
    }
}

@end

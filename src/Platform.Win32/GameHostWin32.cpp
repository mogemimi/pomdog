//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "../InputSystem/InputDeviceFactory.hpp"
#include "../RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp"
#include "../RenderSystem.Direct3D11/GraphicsDeviceDirect3D11.hpp"
#include "../SoundSystem.XAudio2/AudioEngineXAudio2.hpp"
#include "../Application/SubsystemScheduler.hpp"
#include <Pomdog/Application/Game.hpp>
#include <Pomdog/Application/GameClock.hpp>
#include <Pomdog/Audio/AudioEngine.hpp>
#include <Pomdog/Content/AssetManager.hpp>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Logging/Log.hpp>

#include <thread>
#include <chrono>

namespace Pomdog {
namespace Details {
namespace Win32 {
namespace {

static void MessagePump()
{
	MSG msg;
	while (0 != ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
class GameHostWin32::Impl final {
public:
	Impl(std::shared_ptr<GameWindowWin32> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dipatcher,
		RenderSystem::PresentationParameters const& presentationParameters,
		std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactory);

	void Run(Game & game);

	void Exit();

	std::shared_ptr<Pomdog::GameWindow> Window();

	std::shared_ptr<Pomdog::GameClock> Clock(std::shared_ptr<GameHost> && gameHost);

	std::shared_ptr<Pomdog::GraphicsContext> GraphicsContext();

	std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice();

	std::shared_ptr<Pomdog::AudioEngine> AudioEngine();

	std::shared_ptr<Pomdog::AssetManager> AssetManager(std::shared_ptr<GameHost> && gameHost);

	std::shared_ptr<Pomdog::Keyboard> Keyboard();

	std::shared_ptr<Pomdog::Mouse> Mouse();

private:
	void RenderFrame(Game & game);

	void DoEvents();

	void ProcessSystemEvents(Event const& event);

	void ClientSizeChanged();

private:
	GameClock clock;
	Details::SubsystemScheduler subsystemScheduler;

	std::shared_ptr<GameWindowWin32> gameWindow;

	std::shared_ptr<SystemEventDispatcher> systemEventDispatcher;
	ScopedConnection systemEventConnection;

	std::shared_ptr<Pomdog::GraphicsContext> graphicsContext;
	std::shared_ptr<Pomdog::GraphicsDevice> graphicsDevice;
	std::unique_ptr<Pomdog::AssetManager> assetManager;
	std::shared_ptr<Pomdog::AudioEngine> audioEngine;

	std::unique_ptr<InputSystem::InputDeviceFactory> inputDeviceFactory;
	std::shared_ptr<Pomdog::Keyboard> keyboard;
	std::shared_ptr<Pomdog::Mouse> mouse;

	bool exitRequest;
	bool surfaceResizeRequest;
};
//-----------------------------------------------------------------------
GameHostWin32::Impl::Impl(std::shared_ptr<GameWindowWin32> const& window,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
	RenderSystem::PresentationParameters const& presentationParameters,
	std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactoryIn)
	: gameWindow(window)
	, systemEventDispatcher(eventDispatcher)
	, inputDeviceFactory(std::move(inputDeviceFactoryIn))
	, exitRequest(false)
	, surfaceResizeRequest(false)
{
	using Details::RenderSystem::Direct3D11::GraphicsDeviceDirect3D11;
	auto nativeGraphicsDevice = std::make_unique<GraphicsDeviceDirect3D11>();
	auto deviceContext = nativeGraphicsDevice->DeviceContext();
	auto nativeDevice = nativeGraphicsDevice->NativeDevice();
	auto dxgiFactory = nativeGraphicsDevice->DXGIFactory();

	graphicsDevice = std::make_shared<Pomdog::GraphicsDevice>(std::move(nativeGraphicsDevice));

	using Details::RenderSystem::Direct3D11::GraphicsContextDirect3D11;
	graphicsContext = std::make_shared<Pomdog::GraphicsContext>(
		std::make_unique<GraphicsContextDirect3D11>(window->NativeWindowHandle(), dxgiFactory, nativeDevice, deviceContext),
		presentationParameters, graphicsDevice);

	POMDOG_ASSERT(systemEventDispatcher);
	systemEventConnection = systemEventDispatcher->Connect([this](Event const& event) {
		ProcessSystemEvents(event);
	});

	audioEngine = std::make_shared<Pomdog::AudioEngine>();

	POMDOG_ASSERT(inputDeviceFactory);
	keyboard = inputDeviceFactory->CreateKeyboard(subsystemScheduler);
	mouse = inputDeviceFactory->CreateMouse(subsystemScheduler);

	{
		std::string rootDirectory = "Content";
		Details::AssetLoaderContext loaderContext{ rootDirectory, graphicsDevice };

		assetManager = std::make_unique<Pomdog::AssetManager>(std::move(loaderContext));
	}
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::Run(Game & game)
{
	game.Initialize();

	if (!game.CompleteInitialize()) {
		return;
	}

	while (!exitRequest)
	{
		MessagePump();
		clock.Tick();
		DoEvents();
		subsystemScheduler.OnUpdate();
		game.Update();
		RenderFrame(game);

		///@todo Not implemented
		///@todo badcode
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	gameWindow->Close();
	//DoEvents();
	MessagePump();
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::Exit()
{
	exitRequest = true;
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::RenderFrame(Game & game)
{
	POMDOG_ASSERT(gameWindow);

	if (!gameWindow || gameWindow->IsMinimized()) {
		// skip rendering
		return;
	}

	game.Draw();
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::DoEvents()
{
	systemEventDispatcher->Tick();

	if (surfaceResizeRequest) {
		ClientSizeChanged();
		surfaceResizeRequest = false;
	}
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::ProcessSystemEvents(Event const& event)
{
	if (event.Is<WindowShouldCloseEvent>())
	{
		Log::Internal("WindowShouldCloseEvent");
		this->Exit();
	}

	///@todo Not implemented
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::ClientSizeChanged()
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostWin32::Impl::Window()
{
	return gameWindow;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostWin32::Impl::Clock(std::shared_ptr<GameHost> && gameHost)
{
	std::shared_ptr<Pomdog::GameClock> sharedClock(gameHost, &clock);
	return std::move(sharedClock);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> GameHostWin32::Impl::GraphicsContext()
{
	POMDOG_ASSERT(graphicsContext);
	return graphicsContext;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostWin32::Impl::GraphicsDevice()
{
	POMDOG_ASSERT(graphicsDevice);
	return graphicsDevice;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostWin32::Impl::AudioEngine()
{
	POMDOG_ASSERT(audioEngine);
	return audioEngine;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostWin32::Impl::AssetManager(std::shared_ptr<GameHost> && gameHost)
{
	POMDOG_ASSERT(assetManager);
	std::shared_ptr<Pomdog::AssetManager> sharedAssetManager(gameHost, assetManager.get());
	return std::move(sharedAssetManager);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostWin32::Impl::Keyboard()
{
	POMDOG_ASSERT(keyboard);
	return keyboard;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostWin32::Impl::Mouse()
{
	POMDOG_ASSERT(mouse);
	return mouse;
}
//-----------------------------------------------------------------------
GameHostWin32::GameHostWin32(std::shared_ptr<GameWindowWin32> const& window,
	std::shared_ptr<SystemEventDispatcher> const& dispatcher,
	Details::RenderSystem::PresentationParameters const& presentationParameters,
	std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactory)
	: impl(std::make_unique<Impl>(window, dispatcher, presentationParameters, std::move(inputDeviceFactory)))
{}
//-----------------------------------------------------------------------
GameHostWin32::~GameHostWin32() = default;
//-----------------------------------------------------------------------
void GameHostWin32::Run(Game & game)
{
	POMDOG_ASSERT(impl);
	impl->Run(game);
}
//-----------------------------------------------------------------------
void GameHostWin32::Exit()
{
	POMDOG_ASSERT(impl);
	impl->Exit();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostWin32::Window()
{
	POMDOG_ASSERT(impl);
	return impl->Window();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostWin32::Clock()
{
	POMDOG_ASSERT(impl);
	return impl->Clock(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> GameHostWin32::GraphicsContext()
{
	POMDOG_ASSERT(impl);
	return impl->GraphicsContext();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostWin32::GraphicsDevice()
{
	POMDOG_ASSERT(impl);
	return impl->GraphicsDevice();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostWin32::AudioEngine()
{
	POMDOG_ASSERT(impl);
	return impl->AudioEngine();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostWin32::AssetManager()
{
	POMDOG_ASSERT(impl);
	return impl->AssetManager(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostWin32::Keyboard()
{
	POMDOG_ASSERT(impl);
	return impl->Keyboard();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostWin32::Mouse()
{
	POMDOG_ASSERT(impl);
	return impl->Mouse();
}
//-----------------------------------------------------------------------
}// namespace Win32
}// namespace Details
}// namespace Pomdog

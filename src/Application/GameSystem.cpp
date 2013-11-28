//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/GameSystem.hpp>
#include <Pomdog/Application/detail/SystemHub.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Logging/Log.hpp>

namespace Pomdog {

using Details::SystemHub;

enum class GameSystemState: int
{
	///@~Japanese
	/// @brief 初期化前の状態を示します。
	NotInitialized,

	///@~Japanese
	/// @brief 実行中であることを示します。
	Running,

	///@~Japanese
	/// @brief 終了処理の最中であることを示します。
	Quitting,
};
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG) || defined(POMDOG_COMPILER_GCC)
#pragma mark -
#pragma mark GameSystem::Impl
#endif
//-----------------------------------------------------------------------
class GameSystem::Impl final
{
public:
	explicit Impl(std::shared_ptr<SystemHub> const& systemHub);
	~Impl() = default;
	
	void BuildSubsystems();
	
	void Update();
	
public:
	std::shared_ptr<SystemHub> systemHub;
	
	GameSystemState state;
};
//-----------------------------------------------------------------------
GameSystem::Impl::Impl(std::shared_ptr<SystemHub> const& systemHub)
	: systemHub(systemHub)
	, state(GameSystemState::NotInitialized)
{}
//-----------------------------------------------------------------------
void GameSystem::Impl::BuildSubsystems()
{
}
//-----------------------------------------------------------------------
void GameSystem::Impl::Update()
{
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG) || defined(POMDOG_COMPILER_GCC)
#pragma mark -
#pragma mark GameSystem
#endif
//-----------------------------------------------------------------------
GameSystem::GameSystem(std::shared_ptr<SystemHub> const& systemHub)
	: impl(new Impl(systemHub))
{
	POMDOG_ASSERT(impl);
	impl->BuildSubsystems();
}
//-----------------------------------------------------------------------
GameSystem::~GameSystem()
{}
//-----------------------------------------------------------------------
void GameSystem::Update()
{
	POMDOG_ASSERT(impl);
	impl->Update();
}
//-----------------------------------------------------------------------
void GameSystem::RenderBegin()
{
}
//-----------------------------------------------------------------------
void GameSystem::Render()
{
}
//-----------------------------------------------------------------------
void GameSystem::RenderEnd()
{
}
//-----------------------------------------------------------------------
void GameSystem::Quit()
{
	impl->state = GameSystemState::Quitting;
}
//-----------------------------------------------------------------------
bool GameSystem::IsQuitting() const
{
	return GameSystemState::Quitting == impl->state;
}
//-----------------------------------------------------------------------
void GameSystem::FatalError(std::string const& message)
{
	Log::LogMessage(message, LoggingLevel::Critical);
}
//-----------------------------------------------------------------------
void GameSystem::ReportBug(std::string const& message)
{
	Log::LogMessage(message, LoggingLevel::Brief);
}
//-----------------------------------------------------------------------
void GameSystem::Warning(std::string const& message)
{
	Log::LogMessage(message, LoggingLevel::Brief);
}

}// namespace Pomdog

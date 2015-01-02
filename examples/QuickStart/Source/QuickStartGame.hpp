#ifndef QUICKSTARTGAME_HPP
#define QUICKSTARTGAME_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace QuickStart {

using namespace Pomdog;

class QuickStartGame: public Game {
public:
	explicit QuickStartGame(std::shared_ptr<GameHost> const& gameHost);

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;
	std::shared_ptr<Texture2D> texture;
	std::shared_ptr<RenderTarget2D> renderTarget;
};

}// namespace QuickStart

#endif // !defined(QUICKSTARTGAME_HPP)

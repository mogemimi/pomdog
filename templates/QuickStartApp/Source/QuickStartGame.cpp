#include "QuickStartGame.hpp"
#include <utility>

namespace QuickStart {
//-----------------------------------------------------------------------
QuickStartGame::QuickStartGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
void QuickStartGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("QuickStart Game");
	window->AllowPlayerResizing(false);

	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using VertexCombined = CustomVertex<Vector3, Vector2>;

		std::array<VertexCombined, 4> const verticesCombo = {
			Vector3(-0.8f, -0.8f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(-0.8f,  0.8f, 0.0f), Vector2(0.0f, 1.0f),
			Vector3( 0.8f,  0.8f, 0.0f), Vector2(1.0f, 1.0f),
			Vector3( 0.8f, -0.8f, 0.0f), Vector2(1.0f, 0.0f),
		};
		
		// Create vertex buffer
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			verticesCombo.data(), verticesCombo.size(),
			VertexCombined::Declaration().StrideBytes(), BufferUsage::Immutable);
	}
	{
		std::array<std::uint16_t, 6> const indices = {
			0, 1, 2,
			2, 3, 0
		};

		// Create index buffer
		indexBuffer = std::make_shared<IndexBuffer>(graphicsDevice,
			IndexElementSize::SixteenBits, indices.data(), indices.size(), BufferUsage::Immutable);
	}
	{
		effectPass = assets->Load<EffectPass>("SimpleEffect");
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
	{
		auto sampler = SamplerState::CreatePointClamp(graphicsDevice);
		graphicsContext->SetSamplerState(0, sampler);

		// Laod a PNG as texture
		texture = assets->Load<Texture2D>("pomdog.png");
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height);
	}
}
//-----------------------------------------------------------------------
void QuickStartGame::Update()
{
	static float value = 0.0f;
	value += 0.008f;
	if (value > 1.0f) {
		value = -1.0f;
	}

	Vector2 vec {std::abs(value), (1.0f + value) * 0.5f};

	auto parameter = constantBuffers->Find("TestStructure");
	parameter->SetValue(vec);

	auto vector2 = parameter->GetValue<Vector2>();
}
//-----------------------------------------------------------------------
void QuickStartGame::Draw()
{
	graphicsContext->Clear(Color::CornflowerBlue);

	graphicsContext->SetTexture(0, texture);
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	graphicsContext->SetEffectPass(effectPass);
	graphicsContext->SetConstantBuffers(constantBuffers);
	graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->IndexCount());

	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace QuickStart

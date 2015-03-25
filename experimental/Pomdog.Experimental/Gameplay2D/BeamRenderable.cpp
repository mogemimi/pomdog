// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "BeamRenderable.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
BeamRenderable::BeamRenderable()
{}
//-----------------------------------------------------------------------
void BeamRenderable::Visit(GameObject & gameObject, Renderer & renderer)
{
    ///@todo Not implemented

//    auto blendStateAdditive = BlendState::CreateAdditive(gameHost->GraphicsDevice());
//    auto blendState = graphicsContext->GetBlendState();
//    graphicsContext->SetBlendState(blendStateAdditive);
//
//    DrawBeam();
//
//    graphicsContext->SetBlendState(blendState);
}
//-----------------------------------------------------------------------
void BeamRenderable::Load(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::shared_ptr<AssetManager> const& assets)
{
    texture = assets->Load<Texture2D>("Particles/lightning.png");
}
//-----------------------------------------------------------------------
void BeamRenderable::Update(GameClock const& clock)
{
    {
        beamSystem.emitter.InterpolationPoints = 95;
        beamSystem.emitter.StartThickness = 1.374f;
        beamSystem.branching.BranchingRate = 0.134f;
        beamSystem.branching.InheritThickness = 0.5f;
        beamSystem.emitter.SwayRange = std::uniform_real_distribution<float>(-12.5f, 12.5f);
        beamSystem.branching.SpreadRange = std::uniform_real_distribution<float>(-2.357f, 2.357f);
        beamSystem.emitter.EndColor = Color{102,122,204,255};
        beamSystem.emitter.StartLifetime = 0.4f;
    }
    {
        //auto const& muzzleRifleMatrix = maidGlobalPose[35];
        auto const& muzzleRifleMatrix = Matrix3x2::Identity;

        Transform2D transform;
        transform.Scale = {1.0f, 1.0f};
        transform.Rotation = 0.0f;
        transform.Position = Vector2::Transform(Vector2{10.0f, 0.0f}, muzzleRifleMatrix);
        Vector2 target = Vector2::Transform(Vector2{500.0f, 0.0f}, muzzleRifleMatrix);

//        if (slider2->Value() > 0.4f) {
//            transform.Position = {900.0f, 1000.0f};
//            target = transform.Position + Vector2{100.0f, 100.0f};
//        }

        beamSystem.Update(clock.FrameDuration(), transform, target);
    }
}
//-----------------------------------------------------------------------
void BeamRenderable::DrawBeam()
{
//    auto transform = mainCamera->Component<Transform2D>();
//    auto camera = mainCamera->Component<Camera2D>();
//
//    POMDOG_ASSERT(transform && camera);
//    auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
//    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
//        graphicsContext->Viewport().Width(), graphicsContext->Viewport().Height(), 0.1f, 1000.0f);
//
//    gameEditor->SetViewProjection(viewMatrix * projectionMatrix);
//
//    auto const& muzzleRifleMatrix = maidGlobalPose[35];
//    auto transposeMatrix = Matrix4x4{
//        muzzleRifleMatrix(0, 0), muzzleRifleMatrix(0, 1), 0.0f, 0.0f,
//        muzzleRifleMatrix(1, 0), muzzleRifleMatrix(1, 1), 0.0f, 0.0f,
//        0.0f, 0.0f, 1.0f, 0.0f,
//        muzzleRifleMatrix(2, 0), muzzleRifleMatrix(2, 1), 0.0f, 1.0f,
//    };
//
//    POMDOG_ASSERT(spriteRenderer);
//    spriteRenderer->Begin(SpriteSortMode::Deferred, transposeMatrix * viewMatrix);
//    {
//        SpriteLine spriteLine;
//        spriteLine.Texture = beamTexture;
//        spriteLine.HalfCircleSize = {8, 32};
//        spriteLine.InverseThickness = 5.0f;
//        spriteLine.StartRectangle = {0, 0, 32, 64};
//        spriteLine.MiddleRectangle = {32, 0, 1, 64};
//        spriteLine.EndRectangle = {33, 0, 31, 64};
//
//        auto DrawBeam = [&](std::vector<Vector2> const& points, float lineThickness, Color const& color, float normalizedTime)
//        {
//            if (normalizedTime > 0.2f) {
//                for (std::size_t i = 1; i < points.size(); ++i)
//                {
//                    POMDOG_ASSERT(i > 0);
//                    auto & start = points[i - 1];
//                    auto & end = points[i];
//
//                    spriteLine.Draw(*spriteRenderer, start, end, lineThickness, color, 0);
//                }
//            }
//            else {
//                auto a = points.size() - static_cast<std::size_t>(std::max(1.0f, normalizedTime * (points.size() - 1)));
//                POMDOG_ASSERT(a < points.size());
//                for (std::size_t i = a; i < points.size(); ++i)
//                {
//                    POMDOG_ASSERT(i > 0);
//                    auto & start = points[i - 1];
//                    auto & end = points[i];
//
//                    spriteLine.Draw(*spriteRenderer, start, end, lineThickness, color, 0);
//                }
//            }
//        };
//
//        for (auto & beam: beamSystem.beams)
//        {
//            DrawBeam(beam.Points, beam.Thickness, beam.Color, beam.TimeToLive/beamSystem.emitter.StartLifetime);
//        }
//    }
//    spriteRenderer->End();
}
//-----------------------------------------------------------------------
}// namespace Pomdog

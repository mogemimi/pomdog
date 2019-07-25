// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "SkinnedMeshCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SkinnedMeshCommand.hpp"

namespace Pomdog {
namespace {

void SetMatrixPalette(SkinnedEffect & skinnedEffect, const Skeleton& skeleton, const SkeletonTransform& skeletonTransform)
{
    std::array<Matrix3x2, 64> matrices;

    for (auto & joint: skeleton)
    {
        POMDOG_ASSERT(joint.Index);
        POMDOG_ASSERT(*joint.Index < matrices.size());
        POMDOG_ASSERT(*joint.Index < skeletonTransform.GlobalPose.size());
        matrices[*joint.Index] = joint.InverseBindPose * skeletonTransform.GlobalPose[*joint.Index];
    }

    auto const minMatrixCount = std::min(matrices.size(), skeletonTransform.GlobalPose.size());
    skinnedEffect.SetBoneTransforms(matrices.data(), minMatrixCount);
}

//
//static void DrawSkeleton(const std::unique_ptr<PrimitiveBatch>& primitiveBatch,
//    const Matrix4x4& modelViewProjection)
//{
//    POMDOG_ASSERT(primitiveBatch);
//    primitiveBatch->Begin(modelViewProjection);
//
//    Pomdog::Color boneColor {160, 160, 160, 255};
//
//    POMDOG_ASSERT(skeletonTransform);
//    auto & globalPose = skeletonTransform->GlobalPose;
//
//    for (auto & joint: *skeleton)
//    {
//        auto & matrix = globalPose[*joint.Index];
//
//        if (skeleton->Root().Index != joint.Index)
//        {
//            primitiveBatch->DrawTriangle(
//                Vector2::Transform({1.7f, -4.7f}, matrix),
//                Vector2::Transform({1.7f, 4.7f}, matrix),
//                Vector2::Transform({25, 0}, matrix), boneColor);
//        }
//
//        auto center = Vector2::Transform(Vector2::Zero, matrix);
//        primitiveBatch->DrawCircle(center, 5.0f, boneColor, 18);
//        primitiveBatch->DrawCircle(center, 3.0f, Color::White, 13);
//    }
//
//    primitiveBatch->End();
//}

} // unnamed namespace

SkinnedMeshCommandProcessor::SkinnedMeshCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : skinnedEffect(*graphicsDevice, assets)
    , drawCallCount(0)
{
}

void SkinnedMeshCommandProcessor::Begin(GraphicsCommandQueue &)
{
    drawCallCount = 0;
}

void SkinnedMeshCommandProcessor::Draw(GraphicsCommandQueue & commandQueue, RenderCommand & command)
{
    using Detail::Rendering::SkinnedMeshCommand;

    auto & skinnedMeshCommand = static_cast<SkinnedMeshCommand &>(command);

    POMDOG_ASSERT(skinnedMeshCommand.skeleton);
    POMDOG_ASSERT(skinnedMeshCommand.skeletonTransform);
    POMDOG_ASSERT(skinnedMeshCommand.texture);
    POMDOG_ASSERT(skinnedMeshCommand.mesh);

    {
        skinnedEffect.SetTexture(skinnedMeshCommand.texture);
        skinnedEffect.SetColor(skinnedMeshCommand.color);
        skinnedEffect.SetWorldViewProjection(skinnedMeshCommand.localToWorld * viewProjection);
        SetMatrixPalette(skinnedEffect, *skinnedMeshCommand.skeleton, *skinnedMeshCommand.skeletonTransform);

        graphicsContext.SetVertexBuffer(skinnedMeshCommand.mesh->VertexBuffer);
        skinnedEffect.Apply(graphicsContext);

        graphicsContext.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
        graphicsContext.DrawIndexed(
            skinnedMeshCommand.mesh->IndexBuffer,
            skinnedMeshCommand.mesh->IndexBuffer->GetIndexCount());

        ++drawCallCount;
    }
//    {
//        auto rasterizerStateOld = graphicsContext->GetRasterizerState();
//
//        RasterizerDescription rasterizerDesc;
//        rasterizerDesc.FillMode = FillMode::WireFrame;
//        auto rasterizerState = std::make_shared<RasterizerState>(graphicsDevice, rasterizerDesc);
//
//        graphicsContext->SetRasterizerState(rasterizerState);
//
//        graphicsContext->SetTexture(0, texture);
//        graphicsContext->SetInputLayout(inputLayout);
//        graphicsContext->SetVertexBuffer(mesh->VertexBuffer);
//        skinningEffect->Apply();
//        graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
//                                     mesh->IndexBuffer, mesh->IndexBuffer->GetIndexCount());
//
//        graphicsContext->SetRasterizerState(rasterizerStateOld);
//    }
}

void SkinnedMeshCommandProcessor::End(GraphicsCommandQueue &)
{
}

int SkinnedMeshCommandProcessor::GetDrawCallCount() const noexcept
{
    return drawCallCount;
}

void SkinnedMeshCommandProcessor::SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection)
{
    viewProjection = view * projection;
}

} // namespace Pomdog

//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkinnedMeshCommand.hpp"

namespace Pomdog {
namespace Details {
namespace Rendering {
//-----------------------------------------------------------------------
void SkinnedMeshCommand::Execute(GraphicsContext & graphicsContext)
{
	{
		graphicsContext.SetVertexBuffer(mesh->VertexBuffer);
		skinnedEffect.Apply(graphicsContext);
		graphicsContext.DrawIndexed(PrimitiveTopology::TriangleList,
			mesh->IndexBuffer, mesh->IndexBuffer->IndexCount());
	}
//	{
//		auto rasterizerStateOld = graphicsContext->GetRasterizerState();
//	
//		RasterizerDescription rasterizerDesc;
//		rasterizerDesc.FillMode = FillMode::WireFrame;
//		auto rasterizerState = std::make_shared<RasterizerState>(graphicsDevice, rasterizerDesc);
//		
//		graphicsContext->SetRasterizerState(rasterizerState);
//		
//		graphicsContext->SetTexture(0, texture);
//		graphicsContext->SetInputLayout(inputLayout);
//		graphicsContext->SetVertexBuffer(mesh->VertexBuffer);
//		skinningEffect->Apply();
//		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
//									 mesh->IndexBuffer, mesh->IndexBuffer->IndexCount());
//
//		graphicsContext->SetRasterizerState(rasterizerStateOld);
//	}
}
//-----------------------------------------------------------------------
void SkinnedMeshCommand::SetMatrixPalette(Skeleton const& skeleton, SkeletonTransform const& skeletonTransform)
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
//-----------------------------------------------------------------------
RenderCommandType SkinnedMeshCommand::CommandType() const
{
	return RenderCommandType::Custom;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Details
}// namespace Pomdog

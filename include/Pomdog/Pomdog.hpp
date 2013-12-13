//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_89D6FE57_7B46_4032_92AE_EA4D52643E23_HPP
#define POMDOG_89D6FE57_7B46_4032_92AE_EA4D52643E23_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Platform.hpp>
#include <Pomdog/Config/Export.hpp>
#include <Pomdog/Config/FundamentalTypes.hpp>

#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Utility/Noncopyable.hpp>

// Application
#include <Pomdog/Application/Game.hpp>
#include <Pomdog/Application/GameHost.hpp>
#include <Pomdog/Application/GameWindow.hpp>

// Event
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventCode.hpp>
#include <Pomdog/Event/EventCodeHelper.hpp>
#include <Pomdog/Event/EventHandler.hpp>
#include <Pomdog/Event/EventQueue.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>

// Math
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/ContainmentType.hpp>
#include <Pomdog/Math/Degree.hpp>
#include <Pomdog/Math/MathHelper.hpp>
#include <Pomdog/Math/Matrix2x2.hpp>
#include <Pomdog/Math/Matrix3x3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Point2D.hpp>
#include <Pomdog/Math/Point3D.hpp>
#include <Pomdog/Math/Quaternion.hpp>
#include <Pomdog/Math/Radian.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>

// Logging
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Logging/LogEntry.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Logging/LoggingLevel.hpp>

// GamePlay
#include <Pomdog/GamePlay/GameObject.hpp>

// Graphics
#include <Pomdog/Graphics/Blend.hpp>
#include <Pomdog/Graphics/BlendDescription.hpp>
#include <Pomdog/Graphics/BlendFunction.hpp>
#include <Pomdog/Graphics/BlendState.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/ClearOptions.hpp>
#include <Pomdog/Graphics/ComparisonFunction.hpp>
#include <Pomdog/Graphics/CullMode.hpp>
#include <Pomdog/Graphics/DepthStencilDescription.hpp>
#include <Pomdog/Graphics/DepthStencilOperation.hpp>
#include <Pomdog/Graphics/DepthStencilState.hpp>
#include <Pomdog/Graphics/FillMode.hpp>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Graphics/IndexElementSize.hpp>
#include <Pomdog/Graphics/PrimitiveTopology.hpp>
#include <Pomdog/Graphics/RasterizerDescription.hpp>
#include <Pomdog/Graphics/RasterizerState.hpp>
#include <Pomdog/Graphics/SamplerDescription.hpp>
#include <Pomdog/Graphics/SamplerState.hpp>
#include <Pomdog/Graphics/StencilOperation.hpp>
#include <Pomdog/Graphics/TextureAddressMode.hpp>
#include <Pomdog/Graphics/TextureFilter.hpp>
#include <Pomdog/Graphics/VertexDeclaration.hpp>
#include <Pomdog/Graphics/VertexElement.hpp>
#include <Pomdog/Graphics/VertexElementFormat.hpp>
#include <Pomdog/Graphics/VertexElementUsage.hpp>
#include <Pomdog/Graphics/Viewport.hpp>

// Input
#include <Pomdog/Input/ButtonState.hpp>
#include <Pomdog/Input/Mouse.hpp>
#include <Pomdog/Input/MouseState.hpp>

#endif // !defined(POMDOG_89D6FE57_7B46_4032_92AE_EA4D52643E23_HPP)

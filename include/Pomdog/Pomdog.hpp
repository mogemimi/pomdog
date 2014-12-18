//
//  Copyright (C) 2013-2014 mogemimi.
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

// Application
#include "Application/DurationSeconds.hpp"
#include "Application/Game.hpp"
#include "Application/GameClock.hpp"
#include "Application/GameHost.hpp"
#include "Application/GameWindow.hpp"
#include "Application/Timer.hpp"

// Audio
#include "Audio/AudioChannels.hpp"
#include "Audio/AudioClip.hpp"
#include "Audio/AudioEmitter.hpp"
#include "Audio/AudioEngine.hpp"
#include "Audio/AudioListener.hpp"
#include "Audio/SoundEffect.hpp"
#include "Audio/SoundState.hpp"

// Content
#include "Content/AssetManager.hpp"

// Event
#include "Event/Event.hpp"
#include "Event/EventConnection.hpp"
#include "Event/EventHandler.hpp"
#include "Event/EventQueue.hpp"
#include "Event/ScopedConnection.hpp"
#include "Event/Signal.hpp"

// Math
#include "Math/Color.hpp"
#include "Math/ContainmentType.hpp"
#include "Math/Degree.hpp"
#include "Math/MathHelper.hpp"
#include "Math/Matrix2x2.hpp"
#include "Math/Matrix3x2.hpp"
#include "Math/Matrix3x3.hpp"
#include "Math/Matrix4x4.hpp"
#include "Math/Point2D.hpp"
#include "Math/Point3D.hpp"
#include "Math/Quaternion.hpp"
#include "Math/Radian.hpp"
#include "Math/Rectangle.hpp"
#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"
#include "Math/Vector4.hpp"

// Logging
#include "Logging/Log.hpp"
#include "Logging/LogChannel.hpp"
#include "Logging/LogEntry.hpp"
#include "Logging/LogLevel.hpp"
#include "Logging/LogStream.hpp"

// Gameplay
#include "Gameplay/Component.hpp"
#include "Gameplay/GameObject.hpp"
#include "Gameplay/GameObjectContext.hpp"
#include "Gameplay/GameObjectID.hpp"
#include "Gameplay/GameWorld.hpp"

// Graphics
#include "Graphics/Blend.hpp"
#include "Graphics/BlendDescription.hpp"
#include "Graphics/BlendFunction.hpp"
#include "Graphics/BlendState.hpp"
#include "Graphics/BufferUsage.hpp"
#include "Graphics/ClearOptions.hpp"
#include "Graphics/ComparisonFunction.hpp"
#include "Graphics/ConstantBufferBinding.hpp"
#include "Graphics/CullMode.hpp"
#include "Graphics/CustomVertex.hpp"
#include "Graphics/DepthStencilDescription.hpp"
#include "Graphics/DepthStencilOperation.hpp"
#include "Graphics/DepthStencilState.hpp"
#include "Graphics/EffectAnnotation.hpp"
#include "Graphics/EffectConstantDescription.hpp"
#include "Graphics/EffectParameter.hpp"
#include "Graphics/EffectVariableClass.hpp"
#include "Graphics/EffectVariableType.hpp"
#include "Graphics/EffectPass.hpp"
#include "Graphics/EffectReflection.hpp"
#include "Graphics/EffectVariable.hpp"
#include "Graphics/FillMode.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/IndexElementSize.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/PrimitiveTopology.hpp"
#include "Graphics/RasterizerDescription.hpp"
#include "Graphics/RasterizerState.hpp"
#include "Graphics/RenderTarget2D.hpp"
#include "Graphics/SamplerDescription.hpp"
#include "Graphics/SamplerState.hpp"
#include "Graphics/StencilOperation.hpp"
#include "Graphics/SurfaceFormat.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Texture2D.hpp"
#include "Graphics/TextureAddressMode.hpp"
#include "Graphics/TextureFilter.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/VertexBufferBinding.hpp"
#include "Graphics/VertexDeclaration.hpp"
#include "Graphics/VertexElement.hpp"
#include "Graphics/VertexElementFormat.hpp"
#include "Graphics/Viewport.hpp"

// Input
#include "Input/ButtonState.hpp"
#include "Input/Gamepad.hpp"
#include "Input/GamepadButtons.hpp"
#include "Input/GamepadCapabilities.hpp"
#include "Input/GamepadDPad.hpp"
#include "Input/GamepadState.hpp"
#include "Input/GamepadThumbSticks.hpp"
#include "Input/GamepadType.hpp"
#include "Input/Keyboard.hpp"
#include "Input/KeyboardState.hpp"
#include "Input/Keys.hpp"
#include "Input/KeyState.hpp"
#include "Input/Mouse.hpp"
#include "Input/MouseState.hpp"
#include "Input/PlayerIndex.hpp"
#include "Input/TouchLocation.hpp"
#include "Input/TouchLocationState.hpp"

#include "Utility/Assert.hpp"
#include "Utility/Exception.hpp"
#include "Utility/StringFormat.hpp"

#include "Config/Export.hpp"
#include "Config/Platform.hpp"

#endif // !defined(POMDOG_89D6FE57_7B46_4032_92AE_EA4D52643E23_HPP)

cmake_minimum_required(VERSION 3.10)
project(Pomdog CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "../..")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(Application                  REGULAR_EXPRESSION "(include/Pomdog|src)/Application/*")
source_group(Async                        REGULAR_EXPRESSION "(include/Pomdog|src)/Async/*")
source_group(Audio                        REGULAR_EXPRESSION "(include/Pomdog|src)/Audio/*")
source_group(Basic                        REGULAR_EXPRESSION "(include/Pomdog|src)/Basic/*")
source_group(Content                      REGULAR_EXPRESSION "(include/Pomdog|src)/Content/*")
source_group(Content\\AssetBuilders       REGULAR_EXPRESSION "(include/Pomdog|src)/Content/AssetBuilders/*")
source_group(Content\\Utility             REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Utility/*")
source_group(Graphics                     REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics/*")
source_group(Graphics\\ShaderCompilers    REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics/ShaderCompilers/*")
source_group(Input                        REGULAR_EXPRESSION "(include/Pomdog|src)/Input/*")
source_group(Logging                      REGULAR_EXPRESSION "(include/Pomdog|src)/Logging/*")
source_group(Math                         REGULAR_EXPRESSION "(include/Pomdog|src)/Math/*")
source_group(Platform.Cocoa               REGULAR_EXPRESSION "(include/Pomdog/Platform/Cocoa|src/Platform.Cocoa)/*")
source_group(Platform.Win32               REGULAR_EXPRESSION "(include/Pomdog/Platform/Win32|src/Platform.Win32)/*")
source_group(Platform.X11                 REGULAR_EXPRESSION "(include/Pomdog/Platform/X11|src/Platform.X11)/*")
source_group(Reactive                     REGULAR_EXPRESSION "(include/Pomdog|src)/Reactive/*")
source_group(Signals                      REGULAR_EXPRESSION "(include/Pomdog|src)/Signals/*")
source_group(Utility                      REGULAR_EXPRESSION "(include/Pomdog|src)/Utility/*")
source_group(Experimental\\Graphics       REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Graphics/*")
source_group(Experimental\\Image          REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Image/*")
source_group(Experimental\\MagicaVoxel    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/MagicaVoxel/*")
source_group(Experimental\\TexturePacker  REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/TexturePacker/*")
source_group(Experimental\\Tween          REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Tween/*")
source_group(InputSystem                  REGULAR_EXPRESSION src/InputSystem/*)
source_group(InputSystem.DirectInput      REGULAR_EXPRESSION src/InputSystem.DirectInput/*)
source_group(InputSystem.IOKit            REGULAR_EXPRESSION src/InputSystem.IOKit/*)
source_group(InputSystem.Linux            REGULAR_EXPRESSION src/InputSystem.Linux/*)
source_group(Platform.Apple               REGULAR_EXPRESSION src/Platform.Apple/*)
source_group(Platform.Linux               REGULAR_EXPRESSION src/Platform.Linux/*)
source_group(RenderSystem                 REGULAR_EXPRESSION src/RenderSystem/*)
source_group(RenderSystem.DXGI            REGULAR_EXPRESSION src/RenderSystem.DXGI/*)
source_group(RenderSystem.Direct3D        REGULAR_EXPRESSION src/RenderSystem.Direct3D/*)
source_group(RenderSystem.Direct3D11      REGULAR_EXPRESSION src/RenderSystem.Direct3D11/*)
source_group(RenderSystem.GL4             REGULAR_EXPRESSION src/RenderSystem.GL4/*)
source_group(RenderSystem.Metal           REGULAR_EXPRESSION src/RenderSystem.Metal/*)
source_group(RenderSystem.Vulkan          REGULAR_EXPRESSION src/RenderSystem.Vulkan/*)
source_group(SoundSystem.OpenAL           REGULAR_EXPRESSION src/SoundSystem.OpenAL/*)
source_group(SoundSystem.XAudio2          REGULAR_EXPRESSION src/SoundSystem.XAudio2/*)

set(POMDOG_SOURCES_CORE
  ${POMDOG_DIR}/include/Pomdog/Application/Duration.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/Game.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/GameClock.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/GameHost.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/GameWindow.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/MouseCursor.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/Timer.hpp
  ${POMDOG_DIR}/include/Pomdog/Application/TimePoint.hpp
  ${POMDOG_DIR}/include/Pomdog/Async/Helpers.hpp
  ${POMDOG_DIR}/include/Pomdog/Async/ImmediateScheduler.hpp
  ${POMDOG_DIR}/include/Pomdog/Async/QueuedScheduler.hpp
  ${POMDOG_DIR}/include/Pomdog/Async/Scheduler.hpp
  ${POMDOG_DIR}/include/Pomdog/Async/Task.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/AudioClip.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/AudioChannels.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/AudioEmitter.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/AudioEngine.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/AudioListener.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/SoundEffect.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/SoundState.hpp
  ${POMDOG_DIR}/include/Pomdog/Audio/detail/ForwardDeclarations.hpp
  ${POMDOG_DIR}/include/Pomdog/Basic/Export.hpp
  ${POMDOG_DIR}/include/Pomdog/Basic/Platform.hpp
  ${POMDOG_DIR}/include/Pomdog/Basic/Version.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/AssetManager.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/AssetBuilders/Builder.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/AssetBuilders/ShaderBuilder.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/Utility/BinaryFileStream.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/Utility/BinaryReader.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/Utility/MakeFourCC.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/detail/AssetDictionary.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/detail/AssetLoaderContext.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/detail/AssetLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/detail/AssetLoaders/AudioClipLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Content/detail/AssetLoaders/Texture2DLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/Blend.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/BlendDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/BlendOperation.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/BufferUsage.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ComparisonFunction.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ConstantBuffer.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/CullMode.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/DepthFormat.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/DepthStencilDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/DepthStencilOperation.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/EffectAnnotation.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/EffectConstantDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/EffectReflection.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/EffectVariableClass.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/EffectVariableType.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/EffectVariable.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/FillMode.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/GraphicsCommandList.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/GraphicsCommandQueue.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/GraphicsDevice.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/IndexBuffer.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/IndexElementSize.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/InputClassification.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/InputElement.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/InputElementFormat.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/InputLayoutDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/InputLayoutHelper.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/PipelineState.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/PipelineStateDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/PresentationParameters.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/PrimitiveTopology.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/RasterizerDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/RenderPass.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/RenderTarget2D.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/RenderTargetBlendDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/SamplerDescription.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/SamplerState.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/Shader.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ShaderLanguage.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ShaderPipelineStage.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/SurfaceFormat.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/StencilOperation.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/Texture.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/Texture2D.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/TextureAddressMode.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/TextureFilter.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/VertexBuffer.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/VertexBufferBinding.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/Viewport.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/detail/ForwardDeclarations.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/detail/EffectBinaryParameter.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp
  ${POMDOG_DIR}/include/Pomdog/Graphics/ShaderCompilers/MetalCompiler.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/ButtonState.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/Gamepad.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/GamepadButtons.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/GamepadCapabilities.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/GamepadDPad.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/GamepadState.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/GamepadThumbSticks.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/GamepadUUID.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/Keyboard.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/KeyboardState.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/KeyState.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/Keys.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/Mouse.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/MouseState.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/PlayerIndex.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/TouchLocation.hpp
  ${POMDOG_DIR}/include/Pomdog/Input/TouchLocationState.hpp
  ${POMDOG_DIR}/include/Pomdog/Logging/Log.hpp
  ${POMDOG_DIR}/include/Pomdog/Logging/LogChannel.hpp
  ${POMDOG_DIR}/include/Pomdog/Logging/LogEntry.hpp
  ${POMDOG_DIR}/include/Pomdog/Logging/LogLevel.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/BoundingBox.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/BoundingBox.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/BoundingBox2D.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/BoundingCircle.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/BoundingFrustum.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/BoundingSphere.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Color.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/ContainmentType.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Degree.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/MathHelper.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Matrix2x2.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Matrix3x2.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Matrix3x3.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Matrix4x4.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Plane.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/PlaneIntersectionType.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Point2D.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Point3D.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Quaternion.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Radian.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Ray.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Rectangle.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Vector2.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Vector3.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/Vector4.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/Coordinate2D.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/Coordinate3D.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointMatrix2x2.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointMatrix3x2.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointMatrix3x3.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointMatrix4x4.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointQuaternion.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointVector2.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointVector3.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/FloatingPointVector4.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/ForwardDeclarations.hpp
  ${POMDOG_DIR}/include/Pomdog/Math/detail/TaggedArithmetic.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Observable.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/ObservableBase.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/ObservableBuilder.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Observer.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Subscriber.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/BufferOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/DebounceOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/DelayOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/DistinctOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/DoOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/FilterOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/FirstOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/LastOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/MapOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/MergeOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/ScanOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/SkipLastOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/SkipOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/TakeOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/TimeoutOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Operators/ZipOperator.hpp
  ${POMDOG_DIR}/include/Pomdog/Reactive/Subjects/KeyDownSubject.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/Connection.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/ConnectionList.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/Event.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/EventQueue.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/Helpers.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/ScopedConnection.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/Signal.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/detail/EventBody.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/detail/ForwardDeclarations.hpp
  ${POMDOG_DIR}/include/Pomdog/Signals/detail/SignalBody.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/Any.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/Assert.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/Exception.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/FileSystem.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/PathHelper.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/StringHelper.hpp
  ${POMDOG_DIR}/include/Pomdog/Utility/detail/CRC32.hpp
  ${POMDOG_DIR}/src/Application/GameClock.cpp
  ${POMDOG_DIR}/src/Application/SubsystemScheduler.hpp
  ${POMDOG_DIR}/src/Application/SystemEvents.hpp
  ${POMDOG_DIR}/src/Application/Timer.cpp
  ${POMDOG_DIR}/src/Application/TimeSource.hpp
  ${POMDOG_DIR}/src/Async/ImmediateScheduler.cpp
  ${POMDOG_DIR}/src/Async/QueuedScheduler.cpp
  ${POMDOG_DIR}/src/Async/Task.cpp
  ${POMDOG_DIR}/src/Audio/AudioClip.cpp
  ${POMDOG_DIR}/src/Audio/AudioEngine.cpp
  ${POMDOG_DIR}/src/Audio/SoundEffect.cpp
  ${POMDOG_DIR}/src/Basic/ConditionalCompilation.hpp
  ${POMDOG_DIR}/src/Basic/Unreachable.hpp
  ${POMDOG_DIR}/src/Content/AssetDictionary.cpp
  ${POMDOG_DIR}/src/Content/AssetLoaderContext.cpp
  ${POMDOG_DIR}/src/Content/AssetManager.cpp
  ${POMDOG_DIR}/src/Content/AssetBuilders/PipelineStateBuilder.cpp
  ${POMDOG_DIR}/src/Content/AssetBuilders/ShaderBuilder.cpp
  ${POMDOG_DIR}/src/Content/AssetLoaders/AudioClipLoader.cpp
  ${POMDOG_DIR}/src/Content/AssetLoaders/Texture2DLoader.cpp
  ${POMDOG_DIR}/src/Content/Utility/DDSTextureReader.cpp
  ${POMDOG_DIR}/src/Content/Utility/DDSTextureReader.hpp
  ${POMDOG_DIR}/src/Content/Utility/MSWaveAudioLoader.cpp
  ${POMDOG_DIR}/src/Content/Utility/MSWaveAudioLoader.hpp
  ${POMDOG_DIR}/src/Content/Utility/PNGTextureReader.cpp
  ${POMDOG_DIR}/src/Content/Utility/PNGTextureReader.hpp
  ${POMDOG_DIR}/src/Graphics/ConstantBuffer.cpp
  ${POMDOG_DIR}/src/Graphics/EffectBinaryParameter.cpp
  ${POMDOG_DIR}/src/Graphics/EffectReflection.cpp
  ${POMDOG_DIR}/src/Graphics/GraphicsCommandList.cpp
  ${POMDOG_DIR}/src/Graphics/GraphicsCommandQueue.cpp
  ${POMDOG_DIR}/src/Graphics/GraphicsDevice.cpp
  ${POMDOG_DIR}/src/Graphics/IndexBuffer.cpp
  ${POMDOG_DIR}/src/Graphics/InputLayoutHelper.cpp
  ${POMDOG_DIR}/src/Graphics/PipelineState.cpp
  ${POMDOG_DIR}/src/Graphics/RenderTarget2D.cpp
  ${POMDOG_DIR}/src/Graphics/SamplerState.cpp
  ${POMDOG_DIR}/src/Graphics/Texture2D.cpp
  ${POMDOG_DIR}/src/Graphics/Viewport.cpp
  ${POMDOG_DIR}/src/Graphics/VertexBuffer.cpp
  ${POMDOG_DIR}/src/Graphics/ShaderCompilers/GLSLCompiler.cpp
  ${POMDOG_DIR}/src/Graphics/ShaderCompilers/HLSLCompiler.cpp
  ${POMDOG_DIR}/src/Graphics/ShaderCompilers/MetalCompiler.cpp
  ${POMDOG_DIR}/src/Input/GamepadUUID.cpp
  ${POMDOG_DIR}/src/Input/KeyboardState.cpp
  ${POMDOG_DIR}/src/InputSystem/GamepadFactory.cpp
  ${POMDOG_DIR}/src/InputSystem/GamepadFactory.hpp
  ${POMDOG_DIR}/src/InputSystem/GamepadHelper.cpp
  ${POMDOG_DIR}/src/InputSystem/GamepadHelper.hpp
  ${POMDOG_DIR}/src/InputSystem/GamepadMappings.cpp
  ${POMDOG_DIR}/src/InputSystem/GamepadMappings.hpp
  ${POMDOG_DIR}/src/InputSystem/NativeGamepad.hpp
  ${POMDOG_DIR}/src/Logging/Log.cpp
  ${POMDOG_DIR}/src/Logging/LogChannel.cpp
  ${POMDOG_DIR}/src/Math/BoundingBox.cpp
  ${POMDOG_DIR}/src/Math/BoundingBox2D.cpp
  ${POMDOG_DIR}/src/Math/BoundingCircle.cpp
  ${POMDOG_DIR}/src/Math/BoundingFrustum.cpp
  ${POMDOG_DIR}/src/Math/BoundingSphere.cpp
  ${POMDOG_DIR}/src/Math/Color.cpp
  ${POMDOG_DIR}/src/Math/MathHelper.cpp
  ${POMDOG_DIR}/src/Math/Plane.cpp
  ${POMDOG_DIR}/src/Math/Ray.cpp
  ${POMDOG_DIR}/src/Math/Rectangle.cpp
  ${POMDOG_DIR}/src/Math/detail/Coordinate2D.cpp
  ${POMDOG_DIR}/src/Math/detail/Coordinate3D.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointMatrix2x2.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointMatrix3x2.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointMatrix3x3.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointMatrix4x4.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointQuaternion.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointVector2.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointVector3.cpp
  ${POMDOG_DIR}/src/Math/detail/FloatingPointVector4.cpp
  ${POMDOG_DIR}/src/RenderSystem/BufferBindMode.hpp
  ${POMDOG_DIR}/src/RenderSystem/BufferHelper.cpp
  ${POMDOG_DIR}/src/RenderSystem/BufferHelper.hpp
  ${POMDOG_DIR}/src/RenderSystem/GraphicsCapabilities.hpp
  ${POMDOG_DIR}/src/RenderSystem/GraphicsCommandListImmediate.cpp
  ${POMDOG_DIR}/src/RenderSystem/GraphicsCommandListImmediate.hpp
  ${POMDOG_DIR}/src/RenderSystem/GraphicsCommandQueueImmediate.cpp
  ${POMDOG_DIR}/src/RenderSystem/GraphicsCommandQueueImmediate.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeBuffer.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeEffectReflection.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeGraphicsCommandList.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeGraphicsCommandQueue.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeGraphicsContext.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeGraphicsDevice.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativePipelineState.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeRenderTarget2D.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeSamplerState.hpp
  ${POMDOG_DIR}/src/RenderSystem/NativeTexture2D.hpp
  ${POMDOG_DIR}/src/RenderSystem/ShaderBytecode.hpp
  ${POMDOG_DIR}/src/RenderSystem/ShaderCompileOptions.hpp
  ${POMDOG_DIR}/src/RenderSystem/SurfaceFormatHelper.cpp
  ${POMDOG_DIR}/src/RenderSystem/SurfaceFormatHelper.hpp
  ${POMDOG_DIR}/src/RenderSystem/TextureHelper.cpp
  ${POMDOG_DIR}/src/RenderSystem/TextureHelper.hpp
  ${POMDOG_DIR}/src/Signals/Connection.cpp
  ${POMDOG_DIR}/src/Signals/ConnectionList.cpp
  ${POMDOG_DIR}/src/Signals/EventQueue.cpp
  ${POMDOG_DIR}/src/Signals/ScopedConnection.cpp
  ${POMDOG_DIR}/src/Utility/AlignedNew.hpp
  ${POMDOG_DIR}/src/Utility/CRC32.cpp
  ${POMDOG_DIR}/src/Utility/PathHelper.cpp
  ${POMDOG_DIR}/src/Utility/ScopeGuard.hpp
  ${POMDOG_DIR}/src/Utility/StringHelper.cpp
  ${POMDOG_DIR}/src/Utility/Tagged.hpp
)

set(POMDOG_SOURCES_EXPERIMENTAL
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/FontGlyph.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/LineBatch.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/PolygonBatch.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/PolygonShapeBuilder.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/SpriteBatch.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/SpriteFont.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/SpriteFontLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Graphics/TrueTypeFont.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Image/GifImage.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Image/GifImageLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Image/Image.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/MagicaVoxel/VoxModel.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/MagicaVoxel/VoxModelExporter.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/MagicaVoxel/VoxModelLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/TexturePacker/TextureAtlas.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/TexturePacker/TextureAtlasGenerator.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/TexturePacker/TextureAtlasLoader.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/TexturePacker/TextureRegion.hpp
  ${POMDOG_DIR}/include/Pomdog/Experimental/Tween/EasingHelper.hpp
  ${POMDOG_DIR}/src/Experimental/Graphics/LineBatch.cpp
  ${POMDOG_DIR}/src/Experimental/Graphics/PolygonBatch.cpp
  ${POMDOG_DIR}/src/Experimental/Graphics/PolygonShapeBuilder.cpp
  ${POMDOG_DIR}/src/Experimental/Graphics/SpriteBatch.cpp
  ${POMDOG_DIR}/src/Experimental/Graphics/SpriteFont.cpp
  ${POMDOG_DIR}/src/Experimental/Graphics/SpriteFontLoader.cpp
  ${POMDOG_DIR}/src/Experimental/Graphics/TrueTypeFont.cpp
  ${POMDOG_DIR}/src/Experimental/Image/GifImageLoader.cpp
  ${POMDOG_DIR}/src/Experimental/Image/Image.cpp
  ${POMDOG_DIR}/src/Experimental/MagicaVoxel/VoxChunkHeader.hpp
  ${POMDOG_DIR}/src/Experimental/MagicaVoxel/VoxModelLoader.cpp
  ${POMDOG_DIR}/src/Experimental/MagicaVoxel/VoxModelExporter.cpp
  ${POMDOG_DIR}/src/Experimental/TexturePacker/TextureAtlasGenerator.cpp
  ${POMDOG_DIR}/src/Experimental/TexturePacker/TextureAtlasLoader.cpp
  ${POMDOG_DIR}/src/Experimental/Tween/EasingHelper.cpp
)

set(POMDOG_SOURCES_GL4
  ${POMDOG_DIR}/src/RenderSystem.GL4/BlendStateGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/BlendStateGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/BufferGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/BufferGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/DepthStencilStateGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/DepthStencilStateGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/EffectReflectionGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/EffectReflectionGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/ErrorChecker.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/ErrorChecker.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/GraphicsContextGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/GraphicsContextGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/GraphicsDeviceGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/GraphicsDeviceGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/InputLayoutGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/InputLayoutGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/OpenGLContext.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/OpenGLPrerequisites.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/PipelineStateGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/PipelineStateGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/RasterizerStateGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/RasterizerStateGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/RenderTarget2DGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/RenderTarget2DGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/SamplerStateGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/SamplerStateGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/ShaderGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/ShaderGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/Texture2DGL4.cpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/Texture2DGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/TypesafeGL4.hpp
  ${POMDOG_DIR}/src/RenderSystem.GL4/TypesafeHelperGL4.hpp
)

set(POMDOG_SOURCES_OPENAL
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/AudioClipAL.cpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/AudioClipAL.hpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/AudioEngineAL.cpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/AudioEngineAL.hpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/ContextOpenAL.cpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/ContextOpenAL.hpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/ErrorCheckerAL.cpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/ErrorCheckerAL.hpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/PrerequisitesOpenAL.hpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/SoundEffectAL.cpp
  ${POMDOG_DIR}/src/SoundSystem.OpenAL/SoundEffectAL.hpp
)

set(POMDOG_SOURCES_APPLE
  ${POMDOG_DIR}/src/Platform.Apple/FileSystemApple.mm
  ${POMDOG_DIR}/src/Platform.Apple/TimeSourceApple.cpp
  ${POMDOG_DIR}/src/Platform.Apple/TimeSourceApple.hpp
)

set(POMDOG_SOURCES_COCOA
  ${POMDOG_DIR}/include/Pomdog/Platform/Cocoa/Bootstrap.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/Bootstrap.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/CocoaWindowDelegate.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/CocoaWindowDelegate.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/GameHostCocoa.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/GameHostCocoa.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/GameWindowCocoa.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/GameWindowCocoa.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/KeyboardCocoa.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/KeyboardCocoa.cpp
  ${POMDOG_DIR}/src/Platform.Cocoa/MouseCocoa.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/MouseCocoa.cpp
  ${POMDOG_DIR}/src/Platform.Cocoa/OpenGLContextCocoa.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/OpenGLContextCocoa.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/PomdogOpenGLView.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/PomdogOpenGLView.mm
)

set(POMDOG_SOURCES_IOKIT
  ${POMDOG_DIR}/src/InputSystem.IOKit/GamepadIOKit.cpp
  ${POMDOG_DIR}/src/InputSystem.IOKit/GamepadIOKit.hpp
)

set(POMDOG_SOURCES_DIRECT3D
  ${POMDOG_DIR}/src/RenderSystem.DXGI/DXGIFormatHelper.cpp
  ${POMDOG_DIR}/src/RenderSystem.DXGI/DXGIFormatHelper.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D/HLSLCompiling.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D/HLSLCompiling.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D/HLSLReflectionHelper.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D/HLSLReflectionHelper.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D/PrerequisitesDirect3D.hpp
)

set(POMDOG_SOURCES_DIRECT3D11
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/BufferDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/BufferDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/EffectReflectionDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/EffectReflectionDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/GraphicsContextDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/GraphicsDeviceDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/GraphicsDeviceDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/PipelineStateDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/PipelineStateDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/PrerequisitesDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/RenderTarget2DDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/RenderTarget2DDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/SamplerStateDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/SamplerStateDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/ShaderDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/ShaderDirect3D11.hpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/Texture2DDirect3D11.cpp
  ${POMDOG_DIR}/src/RenderSystem.Direct3D11/Texture2DDirect3D11.hpp
)

set(POMDOG_SOURCES_METAL
  ${POMDOG_DIR}/src/RenderSystem.Metal/BufferMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/BufferMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/ConstantsMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/EffectReflectionMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/EffectReflectionMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/GraphicsContextMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/GraphicsContextMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/GraphicsDeviceMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/GraphicsDeviceMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/MetalFormatHelper.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/MetalFormatHelper.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/PipelineStateMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/PipelineStateMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/RenderTarget2DMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/RenderTarget2DMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/SamplerStateMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/SamplerStateMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/ShaderMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/ShaderMetal.mm
  ${POMDOG_DIR}/src/RenderSystem.Metal/Texture2DMetal.hpp
  ${POMDOG_DIR}/src/RenderSystem.Metal/Texture2DMetal.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/GameHostMetal.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/GameHostMetal.mm
  ${POMDOG_DIR}/src/Platform.Cocoa/PomdogMetalViewController.hpp
  ${POMDOG_DIR}/src/Platform.Cocoa/PomdogMetalViewController.mm
)

set(POMDOG_SOURCES_VULKAN
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/BufferVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/BufferVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/EffectReflectionVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/EffectReflectionVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/GraphicsCommandListVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/GraphicsCommandListVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/GraphicsCommandQueueVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/GraphicsCommandQueueVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/GraphicsDeviceVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/GraphicsDeviceVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/PipelineStateVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/PipelineStateVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/RenderTexture2DVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/RenderTexture2DVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/SamplerStateVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/SamplerStateVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/ShaderVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/ShaderVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/Texture2DVulkan.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/Texture2DVulkan.hpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/VulkanFormatHelper.cpp
  ${POMDOG_DIR}/src/RenderSystem.Vulkan/VulkanFormatHelper.hpp
)

set(POMDOG_SOURCES_XAUDIO2
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/AudioClipXAudio2.cpp
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/AudioClipXAudio2.hpp
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/AudioEngineXAudio2.cpp
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/AudioEngineXAudio2.hpp
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/PrerequisitesXAudio2.hpp
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/SoundEffectXAudio2.cpp
  ${POMDOG_DIR}/src/SoundSystem.XAudio2/SoundEffectXAudio2.hpp
)

set(POMDOG_SOURCES_DIRECTINPUT
  ${POMDOG_DIR}/src/InputSystem.DirectInput/GamepadDirectInput.cpp
  ${POMDOG_DIR}/src/InputSystem.DirectInput/GamepadDirectInput.hpp
  ${POMDOG_DIR}/src/InputSystem.DirectInput/PrerequisitesDirectInput.hpp
)

set(POMDOG_SOURCES_WIN32
  ${POMDOG_DIR}/include/Pomdog/Platform/Win32/Bootstrap.hpp
  ${POMDOG_DIR}/include/Pomdog/Platform/Win32/BootstrapSettingsWin32.hpp
  ${POMDOG_DIR}/include/Pomdog/Platform/Win32/PrerequisitesWin32.hpp
  ${POMDOG_DIR}/src/Platform.Win32/Bootstrap.cpp
  ${POMDOG_DIR}/src/Platform.Win32/GameHostWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/GameHostWin32.hpp
  ${POMDOG_DIR}/src/Platform.Win32/GameWindowWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/GameWindowWin32.hpp
  ${POMDOG_DIR}/src/Platform.Win32/FileSystemWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/KeyboardWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/KeyboardWin32.hpp
  ${POMDOG_DIR}/src/Platform.Win32/MouseWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/MouseWin32.hpp
  ${POMDOG_DIR}/src/Platform.Win32/TimeSourceWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/TimeSourceWin32.hpp
)

set(POMDOG_SOURCES_WIN32_OPENGL
  ${POMDOG_DIR}/src/Platform.Win32/OpenGLContextWin32.cpp
  ${POMDOG_DIR}/src/Platform.Win32/OpenGLContextWin32.hpp
)

set(POMDOG_SOURCES_X11
  ${POMDOG_DIR}/include/Pomdog/Platform/X11/Bootstrap.hpp
  ${POMDOG_DIR}/src/Platform.X11/Bootstrap.cpp
  ${POMDOG_DIR}/src/Platform.X11/GameHostX11.cpp
  ${POMDOG_DIR}/src/Platform.X11/GameHostX11.hpp
  ${POMDOG_DIR}/src/Platform.X11/GameWindowX11.cpp
  ${POMDOG_DIR}/src/Platform.X11/GameWindowX11.hpp
  ${POMDOG_DIR}/src/Platform.X11/KeyboardX11.cpp
  ${POMDOG_DIR}/src/Platform.X11/KeyboardX11.hpp
  ${POMDOG_DIR}/src/Platform.X11/MouseX11.cpp
  ${POMDOG_DIR}/src/Platform.X11/MouseX11.hpp
  ${POMDOG_DIR}/src/Platform.X11/OpenGLContextX11.cpp
  ${POMDOG_DIR}/src/Platform.X11/OpenGLContextX11.hpp
  ${POMDOG_DIR}/src/Platform.X11/X11AtomCache.hpp
  ${POMDOG_DIR}/src/Platform.X11/X11Context.cpp
  ${POMDOG_DIR}/src/Platform.X11/X11Context.hpp
)

set(POMDOG_SOURCES_LINUX
  ${POMDOG_DIR}/src/InputSystem.Linux/GamepadLinux.cpp
  ${POMDOG_DIR}/src/InputSystem.Linux/GamepadLinux.hpp
  ${POMDOG_DIR}/src/Platform.Linux/FileSystemLinux.cpp
  ${POMDOG_DIR}/src/Platform.Linux/TimeSourceLinux.cpp
  ${POMDOG_DIR}/src/Platform.Linux/TimeSourceLinux.hpp
)

set(SOURCE_FILES ${POMDOG_SOURCES_CORE} ${POMDOG_SOURCES_EXPERIMENTAL})

if(APPLE)
  set(POMDOG_TARGET_PLATFORM "Mac"   CACHE STRING "Choose platform")
  set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
  set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
  set(POMDOG_USE_METAL       true    CACHE BOOL   "Use Metal")
  set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
  set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
  set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
  set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
  set(POMDOG_USE_IOKIT       true    CACHE BOOL   "Use IOKit")
  set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(POMDOG_TARGET_PLATFORM "Linux" CACHE STRING "Choose platform")
  set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
  set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
  set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
  set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
  set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
  set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
  set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
  set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
  set(POMDOG_USE_X11         true    CACHE BOOL   "Use X11")
elseif(WIN32)
  set(POMDOG_TARGET_PLATFORM "Win32" CACHE STRING "Choose platform")
  set(POMDOG_USE_DIRECT3D11  true    CACHE BOOL   "Use Direct3D11")
  set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
  set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
  set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
  set(POMDOG_USE_OPENAL      false   CACHE BOOL   "Use OpenAL")
  set(POMDOG_USE_XAUDIO2     true    CACHE BOOL   "Use XAudio2")
  set(POMDOG_USE_DIRECTINPUT true    CACHE BOOL   "Use DirectInput")
  set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
  set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
endif()

if(POMDOG_TARGET_PLATFORM MATCHES "Linux")
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_LINUX})
elseif(POMDOG_TARGET_PLATFORM MATCHES "Mac")
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_APPLE})
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_COCOA})
elseif(POMDOG_TARGET_PLATFORM MATCHES "Win32")
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_WIN32})
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_DIRECTINPUT})
endif()

if(POMDOG_USE_DIRECT3D11)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_DIRECT3D})
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_DIRECT3D11})
endif()

if(POMDOG_USE_GL4)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_GL4})
endif()

if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_WIN32_OPENGL})
endif()

if(POMDOG_USE_METAL)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_METAL})
endif()

if(POMDOG_USE_VULKAN)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_VULKAN})
endif()

if(POMDOG_USE_XAUDIO2)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_XAUDIO2})
endif()

if(POMDOG_USE_OPENAL)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_OPENAL})
endif()

if(POMDOG_USE_X11)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_X11})
endif()

if(POMDOG_USE_IOKIT)
  list(APPEND SOURCE_FILES ${POMDOG_SOURCES_IOKIT})
endif()

# NOTE: Linking libraries for Mac and Xcode
if(POMDOG_USE_METAL)
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/Metal.framework")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework Metal")
endif()

if(POMDOG_USE_OPENAL AND (POMDOG_TARGET_PLATFORM MATCHES "Mac"))
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/AudioToolBox.framework")
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/OpenAL.framework")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework AudioToolBox")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenAL")
endif()

if(POMDOG_USE_IOKIT AND (POMDOG_TARGET_PLATFORM MATCHES "Mac"))
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/IOKit.framework")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework IOKit")
endif()

if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Mac"))
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/OpenGL.framework")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenGL")
endif()

if(POMDOG_TARGET_PLATFORM MATCHES "Mac")
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/Cocoa.framework")
  # list(APPEND LINK_FRAMEWORKS "$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework Cocoa")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework QuartzCore")
endif()

list(SORT SOURCE_FILES)

# NOTE: static library
add_library(pomdog_static STATIC ${SOURCE_FILES})
#install(TARGETS pomdog_static DESTINATION lib)

if(UNIX)
  # On Unix platforms, the library is called `libpomdog.a`.
  set_target_properties(pomdog_static PROPERTIES OUTPUT_NAME pomdog)
endif()

target_include_directories(pomdog_static PRIVATE
  ${POMDOG_DIR}/include
  ${POMDOG_DIR}/dependencies
  ${POMDOG_DIR}/dependencies/libpng
  ${POMDOG_DIR}/dependencies/stb
  ${POMDOG_DIR}/dependencies/vendor/libpng
  ${POMDOG_DIR}/dependencies/vendor/giflib/lib
  ${POMDOG_DIR}/dependencies/vendor/SDL_GameControllerDB
)

add_subdirectory(${POMDOG_DIR}/build/dependencies/libpng "${CMAKE_CURRENT_BINARY_DIR}/libpng_build")
target_link_libraries(pomdog_static INTERFACE png)

add_subdirectory(${POMDOG_DIR}/build/dependencies/giflib "${CMAKE_CURRENT_BINARY_DIR}/giflib_build")
target_link_libraries(pomdog_static INTERFACE giflib)

if(POMDOG_USE_OPENAL AND (POMDOG_TARGET_PLATFORM MATCHES "Linux"))
  target_link_libraries(pomdog_static INTERFACE openal)
endif()

if(POMDOG_USE_X11 AND (POMDOG_TARGET_PLATFORM MATCHES "Linux"))
  target_include_directories(pomdog_static PRIVATE
    /usr/X11R6/include
    ${POMDOG_DIR}/dependencies/vendor/glew/include
  )
  link_directories(
    /usr/X11R6/lib
  )
  target_compile_definitions(pomdog_static PRIVATE GLEW_STATIC)

  add_subdirectory(${POMDOG_DIR}/build/dependencies/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
  target_link_libraries(pomdog_static INTERFACE X11 GL glew_static)
endif()

if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
  target_include_directories(pomdog_static PRIVATE
    ${POMDOG_DIR}/dependencies/vendor/glew/include
  )
  target_compile_definitions(pomdog_static PRIVATE GLEW_STATIC)

  add_subdirectory(${POMDOG_DIR}/build/dependencies/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
  target_link_libraries(pomdog_static INTERFACE opengl32.lib glew_static)
endif()

if(POMDOG_USE_DIRECT3D11 AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
  target_link_libraries(pomdog_static INTERFACE
    dxgi.lib
    d3d11.lib
    d3dcompiler.lib
    dxguid.lib # for _IID_ID3D11ShaderReflection
  )
endif()

if(POMDOG_USE_XAUDIO2 AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
  target_link_libraries(pomdog_static INTERFACE xaudio2.lib)
endif()

if(POMDOG_USE_DIRECTINPUT AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
  target_link_libraries(pomdog_static INTERFACE
    dinput8.lib
    dxguid.lib
    oleaut32.lib # for SysAllocString
  )
endif()

target_compile_definitions(pomdog_static PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG;DEBUG=1>
  $<$<CONFIG:RELEASE>:NDEBUG>
)

if(POMDOG_TARGET_PLATFORM MATCHES "Win32")
  target_compile_definitions(pomdog_static PRIVATE
    _WIN32_WINNT=0x0602 # Windows 8 or later
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  )

  target_link_libraries(pomdog_static INTERFACE
    kernel32.lib
    user32.lib
    gdi32.lib
    ole32.lib
    winmm.lib
    shell32.lib
    # ws2_32.lib
    # winspool.lib
    # comdlg32.lib
    # advapi32.lib
    # oleaut32.lib
    # uuid.lib
    # odbc32.lib
    # odbccp32.lib
  )
endif()

target_compile_options(pomdog_static PRIVATE
  # Clang
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wcomma
    -Wextra-semi
    -Wimplicit-fallthrough
    -Wmost
    -Wmove
    -Wnewline-eof

    # FIXME: Use -Wextra instead of the following options.
    -Wignored-qualifiers
    -Winitializer-overrides
    -Wmissing-field-initializers
    -Wmissing-method-return-type
    -Wsemicolon-before-method-body
    -Wsign-compare
    # -Wunused-parameter
  >
  # GCC
  $<$<CXX_COMPILER_ID:GNU>:
    -pedantic

    # FIXME: Use -Wextra instead of the following options.
    -Wclobbered
    -Wcast-function-type
    -Wempty-body
    -Wignored-qualifiers
    -Wimplicit-fallthrough=3
    -Wmissing-field-initializers
    # -Wtype-limits
    -Wuninitialized
    -Wshift-negative-value
  >
  # Clang and GCC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wall
    -Werror
    -Wnon-virtual-dtor
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    /WX
    $<$<CONFIG:DEBUG>:/Od;/MTd>
    $<$<CONFIG:RELEASE>:/O2;/Ob2;/MT>
  >
)

if (POMDOG_TARGET_PLATFORM MATCHES "Mac")
  # TODO: Replace with the following, CMake >= 3.12
  # `target_link_options(pomdog_static PUBLIC "${LINK_FRAMEWORKS}")`
  # https://cmake.org/cmake/help/git-stage/command/target_link_options.html
  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LINK_FRAMEWORKS}")
endif()

# NOTE: For dynamic library
# NOTE: see https://cmake.org/cmake/help/v3.9/prop_tgt/FRAMEWORK.html
# set_target_properties(pomdog_static PROPERTIES
#   XCODE_ATTRIBUTE_PRODUCT_NAME "Pomdog"
#   XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "net.enginetrouble.pomdog"
#   XCODE_ATTRIBUTE_INFOPLIST_FILE "src/Platform.Apple/Info.plist"
#   XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_LIBRARY_DIR)/Frameworks"
#   XCODE_ATTRIBUTE_DYLIB_INSTALL_NAME_BASE "@rpath"
#   XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited);@executable_path/../Frameworks;@loader_path/Frameworks"
# )

set_target_properties(pomdog_static PROPERTIES
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
  XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
  XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

  XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
  XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
  XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
  XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.11"

  XCODE_ATTRIBUTE_SKIP_INSTALL "YES"

  # Warnings (Clang)
  XCODE_ATTRIBUTE_CLANG_WARN_ASSIGN_ENUM "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_COMMA "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_DOCUMENTATION_COMMENTS "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_INFINITE_RECURSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_NON_LITERAL_NULL_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_RANGE_LOOP_ANALYSIS "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_STRICT_PROTOTYPES "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_SUSPICIOUS_MOVE "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_UNREACHABLE_CODE "YES"

  # Warnings (GCC and Clang)
  XCODE_ATTRIBUTE_GCC_TREAT_INCOMPATIBLE_POINTER_TYPE_WARNINGS_AS_ERRORS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_NEWLINE "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ABOUT_RETURN_TYPE "YES_ERROR"
  XCODE_ATTRIBUTE_GCC_WARN_CHECK_SWITCH_STATEMENTS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_MISSING_PARENTHESES "YES"
  XCODE_ATTRIBUTE_GCC_WARN_NON_VIRTUAL_DESTRUCTOR "YES"
  XCODE_ATTRIBUTE_GCC_WARN_SHADOW "YES"
  XCODE_ATTRIBUTE_GCC_WARN_SIGN_COMPARE "YES"
  XCODE_ATTRIBUTE_GCC_WARN_TYPECHECK_CALLS_TO_PRINTF "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNINITIALIZED_AUTOS "YES_AGGRESSIVE"
  XCODE_ATTRIBUTE_GCC_WARN_UNKNOWN_PRAGMAS "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_FUNCTION "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_LABEL "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VALUE "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VARIABLE "YES"

  # Warnings - Objective-C
  XCODE_ATTRIBUTE_CLANG_WARN_DIRECT_OBJC_ISA_USAGE "YES_ERROR"
  XCODE_ATTRIBUTE_CLANG_WARN__DUPLICATE_METHOD_MATCH "YES"
  XCODE_ATTRIBUTE_GCC_WARN_ALLOW_INCOMPLETE_PROTOCOL "YES"
  XCODE_ATTRIBUTE_GCC_WARN_UNDECLARED_SELECTOR "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_OBJC_LITERAL_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_OBJC_ROOT_CLASS "YES_ERROR"

  # Warning Policies
  XCODE_ATTRIBUTE_GCC_TREAT_WARNINGS_AS_ERRORS "YES"
  XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wall"

  # Symbols
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
  XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
  XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden
)

cmake_minimum_required(VERSION 3.31)
project(pomdog CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

cmake_path(SET POMDOG_DIR     NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
cmake_path(SET POMDOG_SRC_DIR NORMALIZE "${POMDOG_DIR}/pomdog")

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(POMDOG_ENABLE_EMSCRIPTEN_PTHREAD 0 CACHE BOOL "Enable pthread in emscripten")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       true    CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       true    CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         true    CACHE BOOL   "Use X11")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(POMDOG_USE_DIRECT3D11  true    CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      false   CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     true    CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT true    CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
endif()
set(POMDOG_USE_EXPERIMENTAL true CACHE BOOL "Use Experimental")

add_library(pomdog_base STATIC)
add_library(pomdog::base ALIAS pomdog_base)
add_library(pomdog_math STATIC)
add_library(pomdog::math ALIAS pomdog_math)
add_library(pomdog_random STATIC)
add_library(pomdog::random ALIAS pomdog_random)
add_library(pomdog_filesystem STATIC)
add_library(pomdog::filesystem ALIAS pomdog_filesystem)
add_library(pomdog_vfs STATIC)
add_library(pomdog::vfs ALIAS pomdog_vfs)
add_library(pomdog_input STATIC)
add_library(pomdog::input ALIAS pomdog_input)
add_library(pomdog_gpu STATIC)
add_library(pomdog::gpu ALIAS pomdog_gpu)
add_library(pomdog_audio STATIC)
add_library(pomdog::audio ALIAS pomdog_audio)
add_library(pomdog_network STATIC)
add_library(pomdog::network ALIAS pomdog_network)
add_library(pomdog_application STATIC)
add_library(pomdog::application ALIAS pomdog_application)
add_library(pomdog_application_gpu STATIC)
add_library(pomdog::application_gpu ALIAS pomdog_application_gpu)
add_library(pomdog_content_utility STATIC)
add_library(pomdog::content_utility ALIAS pomdog_content_utility)
add_library(pomdog_content_gpu STATIC)
add_library(pomdog::content_gpu ALIAS pomdog_content_gpu)
add_library(pomdog_content_audio STATIC)
add_library(pomdog::content_audio ALIAS pomdog_content_audio)
add_library(pomdog_content_input STATIC)
add_library(pomdog::content_input ALIAS pomdog_content_input)
add_library(pomdog_graphics STATIC)
add_library(pomdog::graphics ALIAS pomdog_graphics)
add_library(pomdog_ecs STATIC)
add_library(pomdog::ecs ALIAS pomdog_ecs)
add_library(pomdog_console STATIC)
add_library(pomdog::console ALIAS pomdog_console)
add_library(pomdog_experimental STATIC)
add_library(pomdog::experimental ALIAS pomdog_experimental)

# NOTE: Compatibility target for applications that use the complete engine.
add_library(pomdog_static INTERFACE)

source_group(application             REGULAR_EXPRESSION "pomdog/application/*")
source_group(application/backends    REGULAR_EXPRESSION "pomdog/application/backends/*")
source_group(application/cocoa       REGULAR_EXPRESSION "pomdog/application/cocoa/*")
source_group(application/emscripten  REGULAR_EXPRESSION "pomdog/application/emscripten/*")
source_group(application/linux       REGULAR_EXPRESSION "pomdog/application/linux/*")
source_group(application/win32       REGULAR_EXPRESSION "pomdog/application/win32/*")
source_group(application/x11         REGULAR_EXPRESSION "pomdog/application/x11/*")

target_sources(pomdog_application PRIVATE
    ${POMDOG_SRC_DIR}/application/game.h
    ${POMDOG_SRC_DIR}/application/game.cpp
    ${POMDOG_SRC_DIR}/application/game_host.h
    ${POMDOG_SRC_DIR}/application/game_host.cpp
    ${POMDOG_SRC_DIR}/application/game_setup.h
    ${POMDOG_SRC_DIR}/application/game_setup.cpp
    ${POMDOG_SRC_DIR}/application/backends/frame_rate_limiter.h
    ${POMDOG_SRC_DIR}/application/backends/frame_rate_limiter.cpp
)

target_sources(pomdog_application_gpu PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/application/display_metrics.h
    ${POMDOG_SRC_DIR}/application/game_host_options.h
    ${POMDOG_SRC_DIR}/application/game_window.h
    ${POMDOG_SRC_DIR}/application/game_window.cpp
    ${POMDOG_SRC_DIR}/application/graphics_backend_helper.h
    ${POMDOG_SRC_DIR}/application/graphics_backend_helper.cpp
    ${POMDOG_SRC_DIR}/application/high_dpi_settings.h
    ${POMDOG_SRC_DIR}/application/high_dpi_settings.cpp
    ${POMDOG_SRC_DIR}/application/mouse_cursor.h
    ${POMDOG_SRC_DIR}/application/backends/subsystem_scheduler.h
    ${POMDOG_SRC_DIR}/application/backends/system_event_queue.h
    ${POMDOG_SRC_DIR}/application/backends/system_event_queue.cpp
    ${POMDOG_SRC_DIR}/application/backends/system_events.h

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: application/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/keyboard_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/keyboard_cocoa.cpp
        ${POMDOG_SRC_DIR}/application/cocoa/mouse_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/mouse_cocoa.cpp
        ${POMDOG_SRC_DIR}/application/cocoa/bootstrap_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/bootstrap_cocoa.mm
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_cocoa.mm
        ${POMDOG_SRC_DIR}/application/cocoa/game_window_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_window_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_opengl_view.h
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_opengl_view.mm
        ${POMDOG_SRC_DIR}/application/cocoa/opengl_context_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/opengl_context_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_METAL}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_metal.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_metal.mm
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_metal_view_controller.h
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_metal_view_controller.mm
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: application/emscripten
        ${POMDOG_SRC_DIR}/application/emscripten/keyboard_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/keyboard_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/mouse_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/mouse_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/touchscreen_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/touchscreen_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/bootstrap_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/bootstrap_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/game_host_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/game_host_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/game_window_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/game_window_emscripten.h
    >

    $<$<AND:$<PLATFORM_ID:Emscripten>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/emscripten
        ${POMDOG_SRC_DIR}/application/emscripten/opengl_context_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/opengl_context_emscripten.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: application/linux
        ${POMDOG_SRC_DIR}/application/linux/game_host_linux.cpp
        ${POMDOG_SRC_DIR}/application/linux/game_host_linux.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: application/x11
        ${POMDOG_SRC_DIR}/application/x11/keyboard_x11.h
        ${POMDOG_SRC_DIR}/application/x11/keyboard_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/mouse_x11.h
        ${POMDOG_SRC_DIR}/application/x11/mouse_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/bootstrap_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/bootstrap_x11.h
        ${POMDOG_SRC_DIR}/application/x11/game_window_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/game_window_x11.h
        ${POMDOG_SRC_DIR}/application/x11/x11_atom_cache.h
        ${POMDOG_SRC_DIR}/application/x11/x11_context.cpp
        ${POMDOG_SRC_DIR}/application/x11/x11_context.h
    >

    $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/x11
        ${POMDOG_SRC_DIR}/application/x11/opengl_context_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/opengl_context_x11.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: application/win32
        ${POMDOG_SRC_DIR}/application/win32/keyboard_win32.h
        ${POMDOG_SRC_DIR}/application/win32/keyboard_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/mouse_win32.h
        ${POMDOG_SRC_DIR}/application/win32/mouse_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/bootstrap_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/bootstrap_win32.h
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.cpp
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.h
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.h
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.h
        ${POMDOG_SRC_DIR}/application/win32/precise_sleeper_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/precise_sleeper_win32.h
    >

    $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/application/win32/opengl_context_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/opengl_context_win32.h
    >
)

source_group(audio          REGULAR_EXPRESSION "pomdog/audio/*")
source_group(audio/details  REGULAR_EXPRESSION "pomdog/audio/details/*")
source_group(audio/openal   REGULAR_EXPRESSION "pomdog/audio/openal/*")
source_group(audio/xaudio2  REGULAR_EXPRESSION "pomdog/audio/xaudio2/*")

target_sources(pomdog_audio PRIVATE
    # NOTE: audio
    ${POMDOG_SRC_DIR}/audio/audio_channels.h
    ${POMDOG_SRC_DIR}/audio/audio_clip.h
    ${POMDOG_SRC_DIR}/audio/audio_clip.cpp
    ${POMDOG_SRC_DIR}/audio/audio_engine.h
    ${POMDOG_SRC_DIR}/audio/audio_engine.cpp
    ${POMDOG_SRC_DIR}/audio/audio_source.h
    ${POMDOG_SRC_DIR}/audio/audio_source.cpp
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_file.h
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_file.cpp
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_helpers.h
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_helpers.cpp

    $<$<PLATFORM_ID:Linux,Emscripten,Darwin>:
        # NOTE: audio/openal
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_streaming_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_streaming_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_streaming_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_streaming_al.h
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.h
        ${POMDOG_SRC_DIR}/audio/openal/openal_headers.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: audio/xaudio2
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_streaming_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_streaming_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_streaming_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_streaming_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/xaudio2_headers.h
    >
)

source_group(basic       REGULAR_EXPRESSION "pomdog/basic/*")
source_group(basic/win32 REGULAR_EXPRESSION "pomdog/basic/win32/*")

target_sources(pomdog_base PRIVATE
    # NOTE: basic
    ${POMDOG_SRC_DIR}/basic/compilation_target_architecture.cpp
    ${POMDOG_SRC_DIR}/basic/compilation_target_architecture.h
    ${POMDOG_SRC_DIR}/basic/conditional_compilation.h
    ${POMDOG_SRC_DIR}/basic/export.h
    ${POMDOG_SRC_DIR}/basic/flatbuffers_macros.h
    ${POMDOG_SRC_DIR}/basic/platform.h
    ${POMDOG_SRC_DIR}/basic/types.cpp
    ${POMDOG_SRC_DIR}/basic/types.h
    ${POMDOG_SRC_DIR}/basic/unreachable.h
    ${POMDOG_SRC_DIR}/basic/version.h
    $<$<PLATFORM_ID:Windows>:
        ${POMDOG_SRC_DIR}/basic/win32/windows_headers.h
    >
)

source_group(chrono             REGULAR_EXPRESSION "pomdog/chrono/*")
source_group(chrono/apple       REGULAR_EXPRESSION "pomdog/chrono/apple/*")
source_group(chrono/detail      REGULAR_EXPRESSION "pomdog/chrono/detail/*")
source_group(chrono/emscripten  REGULAR_EXPRESSION "pomdog/chrono/emscripten/*")
source_group(chrono/linux       REGULAR_EXPRESSION "pomdog/chrono/linux/*")
source_group(chrono/win32       REGULAR_EXPRESSION "pomdog/chrono/win32/*")

target_sources(pomdog_base PRIVATE
    # NOTE: chrono
    ${POMDOG_SRC_DIR}/chrono/duration.h
    ${POMDOG_SRC_DIR}/chrono/game_clock.cpp
    ${POMDOG_SRC_DIR}/chrono/game_clock.h
    ${POMDOG_SRC_DIR}/chrono/time_point.h
    ${POMDOG_SRC_DIR}/chrono/time_source.cpp
    ${POMDOG_SRC_DIR}/chrono/time_source.h
    ${POMDOG_SRC_DIR}/chrono/timer.h
    ${POMDOG_SRC_DIR}/chrono/timer.cpp
    ${POMDOG_SRC_DIR}/chrono/detail/game_clock_impl.cpp
    ${POMDOG_SRC_DIR}/chrono/detail/game_clock_impl.h
    ${POMDOG_SRC_DIR}/chrono/detail/make_time_source.cpp
    ${POMDOG_SRC_DIR}/chrono/detail/make_time_source.h

    $<$<PLATFORM_ID:Darwin,iOS>:
        # NOTE: chrono/apple
        ${POMDOG_SRC_DIR}/chrono/apple/time_source_apple.cpp
        ${POMDOG_SRC_DIR}/chrono/apple/time_source_apple.h
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: chrono/emscripten
        ${POMDOG_SRC_DIR}/chrono/emscripten/time_source_emscripten.cpp
        ${POMDOG_SRC_DIR}/chrono/emscripten/time_source_emscripten.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: chrono/linux
        ${POMDOG_SRC_DIR}/chrono/linux/time_source_linux.cpp
        ${POMDOG_SRC_DIR}/chrono/linux/time_source_linux.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: chrono/win32
        ${POMDOG_SRC_DIR}/chrono/win32/time_source_win32.cpp
        ${POMDOG_SRC_DIR}/chrono/win32/time_source_win32.h
    >
)

source_group(console REGULAR_EXPRESSION "pomdog/console/*")

target_sources(pomdog_console PRIVATE
    # NOTE: console
    ${POMDOG_SRC_DIR}/console/console.cpp
    ${POMDOG_SRC_DIR}/console/console.h
)

source_group(ecs REGULAR_EXPRESSION "pomdog/ecs/*")

target_sources(pomdog_ecs PRIVATE
    # NOTE: ecs
    ${POMDOG_SRC_DIR}/ecs/entity_bin.cpp
    ${POMDOG_SRC_DIR}/ecs/entity_bin.h
    ${POMDOG_SRC_DIR}/ecs/entity_lru_queue.cpp
    ${POMDOG_SRC_DIR}/ecs/entity_lru_queue.h
    ${POMDOG_SRC_DIR}/ecs/entity_pool.cpp
    ${POMDOG_SRC_DIR}/ecs/entity_pool.h
    ${POMDOG_SRC_DIR}/ecs/entity.cpp
    ${POMDOG_SRC_DIR}/ecs/entity.h
    ${POMDOG_SRC_DIR}/ecs/tagged_entity.cpp
    ${POMDOG_SRC_DIR}/ecs/tagged_entity.h
)

source_group(experimental/async                     REGULAR_EXPRESSION "pomdog/experimental/async/*")
source_group(experimental/gltf                      REGULAR_EXPRESSION "pomdog/experimental/gltf/*")
source_group(experimental/gui                       REGULAR_EXPRESSION "pomdog/experimental/gui/*")
source_group(experimental/image_effects             REGULAR_EXPRESSION "pomdog/experimental/image_effects/*")
source_group(experimental/image                     REGULAR_EXPRESSION "pomdog/experimental/image/*")
source_group(experimental/magicavoxel               REGULAR_EXPRESSION "pomdog/experimental/magicavoxel/*")
source_group(experimental/particles                 REGULAR_EXPRESSION "pomdog/experimental/particles/*")
source_group(experimental/particles/emitter_shapes  REGULAR_EXPRESSION "pomdog/experimental/particles/emitter_shapes/*")
source_group(experimental/particles/parameters      REGULAR_EXPRESSION "pomdog/experimental/particles/parameters/*")
source_group(experimental/skeletal2d                REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/*")
source_group(experimental/skeletal2d/blendtrees     REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/blendtrees/*")
source_group(experimental/skeletal2d/tracks         REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/tracks/*")
source_group(experimental/spine                     REGULAR_EXPRESSION "pomdog/experimental/spine/*")
source_group(experimental/texture_atlas             REGULAR_EXPRESSION "pomdog/experimental/texture_atlas/*")

target_sources(pomdog_experimental PRIVATE
    # NOTE: experimental/async
    ${POMDOG_SRC_DIR}/experimental/async/helpers.h
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/task.h
    ${POMDOG_SRC_DIR}/experimental/async/task.cpp
)

target_sources(pomdog_experimental PRIVATE
    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/gltf
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf.h
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf.cpp
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.h
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/gui
        ${POMDOG_SRC_DIR}/experimental/gui/check_box.h
        ${POMDOG_SRC_DIR}/experimental/gui/check_box.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu.h
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu_item.h
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu_item.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/debug_navigator.h
        ${POMDOG_SRC_DIR}/experimental/gui/debug_navigator.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/disclosure_triangle_button.h
        ${POMDOG_SRC_DIR}/experimental/gui/disclosure_triangle_button.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/drawing_context.h
        ${POMDOG_SRC_DIR}/experimental/gui/drawing_context.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/float_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/float_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/font_size.h
        ${POMDOG_SRC_DIR}/experimental/gui/font_weight.h
        ${POMDOG_SRC_DIR}/experimental/gui/hierarchy_sort_order.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_alignment.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_layout.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_layout.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_line.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_line.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_splitter.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_splitter.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/int_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/int_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view.h
        ${POMDOG_SRC_DIR}/experimental/gui/list_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view_item.h
        ${POMDOG_SRC_DIR}/experimental/gui/list_view_item.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/number_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/number_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/pointer_event_type.h
        ${POMDOG_SRC_DIR}/experimental/gui/pointer_point.h
        ${POMDOG_SRC_DIR}/experimental/gui/popup_menu.h
        ${POMDOG_SRC_DIR}/experimental/gui/popup_menu.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/push_button.h
        ${POMDOG_SRC_DIR}/experimental/gui/push_button.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/scene_panel.h
        ${POMDOG_SRC_DIR}/experimental/gui/scene_panel.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/scroll_view.h
        ${POMDOG_SRC_DIR}/experimental/gui/scroll_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/slider.h
        ${POMDOG_SRC_DIR}/experimental/gui/slider.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/stack_panel.h
        ${POMDOG_SRC_DIR}/experimental/gui/stack_panel.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_alignment.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_block.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_block.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_edit.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_edit.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/thickness.h
        ${POMDOG_SRC_DIR}/experimental/gui/toggle_switch.h
        ${POMDOG_SRC_DIR}/experimental/gui/toggle_switch.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/tree_view.h
        ${POMDOG_SRC_DIR}/experimental/gui/tree_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_event_dispatcher.h
        ${POMDOG_SRC_DIR}/experimental/gui/ui_event_dispatcher.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_helper.h
        ${POMDOG_SRC_DIR}/experimental/gui/ui_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vector3_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/vector3_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_alignment.h
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_layout.h
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_layout.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_scroll_bar.h
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_scroll_bar.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget.h
        ${POMDOG_SRC_DIR}/experimental/gui/widget.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget_hierarchy.h
        ${POMDOG_SRC_DIR}/experimental/gui/widget_hierarchy.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/image
        ${POMDOG_SRC_DIR}/experimental/image/image.h
        ${POMDOG_SRC_DIR}/experimental/image/image.cpp
        ${POMDOG_SRC_DIR}/experimental/image/svg_loader.h
        ${POMDOG_SRC_DIR}/experimental/image/svg_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/image_effects
        ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/magicavoxel
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model.h
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.h
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.cpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.h
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_chunk_header.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/particles
        ${POMDOG_SRC_DIR}/experimental/particles/beam.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_branching.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_emitter.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_system.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_system.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip_loader.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_system.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_system.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_box.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_box.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_cone.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_cone.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sector.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_curve_key.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_curve_lerp.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_constant.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_curve.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_random.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_random_curves.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/skeletal2d
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_clip.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_clip.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_state.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_state.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_system.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_system.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_time_interval.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animator.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animator.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/compressed_float.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint_index.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint_pose.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/rigid_slot.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_helper.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_pose.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_pose.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skin.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skin.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_mesh_part.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_mesh.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_vertex.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_additive_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_additive_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_blend_input_type.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_blend_input.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_clip_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_clip_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_cross_fade_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_cross_fade_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_lerp_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_lerp_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/weight_blending_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/weight_blending_helper.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_key_helper.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_track.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/rotation_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/scale_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/sprite_animation_track.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/sprite_animation_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/translation_track.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/spine
        ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.h
        ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc.h
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/texture_atlas
        ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas.cpp
        ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas.h
        ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas_dynamic.cpp
        ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas_dynamic.h
    >
)

source_group(filesystem            REGULAR_EXPRESSION "pomdog/filesystem/*")
source_group(filesystem/apple      REGULAR_EXPRESSION "pomdog/filesystem/apple/*")
source_group(filesystem/emscripten REGULAR_EXPRESSION "pomdog/filesystem/emscripten/*")
source_group(filesystem/linux      REGULAR_EXPRESSION "pomdog/filesystem/linux/*")
source_group(filesystem/win32      REGULAR_EXPRESSION "pomdog/filesystem/win32/*")

target_sources(pomdog_filesystem PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/filesystem/file_system.h
    ${POMDOG_SRC_DIR}/filesystem/file_system.cpp

    $<$<PLATFORM_ID:Darwin,iOS>:
        # NOTE: filesystem/apple
        ${POMDOG_SRC_DIR}/filesystem/apple/file_system_apple.h
        ${POMDOG_SRC_DIR}/filesystem/apple/file_system_apple.mm
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: filesystem/emscripten
        ${POMDOG_SRC_DIR}/filesystem/emscripten/file_system_emscripten.cpp
        ${POMDOG_SRC_DIR}/filesystem/emscripten/file_system_emscripten.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: filesystem/linux
        ${POMDOG_SRC_DIR}/filesystem/linux/file_system_linux.cpp
        ${POMDOG_SRC_DIR}/filesystem/linux/file_system_linux.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: filesystem/win32
        ${POMDOG_SRC_DIR}/filesystem/win32/file_system_win32.cpp
        ${POMDOG_SRC_DIR}/filesystem/win32/file_system_win32.h
    >
)

source_group(gpu                    REGULAR_EXPRESSION "pomdog/gpu/*")
source_group(gpu/backends           REGULAR_EXPRESSION "pomdog/gpu/backends/*")
source_group(gpu/dxgi               REGULAR_EXPRESSION "pomdog/gpu/dxgi/*")
source_group(gpu/direct3d11         REGULAR_EXPRESSION "pomdog/gpu/direct3d11/*")
source_group(gpu/gl4                REGULAR_EXPRESSION "pomdog/gpu/gl4/*")
source_group(gpu/metal              REGULAR_EXPRESSION "pomdog/gpu/metal/*")
source_group(gpu/vulkan             REGULAR_EXPRESSION "pomdog/gpu/vulkan/*")

target_sources(pomdog_gpu PRIVATE
    # NOTE: graphics
    ${POMDOG_SRC_DIR}/gpu/blend_desc.cpp
    ${POMDOG_SRC_DIR}/gpu/blend_desc.h
    ${POMDOG_SRC_DIR}/gpu/blend_factor.h
    ${POMDOG_SRC_DIR}/gpu/blend_operation.h
    ${POMDOG_SRC_DIR}/gpu/buffer_bind_flags.h
    ${POMDOG_SRC_DIR}/gpu/buffer_desc.h
    ${POMDOG_SRC_DIR}/gpu/buffer_usage.h
    ${POMDOG_SRC_DIR}/gpu/buffer_view.h
    ${POMDOG_SRC_DIR}/gpu/buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/buffer.h
    ${POMDOG_SRC_DIR}/gpu/command_list.cpp
    ${POMDOG_SRC_DIR}/gpu/command_list.h
    ${POMDOG_SRC_DIR}/gpu/command_queue.cpp
    ${POMDOG_SRC_DIR}/gpu/command_queue.h
    ${POMDOG_SRC_DIR}/gpu/comparison_function.h
    ${POMDOG_SRC_DIR}/gpu/constant_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/constant_buffer.h
    ${POMDOG_SRC_DIR}/gpu/cull_mode.h
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_buffer.h
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_desc.cpp
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_desc.h
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_operation.h
    ${POMDOG_SRC_DIR}/gpu/fill_mode.h
    ${POMDOG_SRC_DIR}/gpu/graphics_backend.h
    ${POMDOG_SRC_DIR}/gpu/graphics_device.cpp
    ${POMDOG_SRC_DIR}/gpu/graphics_device.h
    ${POMDOG_SRC_DIR}/gpu/index_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/index_buffer.h
    ${POMDOG_SRC_DIR}/gpu/index_format.h
    ${POMDOG_SRC_DIR}/gpu/input_classification.h
    ${POMDOG_SRC_DIR}/gpu/input_element_format.h
    ${POMDOG_SRC_DIR}/gpu/input_element.h
    ${POMDOG_SRC_DIR}/gpu/input_layout_desc.h
    ${POMDOG_SRC_DIR}/gpu/input_layout_builder.cpp
    ${POMDOG_SRC_DIR}/gpu/input_layout_builder.h
    ${POMDOG_SRC_DIR}/gpu/memory_usage.h
    ${POMDOG_SRC_DIR}/gpu/pipeline_desc.h
    ${POMDOG_SRC_DIR}/gpu/pipeline_state.cpp
    ${POMDOG_SRC_DIR}/gpu/pipeline_state.h
    ${POMDOG_SRC_DIR}/gpu/pixel_format.h
    ${POMDOG_SRC_DIR}/gpu/presentation_parameters.h
    ${POMDOG_SRC_DIR}/gpu/present_mode.h
    ${POMDOG_SRC_DIR}/gpu/primitive_topology.h
    ${POMDOG_SRC_DIR}/gpu/rasterizer_desc.cpp
    ${POMDOG_SRC_DIR}/gpu/rasterizer_desc.h
    ${POMDOG_SRC_DIR}/gpu/render_pass.h
    ${POMDOG_SRC_DIR}/gpu/render_target_blend_desc.cpp
    ${POMDOG_SRC_DIR}/gpu/render_target_blend_desc.h
    ${POMDOG_SRC_DIR}/gpu/render_target2d.cpp
    ${POMDOG_SRC_DIR}/gpu/render_target2d.h
    ${POMDOG_SRC_DIR}/gpu/sampler_desc.cpp
    ${POMDOG_SRC_DIR}/gpu/sampler_desc.h
    ${POMDOG_SRC_DIR}/gpu/sampler_state.cpp
    ${POMDOG_SRC_DIR}/gpu/sampler_state.h
    ${POMDOG_SRC_DIR}/gpu/shader_pipeline_stage.h
    ${POMDOG_SRC_DIR}/gpu/shader.cpp
    ${POMDOG_SRC_DIR}/gpu/shader.h
    ${POMDOG_SRC_DIR}/gpu/stencil_operation.h
    ${POMDOG_SRC_DIR}/gpu/texture_address_mode.h
    ${POMDOG_SRC_DIR}/gpu/texture_desc.h
    ${POMDOG_SRC_DIR}/gpu/texture_filter.h
    ${POMDOG_SRC_DIR}/gpu/texture_usage.h
    ${POMDOG_SRC_DIR}/gpu/texture.cpp
    ${POMDOG_SRC_DIR}/gpu/texture.h
    ${POMDOG_SRC_DIR}/gpu/texture2d.cpp
    ${POMDOG_SRC_DIR}/gpu/texture2d.h
    ${POMDOG_SRC_DIR}/gpu/vertex_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/vertex_buffer.h
    ${POMDOG_SRC_DIR}/gpu/viewport.cpp
    ${POMDOG_SRC_DIR}/gpu/viewport.h

    # NOTE: gpu/backends
    ${POMDOG_SRC_DIR}/gpu/backends/buffer_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/buffer_helper.h
    ${POMDOG_SRC_DIR}/gpu/backends/command_list_immediate.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/command_list_immediate.h
    ${POMDOG_SRC_DIR}/gpu/backends/command_queue_immediate.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/command_queue_immediate.h
    ${POMDOG_SRC_DIR}/gpu/backends/graphics_capabilities.h
    ${POMDOG_SRC_DIR}/gpu/backends/graphics_context.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/graphics_context.h
    ${POMDOG_SRC_DIR}/gpu/backends/shader_compile_options.h
    ${POMDOG_SRC_DIR}/gpu/backends/surface_format_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/surface_format_helper.h
    ${POMDOG_SRC_DIR}/gpu/backends/texture_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/texture_helper.h

    $<$<AND:$<PLATFORM_ID:Windows,Linux,Darwin,Emscripten>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: gpu/gl4
        ${POMDOG_SRC_DIR}/gpu/gl4/blend_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/blend_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/buffer_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_buffer_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/effect_reflection_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/effect_reflection_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/error_checker.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/error_checker.h
        ${POMDOG_SRC_DIR}/gpu/gl4/format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/format_helper.h
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_context_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_context_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_device_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_device_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/input_layout_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/input_layout_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/opengl_context.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/opengl_context.h
        ${POMDOG_SRC_DIR}/gpu/gl4/opengl_prerequisites.h
        ${POMDOG_SRC_DIR}/gpu/gl4/pipeline_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/pipeline_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/rasterizer_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/rasterizer_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/render_target2d_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/render_target2d_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/sampler_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/sampler_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/shader_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/shader_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/texture2d_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/texture2d_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/typesafe_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/typesafe_helper_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/vertex_buffer_binding_gl4.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: gpu/dxgi
        ${POMDOG_SRC_DIR}/gpu/dxgi/dxgi_format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/dxgi/dxgi_format_helper.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: gpu/direct3d11
        ${POMDOG_SRC_DIR}/gpu/direct3d11/buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/buffer_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/depth_stencil_buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/depth_stencil_buffer_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/format_helper.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_context_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_context_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_device_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_device_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/pipeline_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/pipeline_state_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/prerequisites_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/render_target2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/render_target2d_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/sampler_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/sampler_state_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/shader_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/shader_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/texture2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/texture2d_direct3d11.h
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: gpu/metal
        ${POMDOG_SRC_DIR}/gpu/metal/buffer_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/buffer_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/constants_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/depth_stencil_buffer_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/depth_stencil_buffer_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/frame_counter.cpp
        ${POMDOG_SRC_DIR}/gpu/metal/frame_counter.h
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_context_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_context_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_device_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_device_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/metal_format_helper.h
        ${POMDOG_SRC_DIR}/gpu/metal/metal_format_helper.mm
        ${POMDOG_SRC_DIR}/gpu/metal/pipeline_state_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/pipeline_state_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/render_target2d_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/render_target2d_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/sampler_state_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/sampler_state_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/shader_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/shader_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/texture2d_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/texture2d_metal.mm
    >

    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        # NOTE: gpu/vulkan
        ${POMDOG_SRC_DIR}/gpu/vulkan/buffer_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/buffer_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_list_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_list_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_queue_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_queue_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/depth_stencil_buffer_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/depth_stencil_buffer_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/format_helper.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/graphics_device_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/graphics_device_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/pipeline_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/pipeline_state_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/prerequisites_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/render_target2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/render_target2d_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/sampler_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/sampler_state_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/shader_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/shader_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/swap_chain_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/swap_chain_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/texture2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/texture2d_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/vulkan_format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/vulkan_format_helper.h
    >
)

source_group(graphics REGULAR_EXPRESSION "pomdog/graphics/*")

target_sources(pomdog_graphics PRIVATE
    ${POMDOG_SRC_DIR}/graphics/basic_effect.h
    ${POMDOG_SRC_DIR}/graphics/basic_effect.cpp
    ${POMDOG_SRC_DIR}/graphics/billboard_effect.h
    ${POMDOG_SRC_DIR}/graphics/billboard_effect.cpp
    ${POMDOG_SRC_DIR}/graphics/font_glyph.h
    ${POMDOG_SRC_DIR}/graphics/line_batch.h
    ${POMDOG_SRC_DIR}/graphics/line_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/polyline_batch.h
    ${POMDOG_SRC_DIR}/graphics/polyline_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/polygon_shape_builder.h
    ${POMDOG_SRC_DIR}/graphics/polygon_shape_builder.cpp
    ${POMDOG_SRC_DIR}/graphics/primitive_batch.h
    ${POMDOG_SRC_DIR}/graphics/primitive_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_batch.h
    ${POMDOG_SRC_DIR}/graphics/sprite_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_font.h
    ${POMDOG_SRC_DIR}/graphics/sprite_font.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_font_sdf.h
    ${POMDOG_SRC_DIR}/graphics/sprite_font_sdf.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_line.h
    ${POMDOG_SRC_DIR}/graphics/sprite_line.cpp
    ${POMDOG_SRC_DIR}/graphics/texture_region.h
    ${POMDOG_SRC_DIR}/graphics/truetype_font.h
    ${POMDOG_SRC_DIR}/graphics/truetype_font.cpp
    ${POMDOG_SRC_DIR}/graphics/wireframe_effect.h
    ${POMDOG_SRC_DIR}/graphics/wireframe_effect.cpp
)

source_group(input              REGULAR_EXPRESSION "pomdog/input/*")
source_group(input/backends     REGULAR_EXPRESSION "pomdog/input/backends/*")
source_group(input/directinput  REGULAR_EXPRESSION "pomdog/input/directinput/*")
source_group(input/emscripten   REGULAR_EXPRESSION "pomdog/input/emscripten/*")
source_group(input/iokit        REGULAR_EXPRESSION "pomdog/input/iokit/*")
source_group(input/linux        REGULAR_EXPRESSION "pomdog/input/linux/*")

target_sources(pomdog_input PRIVATE
    # NOTE: input
    ${POMDOG_SRC_DIR}/input/button_state.h
    ${POMDOG_SRC_DIR}/input/game_controller_db.cpp
    ${POMDOG_SRC_DIR}/input/game_controller_db.h
    ${POMDOG_SRC_DIR}/input/gamepad_buttons.h
    ${POMDOG_SRC_DIR}/input/gamepad_capabilities.h
    ${POMDOG_SRC_DIR}/input/gamepad_service.cpp
    ${POMDOG_SRC_DIR}/input/gamepad_service.h
    ${POMDOG_SRC_DIR}/input/gamepad_uuid.cpp
    ${POMDOG_SRC_DIR}/input/gamepad_uuid.h
    ${POMDOG_SRC_DIR}/input/gamepad.cpp
    ${POMDOG_SRC_DIR}/input/gamepad.h
    ${POMDOG_SRC_DIR}/input/gesture_tracker.cpp
    ${POMDOG_SRC_DIR}/input/gesture_tracker.h
    ${POMDOG_SRC_DIR}/input/key_state.h
    ${POMDOG_SRC_DIR}/input/keyboard.cpp
    ${POMDOG_SRC_DIR}/input/keyboard.h
    ${POMDOG_SRC_DIR}/input/keys.h
    ${POMDOG_SRC_DIR}/input/mouse_buttons.h
    ${POMDOG_SRC_DIR}/input/mouse.cpp
    ${POMDOG_SRC_DIR}/input/mouse.h
    ${POMDOG_SRC_DIR}/input/player_index.h
    ${POMDOG_SRC_DIR}/input/touchscreen.cpp
    ${POMDOG_SRC_DIR}/input/touchscreen.h

    # NOTE: input/backends
    ${POMDOG_SRC_DIR}/input/backends/gamepad_helper.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_helper.h
    ${POMDOG_SRC_DIR}/input/backends/gamepad_impl.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_impl.h
    ${POMDOG_SRC_DIR}/input/backends/gamepad_mapping_entry.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_mapping_entry.h
    ${POMDOG_SRC_DIR}/input/backends/keyboard_impl.cpp
    ${POMDOG_SRC_DIR}/input/backends/keyboard_impl.h
    ${POMDOG_SRC_DIR}/input/backends/mouse_impl.cpp
    ${POMDOG_SRC_DIR}/input/backends/mouse_impl.h

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: input/emscripten
        ${POMDOG_SRC_DIR}/input/emscripten/gamepad_emscripten.cpp
        ${POMDOG_SRC_DIR}/input/emscripten/gamepad_emscripten.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: input/directinput
        ${POMDOG_SRC_DIR}/input/directinput/gamepad_directinput.cpp
        ${POMDOG_SRC_DIR}/input/directinput/gamepad_directinput.h
        ${POMDOG_SRC_DIR}/input/directinput/prerequisites_directinput.h
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: input/iokit
        ${POMDOG_SRC_DIR}/input/iokit/gamepad_iokit.cpp
        ${POMDOG_SRC_DIR}/input/iokit/gamepad_iokit.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: input/linux
        ${POMDOG_SRC_DIR}/input/linux/gamepad_linux.cpp
        ${POMDOG_SRC_DIR}/input/linux/gamepad_linux.h
    >
)

source_group(logging REGULAR_EXPRESSION "pomdog/logging/*")

target_sources(pomdog_base PRIVATE
    # NOTE: logging
    ${POMDOG_SRC_DIR}/logging/log.h
    ${POMDOG_SRC_DIR}/logging/log.cpp
    ${POMDOG_SRC_DIR}/logging/log_channel.h
    ${POMDOG_SRC_DIR}/logging/log_channel.cpp
    ${POMDOG_SRC_DIR}/logging/log_entry.h
    ${POMDOG_SRC_DIR}/logging/log_level.h
)

source_group(math REGULAR_EXPRESSION "pomdog/math/*")

target_sources(pomdog_math PRIVATE
    # NOTE: math
    ${POMDOG_SRC_DIR}/math/bounding_box.cpp
    ${POMDOG_SRC_DIR}/math/bounding_box.h
    ${POMDOG_SRC_DIR}/math/bounding_box2d.cpp
    ${POMDOG_SRC_DIR}/math/bounding_box2d.h
    ${POMDOG_SRC_DIR}/math/bounding_box_xz.cpp
    ${POMDOG_SRC_DIR}/math/bounding_box_xz.h
    ${POMDOG_SRC_DIR}/math/bounding_circle.cpp
    ${POMDOG_SRC_DIR}/math/bounding_circle.h
    ${POMDOG_SRC_DIR}/math/bounding_circle_xz.cpp
    ${POMDOG_SRC_DIR}/math/bounding_circle_xz.h
    ${POMDOG_SRC_DIR}/math/bounding_frustum.cpp
    ${POMDOG_SRC_DIR}/math/bounding_frustum.h
    ${POMDOG_SRC_DIR}/math/bounding_sphere.cpp
    ${POMDOG_SRC_DIR}/math/bounding_sphere.h
    ${POMDOG_SRC_DIR}/math/color.cpp
    ${POMDOG_SRC_DIR}/math/color.h
    ${POMDOG_SRC_DIR}/math/containment_type.h
    ${POMDOG_SRC_DIR}/math/degree.cpp
    ${POMDOG_SRC_DIR}/math/degree.h
    ${POMDOG_SRC_DIR}/math/easing.cpp
    ${POMDOG_SRC_DIR}/math/easing.h
    ${POMDOG_SRC_DIR}/math/f32_color.cpp
    ${POMDOG_SRC_DIR}/math/f32_color.h
    ${POMDOG_SRC_DIR}/math/f32_rect2d.cpp
    ${POMDOG_SRC_DIR}/math/f32_rect2d.h
    ${POMDOG_SRC_DIR}/math/math_constants.h
    ${POMDOG_SRC_DIR}/math/math_functions.cpp
    ${POMDOG_SRC_DIR}/math/math_functions.h
    ${POMDOG_SRC_DIR}/math/matrix2x2.cpp
    ${POMDOG_SRC_DIR}/math/matrix2x2.h
    ${POMDOG_SRC_DIR}/math/matrix3x2.cpp
    ${POMDOG_SRC_DIR}/math/matrix3x2.h
    ${POMDOG_SRC_DIR}/math/matrix3x3.cpp
    ${POMDOG_SRC_DIR}/math/matrix3x3.h
    ${POMDOG_SRC_DIR}/math/matrix4x4.cpp
    ${POMDOG_SRC_DIR}/math/matrix4x4.h
    ${POMDOG_SRC_DIR}/math/plane_intersection_type.h
    ${POMDOG_SRC_DIR}/math/plane.cpp
    ${POMDOG_SRC_DIR}/math/plane.h
    ${POMDOG_SRC_DIR}/math/point2d.cpp
    ${POMDOG_SRC_DIR}/math/point2d.h
    ${POMDOG_SRC_DIR}/math/point3d.cpp
    ${POMDOG_SRC_DIR}/math/point3d.h
    ${POMDOG_SRC_DIR}/math/point_xz.cpp
    ${POMDOG_SRC_DIR}/math/point_xz.h
    ${POMDOG_SRC_DIR}/math/pixel_ratio_conversion.cpp
    ${POMDOG_SRC_DIR}/math/pixel_ratio_conversion.h
    ${POMDOG_SRC_DIR}/math/quaternion.cpp
    ${POMDOG_SRC_DIR}/math/quaternion.h
    ${POMDOG_SRC_DIR}/math/radian.cpp
    ${POMDOG_SRC_DIR}/math/radian.h
    ${POMDOG_SRC_DIR}/math/ray.cpp
    ${POMDOG_SRC_DIR}/math/ray.h
    ${POMDOG_SRC_DIR}/math/raycast.cpp
    ${POMDOG_SRC_DIR}/math/raycast.h
    ${POMDOG_SRC_DIR}/math/rect2d.cpp
    ${POMDOG_SRC_DIR}/math/rect2d.h
    ${POMDOG_SRC_DIR}/math/rect_xz.cpp
    ${POMDOG_SRC_DIR}/math/rect_xz.h
    ${POMDOG_SRC_DIR}/math/vector2.cpp
    ${POMDOG_SRC_DIR}/math/vector2.h
    ${POMDOG_SRC_DIR}/math/vector3.cpp
    ${POMDOG_SRC_DIR}/math/vector3.h
    ${POMDOG_SRC_DIR}/math/vector4.cpp
    ${POMDOG_SRC_DIR}/math/vector4.h
    ${POMDOG_SRC_DIR}/math/vector_xz.cpp
    ${POMDOG_SRC_DIR}/math/vector_xz.h
)

source_group(memory REGULAR_EXPRESSION "pomdog/memory/*")

target_sources(pomdog_base PRIVATE
    # NOTE: memory
    ${POMDOG_SRC_DIR}/memory/aligned_alloc.cpp
    ${POMDOG_SRC_DIR}/memory/aligned_alloc.h
    ${POMDOG_SRC_DIR}/memory/aligned_byte_array.h
    ${POMDOG_SRC_DIR}/memory/aligned_new.h
    ${POMDOG_SRC_DIR}/memory/alignment.cpp
    ${POMDOG_SRC_DIR}/memory/alignment.h
    ${POMDOG_SRC_DIR}/memory/linear_allocator.cpp
    ${POMDOG_SRC_DIR}/memory/linear_allocator.h
    ${POMDOG_SRC_DIR}/memory/linear_page_allocator.cpp
    ${POMDOG_SRC_DIR}/memory/linear_page_allocator.h
    ${POMDOG_SRC_DIR}/memory/memcpy_span.h
    ${POMDOG_SRC_DIR}/memory/memset_span.h
    ${POMDOG_SRC_DIR}/memory/placement_new.h
    ${POMDOG_SRC_DIR}/memory/raw_ptr.h
    ${POMDOG_SRC_DIR}/memory/unsafe_ptr.h
)

source_group(network            REGULAR_EXPRESSION "pomdog/network/*")
source_group(network/mbedtls    REGULAR_EXPRESSION "pomdog/network/mbedtls/*")
source_group(network/posix      REGULAR_EXPRESSION "pomdog/network/posix/*")
source_group(network/win32      REGULAR_EXPRESSION "pomdog/network/win32/*")

target_sources(pomdog_network PRIVATE
    # NOTE: network
    ${POMDOG_SRC_DIR}/network/address_family.h
    ${POMDOG_SRC_DIR}/network/address_parser.cpp
    ${POMDOG_SRC_DIR}/network/address_parser.h
    ${POMDOG_SRC_DIR}/network/forward_declarations.h
    ${POMDOG_SRC_DIR}/network/http_client.cpp
    ${POMDOG_SRC_DIR}/network/http_client.h
    ${POMDOG_SRC_DIR}/network/http_method.h
    ${POMDOG_SRC_DIR}/network/http_parser.cpp
    ${POMDOG_SRC_DIR}/network/http_parser.h
    ${POMDOG_SRC_DIR}/network/http_request.cpp
    ${POMDOG_SRC_DIR}/network/http_request.h
    ${POMDOG_SRC_DIR}/network/http_response.cpp
    ${POMDOG_SRC_DIR}/network/http_response.h
    ${POMDOG_SRC_DIR}/network/io_service.cpp
    ${POMDOG_SRC_DIR}/network/io_service.h
    ${POMDOG_SRC_DIR}/network/socket_protocol.h
    ${POMDOG_SRC_DIR}/network/tcp_stream.cpp
    ${POMDOG_SRC_DIR}/network/tcp_stream.h
    ${POMDOG_SRC_DIR}/network/tls_stream.cpp
    ${POMDOG_SRC_DIR}/network/tls_stream.h
    ${POMDOG_SRC_DIR}/network/udp_stream.cpp
    ${POMDOG_SRC_DIR}/network/udp_stream.h

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        ${POMDOG_SRC_DIR}/network/end_point.cpp
        ${POMDOG_SRC_DIR}/network/end_point.h
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: network/emscripten
        ${POMDOG_SRC_DIR}/network/emscripten/tcp_stream_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/tcp_stream_emscripten.h
        ${POMDOG_SRC_DIR}/network/emscripten/tls_stream_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/tls_stream_emscripten.h
        ${POMDOG_SRC_DIR}/network/emscripten/udp_stream_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/udp_stream_emscripten.h
    >

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # NOTE: network/mbedtls
        ${POMDOG_SRC_DIR}/network/mbedtls/certificates.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/certificates.h
        ${POMDOG_SRC_DIR}/network/mbedtls/tls_stream_mbedtls.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/tls_stream_mbedtls.h
    >

    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: network/posix
        ${POMDOG_SRC_DIR}/network/posix/socket_helper_posix.cpp
        ${POMDOG_SRC_DIR}/network/posix/socket_helper_posix.h
        ${POMDOG_SRC_DIR}/network/posix/tcp_stream_posix.cpp
        ${POMDOG_SRC_DIR}/network/posix/tcp_stream_posix.h
        ${POMDOG_SRC_DIR}/network/posix/udp_stream_posix.cpp
        ${POMDOG_SRC_DIR}/network/posix/udp_stream_posix.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: network/win32
        ${POMDOG_SRC_DIR}/network/win32/io_service_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/io_service_win32.h
        ${POMDOG_SRC_DIR}/network/win32/socket_helper_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/socket_helper_win32.h
        ${POMDOG_SRC_DIR}/network/win32/tcp_stream_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/tcp_stream_win32.h
        ${POMDOG_SRC_DIR}/network/win32/udp_stream_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/udp_stream_win32.h
    >
)

source_group(random REGULAR_EXPRESSION "pomdog/random/*")

target_sources(pomdog_random PRIVATE
    # NOTE: random
    ${POMDOG_SRC_DIR}/random/uniform_int_distribution.h
    ${POMDOG_SRC_DIR}/random/xoroshiro128_star_star.h
    ${POMDOG_SRC_DIR}/random/xoroshiro128_star_star.cpp
)

source_group(signals REGULAR_EXPRESSION "pomdog/signals/*")

target_sources(pomdog_base PRIVATE
    # NOTE: signals
    ${POMDOG_SRC_DIR}/signals/connection.cpp
    ${POMDOG_SRC_DIR}/signals/connection.h
    ${POMDOG_SRC_DIR}/signals/connection_list.cpp
    ${POMDOG_SRC_DIR}/signals/connection_list.h
    ${POMDOG_SRC_DIR}/signals/delegate.h
    ${POMDOG_SRC_DIR}/signals/forward_declarations.h
    ${POMDOG_SRC_DIR}/signals/scoped_connection.cpp
    ${POMDOG_SRC_DIR}/signals/scoped_connection.h
    ${POMDOG_SRC_DIR}/signals/signal_helpers.h
    ${POMDOG_SRC_DIR}/signals/signal.h
    ${POMDOG_SRC_DIR}/signals/detail/delegate_body.h
    ${POMDOG_SRC_DIR}/signals/detail/signal_body.cpp
    ${POMDOG_SRC_DIR}/signals/detail/signal_body.h
)

source_group(utility REGULAR_EXPRESSION "pomdog/utility/*")

target_sources(pomdog_base PRIVATE
    # NOTE: utility
    ${POMDOG_SRC_DIR}/utility/assert.cpp
    ${POMDOG_SRC_DIR}/utility/assert.h
    ${POMDOG_SRC_DIR}/utility/bit_cast_span.h
    ${POMDOG_SRC_DIR}/utility/bit.h
    ${POMDOG_SRC_DIR}/utility/circular_buffer.h
    ${POMDOG_SRC_DIR}/utility/cli_parser.cpp
    ${POMDOG_SRC_DIR}/utility/cli_parser.h
    ${POMDOG_SRC_DIR}/utility/concepts.h
    ${POMDOG_SRC_DIR}/utility/crc32.cpp
    ${POMDOG_SRC_DIR}/utility/crc32.h
    ${POMDOG_SRC_DIR}/utility/enum_cast.h
    ${POMDOG_SRC_DIR}/utility/error_helper.cpp
    ${POMDOG_SRC_DIR}/utility/error_helper.h
    ${POMDOG_SRC_DIR}/utility/errors.cpp
    ${POMDOG_SRC_DIR}/utility/errors.h
    ${POMDOG_SRC_DIR}/utility/exception.h
    ${POMDOG_SRC_DIR}/utility/fixed_vector.h
    ${POMDOG_SRC_DIR}/utility/path_helper.cpp
    ${POMDOG_SRC_DIR}/utility/path_helper.h
    ${POMDOG_SRC_DIR}/utility/scope_guard.cpp
    ${POMDOG_SRC_DIR}/utility/scope_guard.h
    ${POMDOG_SRC_DIR}/utility/spin_lock.cpp
    ${POMDOG_SRC_DIR}/utility/spin_lock.h
    ${POMDOG_SRC_DIR}/utility/static_down_cast.h
    ${POMDOG_SRC_DIR}/utility/string_compiletime.h
    ${POMDOG_SRC_DIR}/utility/string_format.h
    ${POMDOG_SRC_DIR}/utility/string_hash32.cpp
    ${POMDOG_SRC_DIR}/utility/string_hash32.h
    ${POMDOG_SRC_DIR}/utility/string_hash64.cpp
    ${POMDOG_SRC_DIR}/utility/string_hash64.h
    ${POMDOG_SRC_DIR}/utility/string_helper.cpp
    ${POMDOG_SRC_DIR}/utility/string_helper.h
    ${POMDOG_SRC_DIR}/utility/tagged.h
    ${POMDOG_SRC_DIR}/utility/utfcpp_headers.h
    ${POMDOG_SRC_DIR}/utility/variant_index.h
    ${POMDOG_SRC_DIR}/utility/xxhash_read.h
    ${POMDOG_SRC_DIR}/utility/xxhash32.cpp
    ${POMDOG_SRC_DIR}/utility/xxhash32.h
    ${POMDOG_SRC_DIR}/utility/xxhash64.cpp
    ${POMDOG_SRC_DIR}/utility/xxhash64.h
)

source_group(vfs REGULAR_EXPRESSION "pomdog/vfs/*")

target_sources(pomdog_vfs PRIVATE
    # NOTE: vfs
    ${POMDOG_SRC_DIR}/vfs/default_vfs_setup.h
    ${POMDOG_SRC_DIR}/vfs/default_vfs_setup.cpp
    ${POMDOG_SRC_DIR}/vfs/file.h
    ${POMDOG_SRC_DIR}/vfs/file.cpp
    ${POMDOG_SRC_DIR}/vfs/file_archive.h
    ${POMDOG_SRC_DIR}/vfs/file_archive.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        ${POMDOG_SRC_DIR}/vfs/file_archive_mmap.h
        ${POMDOG_SRC_DIR}/vfs/file_archive_mmap.cpp
    >
    ${POMDOG_SRC_DIR}/vfs/file_physical.h
    ${POMDOG_SRC_DIR}/vfs/file_physical.cpp
    ${POMDOG_SRC_DIR}/vfs/file_system.h
    ${POMDOG_SRC_DIR}/vfs/file_system.cpp
    ${POMDOG_SRC_DIR}/vfs/mount_volume.h
    ${POMDOG_SRC_DIR}/vfs/mount_volume.cpp
)

target_sources(pomdog_content_utility PRIVATE
    ${POMDOG_SRC_DIR}/content/utility/binary_reader.h
    ${POMDOG_SRC_DIR}/content/utility/binary_reader.cpp
    ${POMDOG_SRC_DIR}/content/utility/make_fourcc.h
)

target_sources(pomdog_content_gpu PRIVATE
    ${POMDOG_SRC_DIR}/content/shader_loader.h
    ${POMDOG_SRC_DIR}/content/shader_loader.cpp
    ${POMDOG_SRC_DIR}/content/texture_loader.h
    ${POMDOG_SRC_DIR}/content/texture_loader.cpp
    ${POMDOG_SRC_DIR}/content/image/image_container.h
    ${POMDOG_SRC_DIR}/content/image/dds.h
    ${POMDOG_SRC_DIR}/content/image/dds.cpp
    ${POMDOG_SRC_DIR}/content/image/png.h
    ${POMDOG_SRC_DIR}/content/image/png.cpp
    ${POMDOG_SRC_DIR}/content/image/pnm.h
    ${POMDOG_SRC_DIR}/content/image/pnm.cpp
)
target_sources(pomdog_content_audio PRIVATE
    ${POMDOG_SRC_DIR}/content/audio_clip_loader.h
    ${POMDOG_SRC_DIR}/content/audio_clip_loader.cpp
    ${POMDOG_SRC_DIR}/content/audio/audio_container.h
    ${POMDOG_SRC_DIR}/content/audio/audio_container.cpp
    ${POMDOG_SRC_DIR}/content/audio/vorbis.h
    ${POMDOG_SRC_DIR}/content/audio/vorbis.cpp
    ${POMDOG_SRC_DIR}/content/audio/wav.h
    ${POMDOG_SRC_DIR}/content/audio/wav.cpp
)
target_sources(pomdog_content_input PRIVATE
    ${POMDOG_SRC_DIR}/content/game_controller_db_loader.h
    ${POMDOG_SRC_DIR}/content/game_controller_db_loader.cpp
)

add_library(pomdog_build_settings INTERFACE)

set(POMDOG_COMPILE_TARGETS
    pomdog_base
    pomdog_math
    pomdog_random
    pomdog_filesystem
    pomdog_vfs
    pomdog_input
    pomdog_gpu
    pomdog_audio
    pomdog_network
    pomdog_application
    pomdog_application_gpu
    pomdog_content_utility
    pomdog_content_gpu
    pomdog_content_audio
    pomdog_content_input
    pomdog_graphics
    pomdog_ecs
    pomdog_console
    pomdog_experimental
)

source_group(content REGULAR_EXPRESSION "pomdog/content/*")
source_group(content/audio REGULAR_EXPRESSION "pomdog/content/audio/*")
source_group(content/image REGULAR_EXPRESSION "pomdog/content/image/*")
source_group(content/utility REGULAR_EXPRESSION "pomdog/content/utility/*")

set_target_properties(${POMDOG_COMPILE_TARGETS} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    # NOTE: Sources use headers, not C++ named modules (CMP0155).
    CXX_SCAN_FOR_MODULES OFF
)

add_subdirectory(${POMDOG_DIR}/cmake/fmt ${CMAKE_CURRENT_BINARY_DIR}/fmt_build)
add_subdirectory(${POMDOG_DIR}/cmake/libpng "${CMAKE_CURRENT_BINARY_DIR}/libpng_build")
add_subdirectory(${POMDOG_DIR}/cmake/mbedtls "${CMAKE_CURRENT_BINARY_DIR}/mbedtls_build")
add_subdirectory(${POMDOG_DIR}/cmake/stb "${CMAKE_CURRENT_BINARY_DIR}/stb_build")
add_subdirectory(${POMDOG_DIR}/cmake/zlib "${CMAKE_CURRENT_BINARY_DIR}/zlib_build")

if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Linux"))
    add_subdirectory(${POMDOG_DIR}/cmake/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
endif()
if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
    add_subdirectory(${POMDOG_DIR}/cmake/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
endif()
if(POMDOG_USE_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

target_include_directories(pomdog_build_settings INTERFACE
    ${POMDOG_DIR}
    ${POMDOG_DIR}/build/schemas-cpp
    ${POMDOG_DIR}/thirdparty
    ${POMDOG_DIR}/thirdparty/flatbuffers/include
)

target_include_directories(pomdog_experimental PRIVATE
    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        ${POMDOG_DIR}/thirdparty/rapidjson/include
    >
)

# NOTE: Public headers establish the shared include path and language requirement.
target_include_directories(pomdog_base PUBLIC
    ${POMDOG_DIR}
    ${POMDOG_DIR}/thirdparty/fmt/include
)
target_compile_features(pomdog_base PUBLIC cxx_std_23)

# NOTE: MSVC consumers must use the same static CRT as the engine and its dependencies.
target_compile_options(pomdog_base PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:$<IF:$<CONFIG:Debug>,/MTd,/MT>>
)
target_link_libraries(pomdog_base
    PUBLIC
        fmt_static
    PRIVATE
        pomdog_build_settings
        $<$<PLATFORM_ID:Windows>:
            kernel32.lib
        >
        $<$<PLATFORM_ID:Linux>:
            pthread
        >
)
target_link_libraries(pomdog_math
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)
target_link_libraries(pomdog_random
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)
target_link_libraries(pomdog_filesystem
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        $<$<PLATFORM_ID:Windows>:
            shell32.lib
            ole32.lib
        >
)
target_link_libraries(pomdog_vfs
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        pomdog_filesystem
)
target_link_libraries(pomdog_input
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_DIRECTINPUT}>>:
            dinput8.lib
            dxguid.lib
            ole32.lib

            # NOTE: for SysAllocString
            oleaut32.lib
        >
)
target_link_libraries(pomdog_gpu
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_DIRECT3D11}>>:
            dxgi.lib
            d3d11.lib
        >
        $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
            opengl32.lib
        >
        $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
            GL
        >
        $<$<AND:$<PLATFORM_ID:Windows,Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
            glew_static
        >
        $<$<BOOL:${POMDOG_USE_VULKAN}>:
            Vulkan::Vulkan
        >
)
target_link_libraries(pomdog_audio
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        pomdog_math
        $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_XAUDIO2}>>:
            xaudio2.lib
        >
        $<$<AND:$<PLATFORM_ID:Linux,Emscripten>,$<BOOL:${POMDOG_USE_OPENAL}>>:
            openal
        >
)
target_link_libraries(pomdog_network
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        $<$<PLATFORM_ID:Windows>:
            ws2_32.lib
        >
        $<$<PLATFORM_ID:Windows,Linux,Darwin>:
            mbedtls
        >
)
target_link_libraries(pomdog_application
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)
target_link_libraries(pomdog_application_gpu
    PUBLIC
        pomdog_application
        pomdog_gpu
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_input
        pomdog_audio
        pomdog_filesystem
        $<$<PLATFORM_ID:Windows,Linux,Darwin>:
            pomdog_network
        >
        $<$<PLATFORM_ID:Windows>:
            user32.lib
            gdi32.lib
            ole32.lib
            winmm.lib

            # NOTE: Dark mode theme
            UxTheme.lib
            Dwmapi.lib

            # NOTE: for GetFileVersionInfoSizeW()
            Version.lib
        >
        $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_X11}>>:
            X11
        >
        $<$<AND:$<PLATFORM_ID:Windows,Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
            glew_static
        >
        $<$<BOOL:${POMDOG_USE_VULKAN}>:
            Vulkan::Vulkan
        >
)
target_link_libraries(pomdog_content_utility
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)
target_link_libraries(pomdog_content_gpu
    PUBLIC
        pomdog_gpu
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
        pomdog_content_utility
        pomdog_math
        png
)
target_link_libraries(pomdog_content_audio
    PUBLIC
        pomdog_audio
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
        pomdog_content_utility
        stb_static
)
target_link_libraries(pomdog_content_input
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
        pomdog_content_utility
        pomdog_input
)
target_link_libraries(pomdog_graphics
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_gpu
        pomdog_vfs
        pomdog_content_gpu
        stb_static
)
target_link_libraries(pomdog_ecs
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)
target_link_libraries(pomdog_console
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)

# NOTE: Keep legacy experimental features together until their own modules are defined.
target_link_libraries(pomdog_experimental
    PUBLIC
        pomdog_base
        $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
            pomdog_math
            pomdog_graphics
            pomdog_input
            pomdog_application_gpu
            pomdog_random
        >
    PRIVATE
        pomdog_build_settings
        $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
            pomdog_content_gpu
            pomdog_filesystem
            pomdog_vfs
            pomdog_gpu
        >
)
target_link_libraries(pomdog_static INTERFACE
    pomdog::application
    pomdog::application_gpu
    pomdog::audio
    pomdog::base
    pomdog::console
    pomdog::content_audio
    pomdog::content_gpu
    pomdog::content_input
    pomdog::content_utility
    pomdog::ecs
    pomdog::experimental
    pomdog::filesystem
    pomdog::gpu
    pomdog::graphics
    pomdog::input
    pomdog::math
    pomdog::network
    pomdog::random
    pomdog::vfs
)

# NOTE: Static libraries pass platform link requirements to their consumers.
target_link_options(pomdog_filesystem INTERFACE
    $<$<PLATFORM_ID:Darwin>:
        "SHELL:-framework Foundation"
    >
    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: Enable file system support for Emscripten
        "SHELL:-lidbfs.js"
    >
)
target_link_options(pomdog_input INTERFACE
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_IOKIT}>>:
        "SHELL:-framework IOKit"
    >
)
target_link_options(pomdog_gpu INTERFACE
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_METAL}>>:
        "SHELL:-framework Metal"
        "SHELL:-framework MetalKit"
    >
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        "SHELL:-framework OpenGL"
    >
    $<$<PLATFORM_ID:Emscripten>:
        "SHELL:-s MIN_WEBGL_VERSION=2"
        "SHELL:-s MAX_WEBGL_VERSION=2"
        "SHELL:-s FULL_ES3=1"
    >
)
target_link_options(pomdog_audio INTERFACE
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_OPENAL}>>:
        "SHELL:-framework AudioToolBox"
        "SHELL:-framework OpenAL"
    >
)
target_link_options(pomdog_application_gpu INTERFACE
    $<$<PLATFORM_ID:Darwin>:
        "SHELL:-framework Cocoa"
        "SHELL:-framework QuartzCore"
    >
    $<$<PLATFORM_ID:Emscripten>:
        "SHELL:-s OFFSCREEN_FRAMEBUFFER=1"
    >
)

target_compile_definitions(pomdog_build_settings INTERFACE
    $<$<CONFIG:Debug>:POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        _WIN32_WINNT=0x0A00 # NOTE: Windows 10 or later
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)
target_compile_definitions(pomdog_application_gpu PRIVATE
    $<$<BOOL:${POMDOG_USE_GL4}>:
        POMDOG_USE_GL4
    >
    $<$<BOOL:${POMDOG_USE_DIRECT3D11}>:
        POMDOG_USE_DIRECT3D11
    >
    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        POMDOG_USE_VULKAN
    >
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        GL_SILENCE_DEPRECATION # For silencing OpenGL warnings on macOS >= 10.14.
    >
)
target_compile_definitions(pomdog_gpu PRIVATE
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        GL_SILENCE_DEPRECATION # For silencing OpenGL warnings on macOS >= 10.14.
    >
)

target_compile_options(pomdog_build_settings INTERFACE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
        -Werror
        -Wnon-virtual-dtor

        $<$<PLATFORM_ID:Linux,Windows,Darwin>:
            -mavx # NOTE: Enable AVX for GCC and Clang
        >

        $<$<PLATFORM_ID:Linux,Windows,Darwin>:
            # Linux, Windows and macOS
            $<$<CONFIG:Debug>:-g;-O0>
            $<$<CONFIG:Release>:-O3>
        >
        $<$<PLATFORM_ID:Emscripten>:
            # Emscripten
            $<$<CONFIG:Debug>:-g3;-O0>
            $<$<CONFIG:Release>:-O3>
        >

        # NOTE: excluding clang-cl
        $<$<NOT:$<BOOL:MSVC>>:
            -pedantic
            -pedantic-errors
            -fvisibility=hidden
        >
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-compat-bind-to-temporary-copy
        -Wno-c++98-compat-local-type-template-args
        -Wno-padded

        # FIXME: Remove the following `-Wno-...` flags.
        -Wno-float-equal
        -Wno-shadow-uncaptured-local
        -Wno-sign-conversion
        -Wno-switch-enum
        -Wno-undefined-func-template
        -Wno-implicit-int-float-conversion

        # FIXME: Conflict between C++20 aggregate initializations and C99 desinated initializers.
        -Wno-c99-extensions

        # FIXME: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-itemplate
        # FIXME: https://bugs.llvm.org/show_bug.cgi?id=18733
        -Wno-weak-template-vtables

        # FIXME: https://github.com/llvm/llvm-project/issues/81354
        -Wno-switch-default

        $<$<PLATFORM_ID:Linux>:
            # FIXME: Remove the following `-Wno` flags for Linux.
            -Wno-unsafe-buffer-usage
        >
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>:
        -fsanitize=address # NOTE: Enable address sanitizer
        -g # NOTE: Generate debugging information to support address sanitizer
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,21>>:
        # NOTE: Disable thread safety analysis for Linux with Clang >= 21, because it causes false positives.
        -Wno-thread-safety-negative

        # FIXME: Disable NRVO warning, because it causes false positives in some cases.
        -Wno-nrvo
    >
    $<$<CXX_COMPILER_ID:AppleClang>:
        # FIXME: Remove the following `-Wno` flags.
        -Wno-direct-ivar-access
        -Wno-objc-messaging-id

        # NOTE: Xcode adds include search paths for directories (e.g. DerivedSources)
        # that may not exist on the first build. Keep the warning but prevent -Werror
        # from promoting it to an error.
        -Wno-error=missing-include-dirs
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -Wextra
        -Wcast-align
        -Wcast-qual
        # -Wconversion
        -Wctor-dtor-privacy
        -Wdelete-non-virtual-dtor
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wendif-labels
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-signedness
        -Winit-self
        # -Winline
        -Wlogical-op
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wredundant-decls
        -Wreorder
        -Wshadow
        -Wstrict-null-sentinel
        # -Wsuggest-final-methods
        -Wundef
        -Wunsafe-loop-optimizations
        # -Wuseless-cast
        -Wvariadic-macros
        -Wwrite-strings
        -Wzero-as-null-pointer-constant
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        # NOTE: See https://docs.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default
        /Wall
        /WX
        /wd4061 # NOTE: enumerator 'E::X' in switch of enum 'E' is not explicitly handled by a case label
        /wd4365 # NOTE: signed/unsigned mismatch
        /wd4514 # NOTE: unreferenced inline function has been removed
        /wd4625 # NOTE: copy constructor was implicitly defined as deleted
        /wd4626 # NOTE: assignment operator was implicitly defined as deleted
        /wd4710 # NOTE: function not inlined
        /wd4711 # NOTE: function 'F' selected for automatic inline expansion
        /wd4820 # NOTE: 'N' bytes padding added after data member
        /wd5026 # NOTE: move constructor was implicitly defined as deleted
        /wd5027 # NOTE: move assignment operator was implicitly defined as deleted
        /wd5039 # NOTE: Undefined behavior may occur if this function throws an exception.
        /wd5045 # NOTE: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5219 # NOTE: implicit conversion from 'int' to 'float', possible loss of data
        /std:c++latest
        /permissive-
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
        $<$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>:
            /fsanitize=address # NOTE: Enable address sanitizer
            /Zi # NOTE: Generate debugging information to support address sanitizer
        >
        /arch:AVX # NOTE: Enable AVX for MSVC
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        $<$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>:
            # NOTE: pthreads can only be used in browsers that have SharedArrayBuffer enabled.
            -pthread
        >

        # FIXME: Please fix the following warning and enable the warning option.
        -Wno-unsafe-buffer-usage

        # NOTE: Disable thread safety analysis for Emscripten, because it causes false positives.
        -Wno-thread-safety-negative

        # FIXME: Disable NRVO warning, because it causes false positives in some cases.
        -Wno-nrvo

        # NOTE: Disable the experimental lifetime-safety diagnostics (Clang >= 21).
        # They fire false positives on standard container patterns such as `vector::push_back`.
        $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,21>:
            -Wno-lifetime-safety-cross-tu-suggestions
            -Wno-lifetime-safety-intra-tu-suggestions
        >
        $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,23>:
            -Wno-lifetime-safety-invalidation
        >
    >
)

# NOTE: For dynamic library
# NOTE: see https://cmake.org/cmake/help/v3.9/prop_tgt/FRAMEWORK.html
# set_target_properties(pomdog_static PROPERTIES
#   XCODE_ATTRIBUTE_PRODUCT_NAME "pomdog"
#   XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "net.enginetrouble.pomdog"
#   XCODE_ATTRIBUTE_INFOPLIST_FILE "Info.plist"
#   XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_LIBRARY_DIR)/Frameworks"
#   XCODE_ATTRIBUTE_DYLIB_INSTALL_NAME_BASE "@rpath"
#   XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited);@executable_path/../Frameworks;@loader_path/Frameworks"
# )

set_target_properties(${POMDOG_COMPILE_TARGETS} PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"

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
    XCODE_ATTRIBUTE_CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_LITERAL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_ROOT_CLASS "YES_ERROR"

    # Warning Policies
    XCODE_ATTRIBUTE_GCC_TREAT_WARNINGS_AS_ERRORS "YES"
    XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wall"

    # Metal Compiler
    XCODE_ATTRIBUTE_MTL_ENABLE_DEBUG_INFO[variant=Debug] "INCLUDE_SOURCE"
    XCODE_ATTRIBUTE_MTL_ENABLE_DEBUG_INFO[variant=Release] "NO"
    XCODE_ATTRIBUTE_MTL_FAST_MATH "YES"

    # Static Analyzer
    XCODE_ATTRIBUTE_CLANG_ANALYZER_LOCALIZABILITY_NONLOCALIZED "YES"

    # Symbols
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
    XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden
)

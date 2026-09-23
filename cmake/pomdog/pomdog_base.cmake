add_library(pomdog_base STATIC)
add_library(pomdog::base ALIAS pomdog_base)

target_link_libraries(pomdog_base
    PUBLIC
        fmt_static
        pomdog_sanitizer_settings
        pomdog_lto_settings
    PRIVATE
        pomdog_build_settings
        $<$<PLATFORM_ID:Windows>:
            kernel32.lib
        >
        $<$<PLATFORM_ID:Linux>:
            pthread
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

# NOTE: These definitions affect inline public code and must agree with consumers.
target_include_directories(pomdog_base PUBLIC
    ${POMDOG_DIR}
    ${POMDOG_DIR}/thirdparty
    ${POMDOG_DIR}/thirdparty/fmt/include
)

target_compile_features(pomdog_base PUBLIC
    cxx_std_23
)

target_compile_definitions(pomdog_base PUBLIC
    $<$<CONFIG:Debug>:POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(pomdog_base PUBLIC
    # NOTE: MSVC consumers must use the same static CRT as the engine and its dependencies.
    $<$<CXX_COMPILER_ID:MSVC>:$<IF:$<CONFIG:Debug>,/MTd,/MT>>

    $<$<AND:$<PLATFORM_ID:Emscripten>,$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>>:
        -pthread
    >
)

target_link_options(pomdog_base INTERFACE
    $<$<AND:$<PLATFORM_ID:Emscripten>,$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>>:
        -pthread
    >
)

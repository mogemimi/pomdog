add_library(pomdog_network STATIC)
add_library(pomdog::network ALIAS pomdog_network)

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

source_group(network            REGULAR_EXPRESSION "pomdog/network/*")
source_group(network/emscripten REGULAR_EXPRESSION "pomdog/network/emscripten/*")
source_group(network/mbedtls    REGULAR_EXPRESSION "pomdog/network/mbedtls/*")
source_group(network/native     REGULAR_EXPRESSION "pomdog/network/native/*")
source_group(network/posix      REGULAR_EXPRESSION "pomdog/network/posix/*")
source_group(network/win32      REGULAR_EXPRESSION "pomdog/network/win32/*")

target_sources(pomdog_network PRIVATE
    # NOTE: network
    ${POMDOG_SRC_DIR}/network/address_family.h
    ${POMDOG_SRC_DIR}/network/connect_timeout.cpp
    ${POMDOG_SRC_DIR}/network/connect_timeout.h
    ${POMDOG_SRC_DIR}/network/forward_declarations.h
    ${POMDOG_SRC_DIR}/network/host_port.cpp
    ${POMDOG_SRC_DIR}/network/host_port.h
    ${POMDOG_SRC_DIR}/network/http_client.cpp
    ${POMDOG_SRC_DIR}/network/http_client.h
    ${POMDOG_SRC_DIR}/network/http_method.h
    ${POMDOG_SRC_DIR}/network/http_parser.cpp
    ${POMDOG_SRC_DIR}/network/http_parser.h
    ${POMDOG_SRC_DIR}/network/http_request.h
    ${POMDOG_SRC_DIR}/network/http_request_check.cpp
    ${POMDOG_SRC_DIR}/network/http_request_check.h
    ${POMDOG_SRC_DIR}/network/http_response.h
    ${POMDOG_SRC_DIR}/network/socket_protocol.h
    ${POMDOG_SRC_DIR}/network/stream.cpp
    ${POMDOG_SRC_DIR}/network/stream.h
    ${POMDOG_SRC_DIR}/network/tcp_stream.cpp
    ${POMDOG_SRC_DIR}/network/tcp_stream.h
    ${POMDOG_SRC_DIR}/network/tls_stream.cpp
    ${POMDOG_SRC_DIR}/network/tls_stream.h
    ${POMDOG_SRC_DIR}/network/trust_store.cpp
    ${POMDOG_SRC_DIR}/network/trust_store.h
    ${POMDOG_SRC_DIR}/network/udp_stream.cpp
    ${POMDOG_SRC_DIR}/network/udp_stream.h

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        ${POMDOG_SRC_DIR}/network/connect_worker.h
        ${POMDOG_SRC_DIR}/network/end_point.cpp
        ${POMDOG_SRC_DIR}/network/end_point.h
        ${POMDOG_SRC_DIR}/network/receive_limits.h
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: network/emscripten
        ${POMDOG_SRC_DIR}/network/emscripten/http_client_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/http_client_emscripten.h
    >

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # NOTE: network/mbedtls
        ${POMDOG_SRC_DIR}/network/mbedtls/mbedtls_helper.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/mbedtls_helper.h
        ${POMDOG_SRC_DIR}/network/mbedtls/tls_stream_mbedtls.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/tls_stream_mbedtls.h
        ${POMDOG_SRC_DIR}/network/mbedtls/trust_store_mbedtls.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/trust_store_mbedtls.h
    >

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # NOTE: network/native
        ${POMDOG_SRC_DIR}/network/native/http_client_native.cpp
        ${POMDOG_SRC_DIR}/network/native/http_client_native.h
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
        ${POMDOG_SRC_DIR}/network/win32/socket_helper_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/socket_helper_win32.h
        ${POMDOG_SRC_DIR}/network/win32/tcp_stream_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/tcp_stream_win32.h
        ${POMDOG_SRC_DIR}/network/win32/udp_stream_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/udp_stream_win32.h
        ${POMDOG_SRC_DIR}/network/win32/winsock_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/winsock_win32.h
    >
)

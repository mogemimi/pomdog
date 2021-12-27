source_group(network            REGULAR_EXPRESSION "pomdog/network/*")
source_group(network/mbedtls    REGULAR_EXPRESSION "pomdog/network/mbedtls/*")
source_group(network/posix      REGULAR_EXPRESSION "pomdog/network/posix/*")
source_group(network/win32      REGULAR_EXPRESSION "pomdog/network/win32/*")

target_sources(pomdog_static PRIVATE
    # NOTE: network
    ${POMDOG_SRC_DIR}/network/address_family.h
    ${POMDOG_SRC_DIR}/network/address_parser.cpp
    ${POMDOG_SRC_DIR}/network/address_parser.h
    ${POMDOG_SRC_DIR}/network/array_view.h
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

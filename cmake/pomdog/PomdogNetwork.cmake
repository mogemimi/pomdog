source_group(network            REGULAR_EXPRESSION "pomdog/network/*")
source_group(network/mbedtls    REGULAR_EXPRESSION "pomdog/network/mbedtls/*")
source_group(network/posix      REGULAR_EXPRESSION "pomdog/network/posix/*")
source_group(network/win32      REGULAR_EXPRESSION "pomdog/network/win32/*")

target_sources(pomdog_static PRIVATE
    # NOTE: network
    ${POMDOG_SRC_DIR}/network/address_family.hpp
    ${POMDOG_SRC_DIR}/network/address_parser.cpp
    ${POMDOG_SRC_DIR}/network/address_parser.hpp
    ${POMDOG_SRC_DIR}/network/array_view.hpp
    ${POMDOG_SRC_DIR}/network/forward_declarations.hpp
    ${POMDOG_SRC_DIR}/network/http_client.cpp
    ${POMDOG_SRC_DIR}/network/http_client.hpp
    ${POMDOG_SRC_DIR}/network/http_method.hpp
    ${POMDOG_SRC_DIR}/network/http_parser.cpp
    ${POMDOG_SRC_DIR}/network/http_parser.hpp
    ${POMDOG_SRC_DIR}/network/http_request.cpp
    ${POMDOG_SRC_DIR}/network/http_request.hpp
    ${POMDOG_SRC_DIR}/network/http_response.cpp
    ${POMDOG_SRC_DIR}/network/http_response.hpp
    ${POMDOG_SRC_DIR}/network/io_service.cpp
    ${POMDOG_SRC_DIR}/network/io_service.hpp
    ${POMDOG_SRC_DIR}/network/socket_protocol.hpp
    ${POMDOG_SRC_DIR}/network/tcp_stream.cpp
    ${POMDOG_SRC_DIR}/network/tcp_stream.hpp
    ${POMDOG_SRC_DIR}/network/tls_stream.cpp
    ${POMDOG_SRC_DIR}/network/tls_stream.hpp
    ${POMDOG_SRC_DIR}/network/udp_stream.cpp
    ${POMDOG_SRC_DIR}/network/udp_stream.hpp

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        ${POMDOG_SRC_DIR}/network/end_point.cpp
        ${POMDOG_SRC_DIR}/network/end_point.hpp
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: network/emscripten
        ${POMDOG_SRC_DIR}/network/emscripten/tcp_stream_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/tcp_stream_emscripten.hpp
        ${POMDOG_SRC_DIR}/network/emscripten/tls_stream_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/tls_stream_emscripten.hpp
        ${POMDOG_SRC_DIR}/network/emscripten/udp_stream_emscripten.cpp
        ${POMDOG_SRC_DIR}/network/emscripten/udp_stream_emscripten.hpp
    >

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # NOTE: network/mbedtls
        ${POMDOG_SRC_DIR}/network/mbedtls/certificates.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/certificates.hpp
        ${POMDOG_SRC_DIR}/network/mbedtls/tls_stream_mbedtls.cpp
        ${POMDOG_SRC_DIR}/network/mbedtls/tls_stream_mbedtls.hpp
    >

    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: network/posix
        ${POMDOG_SRC_DIR}/network/posix/socket_helper_posix.cpp
        ${POMDOG_SRC_DIR}/network/posix/socket_helper_posix.hpp
        ${POMDOG_SRC_DIR}/network/posix/tcp_stream_posix.cpp
        ${POMDOG_SRC_DIR}/network/posix/tcp_stream_posix.hpp
        ${POMDOG_SRC_DIR}/network/posix/udp_stream_posix.cpp
        ${POMDOG_SRC_DIR}/network/posix/udp_stream_posix.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: network/win32
        ${POMDOG_SRC_DIR}/network/win32/io_service_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/io_service_win32.hpp
        ${POMDOG_SRC_DIR}/network/win32/socket_helper_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/socket_helper_win32.hpp
        ${POMDOG_SRC_DIR}/network/win32/tcp_stream_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/tcp_stream_win32.hpp
        ${POMDOG_SRC_DIR}/network/win32/udp_stream_win32.cpp
        ${POMDOG_SRC_DIR}/network/win32/udp_stream_win32.hpp
    >
)

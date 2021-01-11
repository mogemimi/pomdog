source_group(Network            REGULAR_EXPRESSION "(include/Pomdog|src)/Network/*")
source_group(Network.MbedTLS    REGULAR_EXPRESSION "(include/Pomdog|src)/Network.MbedTLS/*")
source_group(Network.POSIX      REGULAR_EXPRESSION "(include/Pomdog|src)/Network.POSIX/*")
source_group(Network.Win32      REGULAR_EXPRESSION "(include/Pomdog|src)/Network.Win32/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Network
    ${POMDOG_SRC_DIR}/Network/AddressFamily.hpp
    ${POMDOG_SRC_DIR}/Network/AddressParser.cpp
    ${POMDOG_SRC_DIR}/Network/AddressParser.hpp
    ${POMDOG_INC_DIR}/Network/ArrayView.hpp
    ${POMDOG_INC_DIR}/Network/ForwardDeclarations.hpp
    ${POMDOG_INC_DIR}/Network/HTTPClient.hpp
    ${POMDOG_SRC_DIR}/Network/HTTPClient.cpp
    ${POMDOG_INC_DIR}/Network/HTTPMethod.hpp
    ${POMDOG_SRC_DIR}/Network/HTTPParser.cpp
    ${POMDOG_SRC_DIR}/Network/HTTPParser.hpp
    ${POMDOG_INC_DIR}/Network/HTTPRequest.hpp
    ${POMDOG_SRC_DIR}/Network/HTTPRequest.cpp
    ${POMDOG_INC_DIR}/Network/HTTPResponse.hpp
    ${POMDOG_SRC_DIR}/Network/HTTPResponse.cpp
    ${POMDOG_INC_DIR}/Network/IOService.hpp
    ${POMDOG_SRC_DIR}/Network/IOService.cpp
    ${POMDOG_SRC_DIR}/Network/SocketProtocol.hpp
    ${POMDOG_INC_DIR}/Network/TCPStream.hpp
    ${POMDOG_SRC_DIR}/Network/TCPStream.cpp
    ${POMDOG_INC_DIR}/Network/TLSStream.hpp
    ${POMDOG_SRC_DIR}/Network/TLSStream.cpp
    ${POMDOG_INC_DIR}/Network/UDPStream.hpp
    ${POMDOG_SRC_DIR}/Network/UDPStream.cpp

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        ${POMDOG_SRC_DIR}/Network/EndPoint.cpp
        ${POMDOG_SRC_DIR}/Network/EndPoint.hpp
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: Emscripten
        ${POMDOG_SRC_DIR}/Network.Emscripten/TCPStreamEmscripten.cpp
        ${POMDOG_SRC_DIR}/Network.Emscripten/TCPStreamEmscripten.hpp
        ${POMDOG_SRC_DIR}/Network.Emscripten/TLSStreamEmscripten.cpp
        ${POMDOG_SRC_DIR}/Network.Emscripten/TLSStreamEmscripten.hpp
        ${POMDOG_SRC_DIR}/Network.Emscripten/UDPStreamEmscripten.cpp
        ${POMDOG_SRC_DIR}/Network.Emscripten/UDPStreamEmscripten.hpp
    >

    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # NOTE: Network.MbedTLS
        ${POMDOG_SRC_DIR}/Network.MbedTLS/Certificates.cpp
        ${POMDOG_SRC_DIR}/Network.MbedTLS/Certificates.hpp
        ${POMDOG_SRC_DIR}/Network.MbedTLS/TLSStreamMbedTLS.cpp
        ${POMDOG_SRC_DIR}/Network.MbedTLS/TLSStreamMbedTLS.hpp
    >

    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: Network.POSIX
        ${POMDOG_SRC_DIR}/Network.POSIX/SocketHelperPOSIX.cpp
        ${POMDOG_SRC_DIR}/Network.POSIX/SocketHelperPOSIX.hpp
        ${POMDOG_SRC_DIR}/Network.POSIX/TCPStreamPOSIX.cpp
        ${POMDOG_SRC_DIR}/Network.POSIX/TCPStreamPOSIX.hpp
        ${POMDOG_SRC_DIR}/Network.POSIX/UDPStreamPOSIX.cpp
        ${POMDOG_SRC_DIR}/Network.POSIX/UDPStreamPOSIX.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Network.Win32
        ${POMDOG_SRC_DIR}/Network.Win32/IOServiceWin32.cpp
        ${POMDOG_SRC_DIR}/Network.Win32/IOServiceWin32.hpp
        ${POMDOG_SRC_DIR}/Network.Win32/SocketHelperWin32.cpp
        ${POMDOG_SRC_DIR}/Network.Win32/SocketHelperWin32.hpp
        ${POMDOG_SRC_DIR}/Network.Win32/TCPStreamWin32.cpp
        ${POMDOG_SRC_DIR}/Network.Win32/TCPStreamWin32.hpp
        ${POMDOG_SRC_DIR}/Network.Win32/UDPStreamWin32.cpp
        ${POMDOG_SRC_DIR}/Network.Win32/UDPStreamWin32.hpp
    >
)

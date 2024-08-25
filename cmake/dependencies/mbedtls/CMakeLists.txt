cmake_minimum_required(VERSION 3.27)
project(mbedtls C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../../dependencies")
set(MBEDTLS_DIR "${THIRDPARTY_DIR}/mbedtls")

add_library(mbedtls STATIC
    ${MBEDTLS_DIR}/library/aes.c
    ${MBEDTLS_DIR}/library/aesni.c
    ${MBEDTLS_DIR}/library/aria.c
    ${MBEDTLS_DIR}/library/asn1parse.c
    ${MBEDTLS_DIR}/library/asn1write.c
    ${MBEDTLS_DIR}/library/base64.c
    ${MBEDTLS_DIR}/library/bignum.c
    ${MBEDTLS_DIR}/library/camellia.c
    ${MBEDTLS_DIR}/library/ccm.c
    ${MBEDTLS_DIR}/library/chacha20.c
    ${MBEDTLS_DIR}/library/chachapoly.c
    ${MBEDTLS_DIR}/library/cipher_wrap.c
    ${MBEDTLS_DIR}/library/cipher.c
    ${MBEDTLS_DIR}/library/cmac.c
    ${MBEDTLS_DIR}/library/constant_time.c
    ${MBEDTLS_DIR}/library/ctr_drbg.c
    ${MBEDTLS_DIR}/library/debug.c
    ${MBEDTLS_DIR}/library/des.c
    ${MBEDTLS_DIR}/library/dhm.c
    ${MBEDTLS_DIR}/library/ecdh.c
    ${MBEDTLS_DIR}/library/ecdsa.c
    ${MBEDTLS_DIR}/library/ecjpake.c
    ${MBEDTLS_DIR}/library/ecp_curves.c
    ${MBEDTLS_DIR}/library/ecp.c
    ${MBEDTLS_DIR}/library/entropy_poll.c
    ${MBEDTLS_DIR}/library/entropy.c
    ${MBEDTLS_DIR}/library/gcm.c
    ${MBEDTLS_DIR}/library/hkdf.c
    ${MBEDTLS_DIR}/library/hmac_drbg.c
    ${MBEDTLS_DIR}/library/md.c
    ${MBEDTLS_DIR}/library/md5.c
    ${MBEDTLS_DIR}/library/memory_buffer_alloc.c
    ${MBEDTLS_DIR}/library/mps_reader.c
    ${MBEDTLS_DIR}/library/mps_trace.c
    ${MBEDTLS_DIR}/library/net_sockets.c
    ${MBEDTLS_DIR}/library/nist_kw.c
    ${MBEDTLS_DIR}/library/oid.c
    ${MBEDTLS_DIR}/library/padlock.c
    ${MBEDTLS_DIR}/library/pem.c
    ${MBEDTLS_DIR}/library/pk_wrap.c
    ${MBEDTLS_DIR}/library/pk.c
    ${MBEDTLS_DIR}/library/pkcs5.c
    ${MBEDTLS_DIR}/library/pkcs12.c
    ${MBEDTLS_DIR}/library/pkparse.c
    ${MBEDTLS_DIR}/library/pkwrite.c
    ${MBEDTLS_DIR}/library/platform_util.c
    ${MBEDTLS_DIR}/library/platform.c
    ${MBEDTLS_DIR}/library/poly1305.c
    ${MBEDTLS_DIR}/library/psa_crypto_aead.c
    ${MBEDTLS_DIR}/library/psa_crypto_cipher.c
    ${MBEDTLS_DIR}/library/psa_crypto_client.c
    ${MBEDTLS_DIR}/library/psa_crypto_driver_wrappers.c
    ${MBEDTLS_DIR}/library/psa_crypto_ecp.c
    ${MBEDTLS_DIR}/library/psa_crypto_hash.c
    ${MBEDTLS_DIR}/library/psa_crypto_mac.c
    ${MBEDTLS_DIR}/library/psa_crypto_rsa.c
    ${MBEDTLS_DIR}/library/psa_crypto_se.c
    ${MBEDTLS_DIR}/library/psa_crypto_slot_management.c
    ${MBEDTLS_DIR}/library/psa_crypto_storage.c
    ${MBEDTLS_DIR}/library/psa_crypto.c
    ${MBEDTLS_DIR}/library/psa_its_file.c
    ${MBEDTLS_DIR}/library/ripemd160.c
    ${MBEDTLS_DIR}/library/rsa_alt_helpers.c
    ${MBEDTLS_DIR}/library/rsa.c
    ${MBEDTLS_DIR}/library/sha1.c
    ${MBEDTLS_DIR}/library/sha256.c
    ${MBEDTLS_DIR}/library/sha512.c
    ${MBEDTLS_DIR}/library/ssl_cache.c
    ${MBEDTLS_DIR}/library/ssl_ciphersuites.c
    ${MBEDTLS_DIR}/library/ssl_cli.c
    ${MBEDTLS_DIR}/library/ssl_cookie.c
    ${MBEDTLS_DIR}/library/ssl_msg.c
    ${MBEDTLS_DIR}/library/ssl_srv.c
    ${MBEDTLS_DIR}/library/ssl_ticket.c
    ${MBEDTLS_DIR}/library/ssl_tls.c
    ${MBEDTLS_DIR}/library/ssl_tls13_client.c
    ${MBEDTLS_DIR}/library/ssl_tls13_generic.c
    ${MBEDTLS_DIR}/library/ssl_tls13_keys.c
    ${MBEDTLS_DIR}/library/ssl_tls13_server.c
    ${MBEDTLS_DIR}/library/threading.c
    ${MBEDTLS_DIR}/library/timing.c
    ${MBEDTLS_DIR}/library/version.c
    ${MBEDTLS_DIR}/library/x509_create.c
    ${MBEDTLS_DIR}/library/x509_crl.c
    ${MBEDTLS_DIR}/library/x509_crt.c
    ${MBEDTLS_DIR}/library/x509_csr.c
    ${MBEDTLS_DIR}/library/x509.c
    ${MBEDTLS_DIR}/library/x509write_crt.c
    ${MBEDTLS_DIR}/library/x509write_csr.c
    ${THIRDPARTY_DIR}/vendor/mbedtls/error.c
)

target_include_directories(mbedtls PRIVATE
    ${MBEDTLS_DIR}/include
    ${MBEDTLS_DIR}/library
    ${MBEDTLS_DIR}/3rdparty/everest/include
)

target_compile_definitions(mbedtls PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _WINDOWS
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_WARNINGS
    >
)

target_compile_options(mbedtls PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wno-implicit-function-declaration
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /nologo
        /W3
        /O2
        /Ob1
        /GF
        /Gy
        /GS-
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)

set_target_properties(mbedtls PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2a"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK "YES"

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.14"
    XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)

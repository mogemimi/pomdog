cmake_minimum_required(VERSION 3.16)
project(mbedtls C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(POMDOG_DIR "../../..")
set(THIRDPARTY_DIR "../../../dependencies")

add_library(mbedtls STATIC
  ${THIRDPARTY_DIR}/mbedtls/library/aes.c
  ${THIRDPARTY_DIR}/mbedtls/library/aesni.c
  ${THIRDPARTY_DIR}/mbedtls/library/arc4.c
  ${THIRDPARTY_DIR}/mbedtls/library/aria.c
  ${THIRDPARTY_DIR}/mbedtls/library/asn1parse.c
  ${THIRDPARTY_DIR}/mbedtls/library/asn1write.c
  ${THIRDPARTY_DIR}/mbedtls/library/base64.c
  ${THIRDPARTY_DIR}/mbedtls/library/bignum.c
  ${THIRDPARTY_DIR}/mbedtls/library/blowfish.c
  ${THIRDPARTY_DIR}/mbedtls/library/camellia.c
  ${THIRDPARTY_DIR}/mbedtls/library/ccm.c
  ${THIRDPARTY_DIR}/mbedtls/library/certs.c
  ${THIRDPARTY_DIR}/mbedtls/library/chacha20.c
  ${THIRDPARTY_DIR}/mbedtls/library/chachapoly.c
  ${THIRDPARTY_DIR}/mbedtls/library/cipher_wrap.c
  ${THIRDPARTY_DIR}/mbedtls/library/cipher.c
  ${THIRDPARTY_DIR}/mbedtls/library/cmac.c
  ${THIRDPARTY_DIR}/mbedtls/library/ctr_drbg.c
  ${THIRDPARTY_DIR}/mbedtls/library/debug.c
  ${THIRDPARTY_DIR}/mbedtls/library/des.c
  ${THIRDPARTY_DIR}/mbedtls/library/dhm.c
  ${THIRDPARTY_DIR}/mbedtls/library/ecdh.c
  ${THIRDPARTY_DIR}/mbedtls/library/ecdsa.c
  ${THIRDPARTY_DIR}/mbedtls/library/ecjpake.c
  ${THIRDPARTY_DIR}/mbedtls/library/ecp_curves.c
  ${THIRDPARTY_DIR}/mbedtls/library/ecp.c
  ${THIRDPARTY_DIR}/mbedtls/library/entropy_poll.c
  ${THIRDPARTY_DIR}/mbedtls/library/entropy.c
  ${THIRDPARTY_DIR}/mbedtls/library/error.c
  ${THIRDPARTY_DIR}/mbedtls/library/gcm.c
  ${THIRDPARTY_DIR}/mbedtls/library/havege.c
  ${THIRDPARTY_DIR}/mbedtls/library/hkdf.c
  ${THIRDPARTY_DIR}/mbedtls/library/hmac_drbg.c
  ${THIRDPARTY_DIR}/mbedtls/library/Makefile
  ${THIRDPARTY_DIR}/mbedtls/library/md_wrap.c
  ${THIRDPARTY_DIR}/mbedtls/library/md.c
  ${THIRDPARTY_DIR}/mbedtls/library/md2.c
  ${THIRDPARTY_DIR}/mbedtls/library/md4.c
  ${THIRDPARTY_DIR}/mbedtls/library/md5.c
  ${THIRDPARTY_DIR}/mbedtls/library/memory_buffer_alloc.c
  ${THIRDPARTY_DIR}/mbedtls/library/net_sockets.c
  ${THIRDPARTY_DIR}/mbedtls/library/nist_kw.c
  ${THIRDPARTY_DIR}/mbedtls/library/oid.c
  ${THIRDPARTY_DIR}/mbedtls/library/padlock.c
  ${THIRDPARTY_DIR}/mbedtls/library/pem.c
  ${THIRDPARTY_DIR}/mbedtls/library/pk_wrap.c
  ${THIRDPARTY_DIR}/mbedtls/library/pk.c
  ${THIRDPARTY_DIR}/mbedtls/library/pkcs5.c
  ${THIRDPARTY_DIR}/mbedtls/library/pkcs11.c
  ${THIRDPARTY_DIR}/mbedtls/library/pkcs12.c
  ${THIRDPARTY_DIR}/mbedtls/library/pkparse.c
  ${THIRDPARTY_DIR}/mbedtls/library/pkwrite.c
  ${THIRDPARTY_DIR}/mbedtls/library/platform_util.c
  ${THIRDPARTY_DIR}/mbedtls/library/platform.c
  ${THIRDPARTY_DIR}/mbedtls/library/poly1305.c
  ${THIRDPARTY_DIR}/mbedtls/library/ripemd160.c
  ${THIRDPARTY_DIR}/mbedtls/library/rsa_internal.c
  ${THIRDPARTY_DIR}/mbedtls/library/rsa.c
  ${THIRDPARTY_DIR}/mbedtls/library/sha1.c
  ${THIRDPARTY_DIR}/mbedtls/library/sha256.c
  ${THIRDPARTY_DIR}/mbedtls/library/sha512.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_cache.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_ciphersuites.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_cli.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_cookie.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_srv.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_ticket.c
  ${THIRDPARTY_DIR}/mbedtls/library/ssl_tls.c
  ${THIRDPARTY_DIR}/mbedtls/library/threading.c
  ${THIRDPARTY_DIR}/mbedtls/library/timing.c
  ${THIRDPARTY_DIR}/mbedtls/library/version_features.c
  ${THIRDPARTY_DIR}/mbedtls/library/version.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509_create.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509_crl.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509_crt.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509_csr.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509write_crt.c
  ${THIRDPARTY_DIR}/mbedtls/library/x509write_csr.c
  ${THIRDPARTY_DIR}/mbedtls/library/xtea.c
)

target_include_directories(mbedtls PRIVATE
  ${THIRDPARTY_DIR}/mbedtls/include
  ${THIRDPARTY_DIR}/mbedtls/crypto/include
)

target_compile_definitions(mbedtls PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG>
  $<$<CONFIG:RELEASE>:NDEBUG>
  $<$<PLATFORM_ID:Windows>:
    WIN32
    _WINDOWS
    _CRT_NONSTDC_NO_DEPRECATE
    _CRT_SECURE_NO_DEPRECATE
    _CRT_NONSTDC_NO_WARNINGS
  >
)

target_compile_options(mbedtls PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wno-implicit-function-declaration
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  $<$<CXX_COMPILER_ID:MSVC>:
    /nologo
    /W3
    /O2
    /Ob1
    /GF
    /Gy
    /GS-
    $<$<CONFIG:DEBUG>:/MTd>
    $<$<CONFIG:RELEASE>:/MT>
  >
)

if(UNIX)
  set_target_properties(mbedtls PROPERTIES OUTPUT_NAME z)
endif()

set_target_properties(mbedtls PROPERTIES
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
  XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
  XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

  XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
  XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
  XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK "YES"

  XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.12"
  XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)

add_subdirectory("${POMDOG_DIR}/build/dependencies/mbedtls-crypto" "${CMAKE_CURRENT_BINARY_DIR}/mbedtls-crypto_build")
target_link_libraries(mbedtls INTERFACE mbedtls-crypto)

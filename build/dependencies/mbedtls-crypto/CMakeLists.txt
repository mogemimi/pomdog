cmake_minimum_required(VERSION 3.16)
project(mbedtls-crypto C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../../dependencies")

add_library(mbedtls-crypto STATIC
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/aes.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/aesni.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/arc4.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/aria.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/asn1parse.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/asn1write.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/base64.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/bignum.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/blowfish.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/camellia.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ccm.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/chacha20.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/chachapoly.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/cipher_wrap.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/cipher.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/cmac.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ctr_drbg.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/des.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/dhm.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ecdh.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ecdsa.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ecjpake.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ecp_curves.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ecp.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/entropy_poll.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/entropy.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/error.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/gcm.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/havege.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/hkdf.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/hmac_drbg.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/Makefile
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/md_wrap.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/md.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/md2.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/md4.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/md5.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/memory_buffer_alloc.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/nist_kw.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/oid.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/padlock.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pem.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pk_wrap.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pk.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pkcs5.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pkcs12.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pkparse.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/pkwrite.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/platform_util.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/platform.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/poly1305.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_core.h
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_invasive.h
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_its.h
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_service_integration.h
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_slot_management.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_slot_management.h
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_storage.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto_storage.h
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_crypto.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/psa_its_file.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/ripemd160.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/rsa_internal.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/rsa.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/sha1.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/sha256.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/sha512.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/threading.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/timing.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/version_features.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/version.c
  ${THIRDPARTY_DIR}/mbedtls/crypto/library/xtea.c
)

target_include_directories(mbedtls-crypto PRIVATE
  ${THIRDPARTY_DIR}/mbedtls/crypto/include
)

target_compile_definitions(mbedtls-crypto PRIVATE
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

target_compile_options(mbedtls-crypto PRIVATE
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
  set_target_properties(mbedtls-crypto PROPERTIES OUTPUT_NAME z)
endif()

set_target_properties(mbedtls-crypto PROPERTIES
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

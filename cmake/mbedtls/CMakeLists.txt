cmake_minimum_required(VERSION 3.31)
project(mbedtls C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

cmake_path(SET THIRDPARTY_DIR    NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty")
cmake_path(SET MBEDTLS_DIR       NORMALIZE "${THIRDPARTY_DIR}/mbedtls")
cmake_path(SET TF_PSA_CRYPTO_DIR NORMALIZE "${MBEDTLS_DIR}/tf-psa-crypto")

# NOTE: Upstream generates some sources at build time with Perl and Python.
# tools/script/generate_mbedtls_sources.sh writes them to this directory.
cmake_path(SET MBEDTLS_GENERATED_DIR NORMALIZE "${THIRDPARTY_DIR}/vendor/mbedtls")

# NOTE: Mbed TLS (X.509 and TLS) and TF-PSA-Crypto build into one library.
add_library(mbedtls STATIC
    # NOTE: TF-PSA-Crypto
    ${TF_PSA_CRYPTO_DIR}/core/psa_crypto.c
    ${TF_PSA_CRYPTO_DIR}/core/psa_crypto_client.c
    ${TF_PSA_CRYPTO_DIR}/core/psa_crypto_random.c
    ${TF_PSA_CRYPTO_DIR}/core/psa_crypto_slot_management.c
    ${TF_PSA_CRYPTO_DIR}/core/psa_crypto_storage.c
    ${TF_PSA_CRYPTO_DIR}/core/psa_its_file.c
    ${TF_PSA_CRYPTO_DIR}/core/psa_util.c
    ${TF_PSA_CRYPTO_DIR}/core/tf_psa_crypto_config.c
    ${TF_PSA_CRYPTO_DIR}/core/tf_psa_crypto_version.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/aes.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/aesce.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/aesni.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/aria.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/bignum.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/bignum_core.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/bignum_mod.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/bignum_mod_raw.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/block_cipher.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/camellia.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ccm.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/chacha20.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/chacha20_neon.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/chachapoly.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/cipher.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/cipher_wrap.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/cmac.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ctr_drbg.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ecdsa.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ecjpake.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ecp.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ecp_curves.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/entropy.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/entropy_poll.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/gcm.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/hmac_drbg.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/md5.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/poly1305.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_aead.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_cipher.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_ecp.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_ffdh.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_hash.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_mac.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_pake.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_rsa.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_crypto_xof.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/psa_util_internal.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/ripemd160.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/rsa.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/rsa_alt_helpers.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/sha1.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/sha256.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/sha3.c
    ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src/sha512.c
    ${TF_PSA_CRYPTO_DIR}/drivers/everest/library/Hacl_Curve25519_joined.c
    ${TF_PSA_CRYPTO_DIR}/drivers/everest/library/x25519.c
    ${TF_PSA_CRYPTO_DIR}/drivers/p256-m/p256-m/p256-m.c
    ${TF_PSA_CRYPTO_DIR}/drivers/p256-m/p256-m_driver_entrypoints.c
    ${TF_PSA_CRYPTO_DIR}/drivers/pqcp/src/pqcp_buffer_alloc.c
    ${TF_PSA_CRYPTO_DIR}/drivers/pqcp/src/psa_crypto_mldsa.c
    ${TF_PSA_CRYPTO_DIR}/drivers/pqcp/src/wrap_mldsa_native.c
    ${TF_PSA_CRYPTO_DIR}/extras/lmots.c
    ${TF_PSA_CRYPTO_DIR}/extras/lms.c
    ${TF_PSA_CRYPTO_DIR}/extras/md.c
    ${TF_PSA_CRYPTO_DIR}/extras/nist_kw.c
    ${TF_PSA_CRYPTO_DIR}/extras/pk.c
    ${TF_PSA_CRYPTO_DIR}/extras/pk_ecc.c
    ${TF_PSA_CRYPTO_DIR}/extras/pk_rsa.c
    ${TF_PSA_CRYPTO_DIR}/extras/pk_wrap.c
    ${TF_PSA_CRYPTO_DIR}/extras/pkparse.c
    ${TF_PSA_CRYPTO_DIR}/extras/pkwrite.c
    ${TF_PSA_CRYPTO_DIR}/platform/memory_buffer_alloc.c
    ${TF_PSA_CRYPTO_DIR}/platform/platform.c
    ${TF_PSA_CRYPTO_DIR}/platform/platform_util.c
    ${TF_PSA_CRYPTO_DIR}/platform/threading.c
    ${TF_PSA_CRYPTO_DIR}/utilities/asn1parse.c
    ${TF_PSA_CRYPTO_DIR}/utilities/asn1write.c
    ${TF_PSA_CRYPTO_DIR}/utilities/base64.c
    ${TF_PSA_CRYPTO_DIR}/utilities/constant_time.c
    ${TF_PSA_CRYPTO_DIR}/utilities/oid.c
    ${TF_PSA_CRYPTO_DIR}/utilities/pem.c
    ${TF_PSA_CRYPTO_DIR}/utilities/pkcs5.c
    ${MBEDTLS_GENERATED_DIR}/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c

    # NOTE: Mbed TLS
    ${MBEDTLS_DIR}/library/debug.c
    ${MBEDTLS_DIR}/library/mbedtls_config.c
    ${MBEDTLS_DIR}/library/mps_reader.c
    ${MBEDTLS_DIR}/library/mps_trace.c
    ${MBEDTLS_DIR}/library/net_sockets.c
    ${MBEDTLS_DIR}/library/pkcs7.c
    ${MBEDTLS_DIR}/library/ssl_cache.c
    ${MBEDTLS_DIR}/library/ssl_ciphersuites.c
    ${MBEDTLS_DIR}/library/ssl_client.c
    ${MBEDTLS_DIR}/library/ssl_cookie.c
    ${MBEDTLS_DIR}/library/ssl_msg.c
    ${MBEDTLS_DIR}/library/ssl_ticket.c
    ${MBEDTLS_DIR}/library/ssl_tls.c
    ${MBEDTLS_DIR}/library/ssl_tls12_client.c
    ${MBEDTLS_DIR}/library/ssl_tls12_server.c
    ${MBEDTLS_DIR}/library/ssl_tls13_client.c
    ${MBEDTLS_DIR}/library/ssl_tls13_generic.c
    ${MBEDTLS_DIR}/library/ssl_tls13_keys.c
    ${MBEDTLS_DIR}/library/ssl_tls13_server.c
    ${MBEDTLS_DIR}/library/timing.c
    ${MBEDTLS_DIR}/library/version.c
    ${MBEDTLS_DIR}/library/x509.c
    ${MBEDTLS_DIR}/library/x509_create.c
    ${MBEDTLS_DIR}/library/x509_crl.c
    ${MBEDTLS_DIR}/library/x509_crt.c
    ${MBEDTLS_DIR}/library/x509_csr.c
    ${MBEDTLS_DIR}/library/x509_oid.c
    ${MBEDTLS_DIR}/library/x509write.c
    ${MBEDTLS_DIR}/library/x509write_crt.c
    ${MBEDTLS_DIR}/library/x509write_csr.c
    ${MBEDTLS_GENERATED_DIR}/library/error.c
    ${MBEDTLS_GENERATED_DIR}/library/ssl_debug_helpers_generated.c
    ${MBEDTLS_GENERATED_DIR}/library/version_features.c
)

target_include_directories(mbedtls
    PUBLIC
        ${MBEDTLS_DIR}/include
        ${TF_PSA_CRYPTO_DIR}/include
        ${TF_PSA_CRYPTO_DIR}/drivers/builtin/include
        ${TF_PSA_CRYPTO_DIR}/drivers/everest/include
        ${TF_PSA_CRYPTO_DIR}/drivers/pqcp/include
    PRIVATE
        ${MBEDTLS_DIR}/library
        ${MBEDTLS_GENERATED_DIR}/library
        ${TF_PSA_CRYPTO_DIR}/core
        ${MBEDTLS_GENERATED_DIR}/tf-psa-crypto/core
        ${TF_PSA_CRYPTO_DIR}/dispatch
        ${TF_PSA_CRYPTO_DIR}/extras
        ${TF_PSA_CRYPTO_DIR}/platform
        ${TF_PSA_CRYPTO_DIR}/utilities
        ${TF_PSA_CRYPTO_DIR}/drivers/builtin/src
        ${TF_PSA_CRYPTO_DIR}/drivers/everest/include/tf-psa-crypto/private/everest
        ${TF_PSA_CRYPTO_DIR}/drivers/everest/include/tf-psa-crypto/private/everest/kremlib
        ${TF_PSA_CRYPTO_DIR}/drivers/p256-m
        ${TF_PSA_CRYPTO_DIR}/drivers/p256-m/p256-m
        ${TF_PSA_CRYPTO_DIR}/drivers/pqcp/src
        ${TF_PSA_CRYPTO_DIR}/drivers/pqcp/mldsa-native/mldsa
)

target_link_libraries(mbedtls
    PRIVATE
        # NOTE: The PSA random generator uses BCryptGenRandom on Windows.
        $<$<PLATFORM_ID:Windows>:bcrypt>
)

target_compile_definitions(mbedtls PRIVATE
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
    # NOTE: Clang, Emscripten, Apple Clang, clang-cl, and GCC.
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wno-implicit-function-declaration
    >
    # NOTE: GNU-style frontend (Clang, Emscripten, Apple Clang, and GCC; excludes clang-cl).
    $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    # NOTE: MSVC-style frontend (MSVC and clang-cl).
    $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
        /nologo
        /W3
        /O2
        /Ob1
        /GF
        /Gy
        /GS-
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)

set_target_properties(mbedtls PROPERTIES
    C_STANDARD 99

    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK "YES"

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"
    XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)

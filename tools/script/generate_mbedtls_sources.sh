#!/usr/bin/env bash
# Writes the Mbed TLS and TF-PSA-Crypto sources that upstream generates at build
# time to thirdparty/vendor/mbedtls, mirroring their upstream paths. Run it from
# the repository root after updating thirdparty/mbedtls. Requires Perl and
# Python 3 with jinja2.
set -euo pipefail

ROOT_DIR="$PWD"
MBEDTLS_DIR="$ROOT_DIR/thirdparty/mbedtls"
OUTPUT_DIR="$ROOT_DIR/thirdparty/vendor/mbedtls"
PYTHON="${PYTHON:-$(command -v python3 || command -v python)}"

mkdir -p "$OUTPUT_DIR/library" "$OUTPUT_DIR/tf-psa-crypto/core"

cd "$MBEDTLS_DIR"
perl scripts/generate_errors.pl \
    tf-psa-crypto/drivers/builtin/include/mbedtls \
    include/mbedtls \
    scripts/data_files \
    "$OUTPUT_DIR/library/error.c"
perl scripts/generate_features.pl \
    include/mbedtls \
    scripts/data_files \
    "$OUTPUT_DIR/library/version_features.c"
"$PYTHON" framework/scripts/generate_ssl_debug_helpers.py "$OUTPUT_DIR/library"
"$PYTHON" scripts/generate_config_checks.py "$OUTPUT_DIR/library"

cd "$MBEDTLS_DIR/tf-psa-crypto"
"$PYTHON" scripts/generate_driver_wrappers.py "$OUTPUT_DIR/tf-psa-crypto/core"
"$PYTHON" scripts/generate_config_checks.py "$OUTPUT_DIR/tf-psa-crypto/core"

# NOTE: Python writes CRLF on Windows. Keep LF so the output does not depend on
# the host.
find "$OUTPUT_DIR" -type f -exec perl -pi -e 's/\r$//' {} +

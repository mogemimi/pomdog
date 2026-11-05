# pem-compact

A tool that checks the syntax of a PEM file of certificates, such as Pomdog's
root certificates, and writes the certificates without the comments and blank
lines between them.

## Background

`assets/certificates/root_certificates.pem` describes each root with comment
lines, such as its issuer and fingerprints, which help a reviewer but which the
runtime does not read. `pem-compact` drops them from the file that the asset
build ships. It also fails the build when the file is not valid PEM, so that a
broken edit does not reach the trust store, which would reject it only at run
time.

## How it works

1. Read the input line by line. CRLF line ends and trailing spaces are
   accepted.
2. Outside the blocks, accept only blank lines and lines that start with `#`.
3. Accept only `-----BEGIN CERTIFICATE-----` blocks, each closed by
   `-----END CERTIFICATE-----`, without headers or blank lines inside.
4. Decode the Base64 of each block strictly, and fail on invalid characters,
   wrong padding, or an empty block. The tool does not check whether a block
   holds a valid certificate.
5. Write each block again with the Base64 in lines of 64 characters
   (RFC 7468), with LF line ends and nothing between the blocks.

## Installation

```sh
cd tools/cmd/pem-compact
go build -o ../../../build/tools/pem-compact
```

## Usage

```sh
pem-compact -i <input> -o <output>
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input PEM file |
| `-o <path>` | Output PEM file |

### Example

```sh
./build/tools/pem-compact \
    -i ./assets/certificates/root_certificates.pem \
    -o ./build/content/certificates/root_certificates.pem
```

## Build pipeline integration

`asset-convert-ninja-gen` generates a Ninja build rule for each `[[pem]]` entry
of a recipe. `assets/assetconvert.toml` compacts the root certificates:

```toml
[[pem]]
in = "certificates/root_certificates.pem"
out = "certificates/root_certificates.pem"
```

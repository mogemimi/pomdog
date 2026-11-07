# distrusted-keys-gen

A tool that converts a PEM file of distrusted certificates into a `.dkeys`
file, which lists the SHA-256 hashes of their public keys for
`network::createTrustStore()`.

## Background

`assets/certificates/distrusted_certificates.pem` lists certificate authorities
that TLS connections must reject even when their chain reaches a trusted root,
with comments that cite each incident. The runtime compares public keys, not
certificates, so that one entry also rejects every cross-signed copy of the
authority. `distrusted-keys-gen` computes the hashes at build time, so the
runtime parses no PEM and no certificate for the list.

## How it works

1. Read the input line by line. Outside the blocks, accept only blank lines and
   lines that start with `#`.
2. Decode each `CERTIFICATE` block, and parse it as an X.509 certificate. The
   tool does not check whether a certificate is valid or expired, since a
   distrusted one often is not.
3. Hash the SubjectPublicKeyInfo of each certificate with SHA-256, as the
   certificate holds it. A re-encoded key, such as the output of
   `openssl pkey -pubin -outform DER`, can differ.
4. Sort the hashes and drop duplicates, which certificates of the same key
   produce.
5. Write them as one byte vector of the `DistrustedKeys` table in
   `schemas/distrusted_keys.fbs`, 32 bytes per hash.

## Installation

```sh
cd tools/cmd/distrusted-keys-gen
go build -o ../../../build/tools/distrusted-keys-gen
```

The tool reads the Go code that bootstrap generates from the schemas into
`build/schemas-go`.

## Usage

```sh
distrusted-keys-gen -i <input> -o <output>
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input PEM file of distrusted certificates |
| `-o <path>` | Output `.dkeys` file |

### Example

```sh
./build/tools/distrusted-keys-gen \
    -i ./assets/certificates/distrusted_certificates.pem \
    -o ./build/content/certificates/distrusted_keys.dkeys
```

## Build pipeline integration

`asset-convert-ninja-gen` generates a Ninja build rule for each
`[[distrusted_keys]]` entry of a recipe. `assets/assetconvert.toml` converts the
distrusted certificates:

```toml
[[distrusted_keys]]
in = "certificates/distrusted_certificates.pem"
out = "certificates/distrusted_keys.dkeys"
```

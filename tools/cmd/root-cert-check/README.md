# root-cert-check

A tool that checks a PEM file of root certificates, such as Pomdog's
`assets/certificates/root_certificates.pem`, against Mozilla's root store.

## Background

Pomdog has no built-in root certificates, and it does not check revocation, so
a root stays trusted until the application ships a set without it. Browsers
follow Mozilla's and Chrome's root programs, which drop roots over time, such as
those of a certificate authority that they distrust. `root-cert-check` finds the
roots of a curated set that Mozilla no longer trusts, so that the set follows
the browsers.

The tool never touches the network. The developer downloads Mozilla's roots,
such as curl's `cacert.pem`, which curl extracts from Mozilla's root store
with the roots trusted for TLS only.

## How it works

1. Read the certificate blocks of the roots file, and the
   `# SHA256 Fingerprint:` comment above each block.
2. Read the certificates of the Mozilla bundle.
3. For each root, report an error when:
   - Mozilla does not hold it.
   - Mozilla holds another edition of it: a certificate with the same subject
     and public key, but other bytes. Replace the root with Mozilla's edition.
   - Its fingerprint comment is missing or does not match the block.
   - It has expired.
4. Report a warning for a root that expires within `-warn-days` days.
5. Print a summary, and exit with 1 when there are errors.

The tool compares certificates only. It does not verify them, and it cannot see
intermediate authorities, which servers send during the handshake.

## Installation

```sh
cd tools/cmd/root-cert-check
go build -o ../../../build/tools/root-cert-check
```

## Usage

```sh
root-cert-check -roots <path> -mozilla <path> [-warn-days <days>]
```

### Options

| Option | Description |
|--------|-------------|
| `-roots <path>` | Root certificates to check, in PEM with comments |
| `-mozilla <path>` | Mozilla's root certificates in PEM, such as curl's `cacert.pem` |
| `-warn-days <days>` | Warn about a root that expires within this many days (default: 365) |

### Example

```sh
curl -o ./build/cacert.pem https://curl.se/ca/cacert.pem
./build/tools/root-cert-check \
    -roots ./assets/certificates/root_certificates.pem \
    -mozilla ./build/cacert.pem
```

When every root passes, the tool prints only the summary:

```
checked 20 roots against 118 Mozilla roots: 0 errors, 0 warnings
```

Each problem names the line of its block and the subject of the root:

```
./old_roots.pem:80: error: CN=AAA Certificate Services,O=Comodo CA Limited,L=Salford,ST=Greater Manchester,C=GB: Mozilla does not trust this root
```

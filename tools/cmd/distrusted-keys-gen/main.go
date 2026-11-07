// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"crypto/sha256"
	"crypto/x509"
	"encoding/pem"
	"errors"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"slices"
	"strings"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
)

const (
	beginLine = "-----BEGIN CERTIFICATE-----"
	endLine   = "-----END CERTIFICATE-----"
)

func main() {
	inFile := ""
	outFile := ""
	flag.StringVar(&inFile, "i", "", "input PEM file of distrusted certificates")
	flag.StringVar(&outFile, "o", "", "output distrusted keys file (*.dkeys)")
	flag.Parse()

	if inFile == "" {
		log.Fatal("-i is required")
	}
	if outFile == "" {
		log.Fatal("-o is required")
	}

	if err := run(inFile, outFile); err != nil {
		log.Fatal(err)
	}
}

func run(inFile, outFile string) error {
	input, err := os.ReadFile(inFile)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	certificates, err := readCertificates(string(input))
	if err != nil {
		return fmt.Errorf("%s: %w", inFile, err)
	}
	hashes, err := keyHashes(certificates)
	if err != nil {
		return fmt.Errorf("%s: %w", inFile, err)
	}

	builder := flatbuffers.NewBuilder(0)
	keys := &schemas.DistrustedKeysT{KeyHashes: bytes.Join(hashes, nil)}
	builder.Finish(keys.Pack(builder))

	if err := os.MkdirAll(filepath.Dir(outFile), os.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(outFile, builder.FinishedBytes(), 0o644); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}

// certificate is a block of the input, and the line where it starts.
type certificate struct {
	line int
	der  []byte
}

// readCertificates returns the CERTIFICATE blocks of text. Outside the
// blocks, text may hold only blank lines and lines that start with "#", as in
// the root certificates.
func readCertificates(text string) ([]certificate, error) {
	var certificates []certificate
	var block []string
	blockStart := 0

	for i, raw := range strings.Split(text, "\n") {
		number := i + 1
		line := strings.TrimRight(raw, " \t\r")

		if block == nil {
			switch {
			case (line == "") || strings.HasPrefix(line, "#"):
			case line == beginLine:
				block = []string{line}
				blockStart = number
			default:
				return nil, fmt.Errorf("line %d: unexpected text outside a %s block: %q", number, beginLine, line)
			}
			continue
		}

		block = append(block, line)
		if line != endLine {
			continue
		}
		decoded, rest := pem.Decode([]byte(strings.Join(block, "\n") + "\n"))
		if (decoded == nil) || (len(bytes.TrimSpace(rest)) != 0) {
			return nil, fmt.Errorf("line %d: the block is not valid PEM", blockStart)
		}
		certificates = append(certificates, certificate{line: blockStart, der: decoded.Bytes})
		block = nil
	}

	if block != nil {
		return nil, fmt.Errorf("line %d: the block has no %q line", blockStart, endLine)
	}
	if len(certificates) == 0 {
		return nil, errors.New("no " + beginLine + " block")
	}
	return certificates, nil
}

// keyHashes returns the SHA-256 hash of the SubjectPublicKeyInfo of each
// certificate, as the certificate holds it, sorted and without duplicates. It
// does not check whether the certificates are valid, since distrusted ones
// often are not.
func keyHashes(certificates []certificate) ([][]byte, error) {
	var hashes [][]byte
	for _, c := range certificates {
		cert, err := x509.ParseCertificate(c.der)
		if err != nil {
			return nil, fmt.Errorf("line %d: cannot parse the certificate: %w", c.line, err)
		}
		hash := sha256.Sum256(cert.RawSubjectPublicKeyInfo)
		hashes = append(hashes, hash[:])
	}

	// NOTE: Certificates that share a key, such as an authority and its
	// cross-signed copy, need one entry.
	slices.SortFunc(hashes, bytes.Compare)
	return slices.CompactFunc(hashes, bytes.Equal), nil
}

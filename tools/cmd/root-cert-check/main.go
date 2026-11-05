// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"crypto/sha256"
	"crypto/x509"
	"encoding/hex"
	"encoding/pem"
	"errors"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
	"time"
)

const (
	beginLine         = "-----BEGIN CERTIFICATE-----"
	endLine           = "-----END CERTIFICATE-----"
	fingerprintPrefix = "# SHA256 Fingerprint:"
)

func main() {
	rootsFile := ""
	mozillaFile := ""
	warnDays := 0
	flag.StringVar(&rootsFile, "roots", "", "root certificates to check, in PEM with comments (root_certificates.pem)")
	flag.StringVar(&mozillaFile, "mozilla", "", "Mozilla's root certificates in PEM (curl's cacert.pem)")
	flag.IntVar(&warnDays, "warn-days", 365, "warn about a root that expires within this many days")
	flag.Parse()

	if rootsFile == "" {
		log.Fatal("-roots is required")
	}
	if mozillaFile == "" {
		log.Fatal("-mozilla is required")
	}

	errorCount, err := run(os.Stdout, rootsFile, mozillaFile, warnDays, time.Now())
	if err != nil {
		log.Fatal(err)
	}
	if errorCount > 0 {
		os.Exit(1)
	}
}

// root is a certificate of the roots file, with the fingerprint that its
// comments state.
type root struct {
	line        int
	der         []byte
	fingerprint string
}

// identity names a certificate by its subject and public key, which all
// editions of a root share.
type identity struct {
	subject string
	key     [sha256.Size]byte
}

func identityOf(cert *x509.Certificate) identity {
	return identity{subject: string(cert.RawSubject), key: sha256.Sum256(cert.RawSubjectPublicKeyInfo)}
}

func run(out io.Writer, rootsFile, mozillaFile string, warnDays int, now time.Time) (int, error) {
	rootsText, err := os.ReadFile(rootsFile)
	if err != nil {
		return 0, fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	mozillaText, err := os.ReadFile(mozillaFile)
	if err != nil {
		return 0, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	roots, err := readRoots(string(rootsText))
	if err != nil {
		return 0, fmt.Errorf("%s: %w", rootsFile, err)
	}
	mozilla, err := readMozilla(mozillaText)
	if err != nil {
		return 0, fmt.Errorf("%s: %w", mozillaFile, err)
	}

	errorCount := 0
	warningCount := 0
	report := func(r root, cert *x509.Certificate, kind, format string, args ...any) {
		name := "an unreadable certificate"
		if cert != nil {
			name = cert.Subject.String()
		}
		fmt.Fprintf(out, "%s:%d: %s: %s: %s\n", rootsFile, r.line, kind, name, fmt.Sprintf(format, args...))
		if kind == "error" {
			errorCount++
		} else {
			warningCount++
		}
	}

	for _, r := range roots {
		cert, err := x509.ParseCertificate(r.der)
		if err != nil {
			report(r, nil, "error", "cannot parse the certificate: %v", err)
			continue
		}

		fingerprint := sha256.Sum256(r.der)
		if r.fingerprint == "" {
			report(r, cert, "error", "no %q comment above the block", fingerprintPrefix)
		} else if r.fingerprint != hex.EncodeToString(fingerprint[:]) {
			report(r, cert, "error", "the %q comment does not match the block, which has %X", fingerprintPrefix, fingerprint)
		}

		if !mozilla.fingerprints[fingerprint] {
			if other, ok := mozilla.editions[identityOf(cert)]; ok {
				report(r, cert, "error", "Mozilla has another edition of this root, with the SHA-256 fingerprint %X", other)
			} else {
				report(r, cert, "error", "Mozilla does not trust this root")
			}
		}

		if now.After(cert.NotAfter) {
			report(r, cert, "error", "expired on %s", cert.NotAfter.Format(time.DateOnly))
		} else if now.AddDate(0, 0, warnDays).After(cert.NotAfter) {
			report(r, cert, "warning", "expires on %s", cert.NotAfter.Format(time.DateOnly))
		}
	}

	fmt.Fprintf(out, "checked %d roots against %d Mozilla roots: %d errors, %d warnings\n", len(roots), len(mozilla.fingerprints), errorCount, warningCount)
	return errorCount, nil
}

// readRoots reads the certificate blocks of text, and the SHA-256 fingerprint
// that a comment states between each block and the one before it.
func readRoots(text string) ([]root, error) {
	var roots []root
	var block []string
	fingerprint := ""
	blockStart := 0

	for i, raw := range strings.Split(text, "\n") {
		line := strings.TrimRight(raw, " \t\r")
		switch {
		case line == beginLine:
			if block != nil {
				return nil, fmt.Errorf("line %d: %q inside the block from line %d", i+1, line, blockStart)
			}
			block = []string{line}
			blockStart = i + 1
		case block != nil:
			block = append(block, line)
			if line != endLine {
				continue
			}
			decoded, _ := pem.Decode([]byte(strings.Join(block, "\n") + "\n"))
			if decoded == nil {
				return nil, fmt.Errorf("line %d: the block is not valid PEM", blockStart)
			}
			roots = append(roots, root{line: blockStart, der: decoded.Bytes, fingerprint: fingerprint})
			block = nil
			fingerprint = ""
		case strings.HasPrefix(line, fingerprintPrefix):
			value := strings.TrimSpace(strings.TrimPrefix(line, fingerprintPrefix))
			fingerprint = strings.ToLower(strings.ReplaceAll(value, ":", ""))
		}
	}

	if block != nil {
		return nil, fmt.Errorf("line %d: the block has no %q line", blockStart, endLine)
	}
	if len(roots) == 0 {
		return nil, errors.New("no certificates")
	}
	return roots, nil
}

// mozillaRoots indexes Mozilla's roots by fingerprint, and the fingerprint of
// each by subject and public key.
type mozillaRoots struct {
	fingerprints map[[sha256.Size]byte]bool
	editions     map[identity][sha256.Size]byte
}

func readMozilla(text []byte) (*mozillaRoots, error) {
	roots := &mozillaRoots{
		fingerprints: map[[sha256.Size]byte]bool{},
		editions:     map[identity][sha256.Size]byte{},
	}
	for {
		var block *pem.Block
		block, text = pem.Decode(text)
		if block == nil {
			break
		}
		if block.Type != "CERTIFICATE" {
			continue
		}
		fingerprint := sha256.Sum256(block.Bytes)
		roots.fingerprints[fingerprint] = true

		// NOTE: A root that crypto/x509 cannot parse still counts by its
		// fingerprint, but cannot name another edition.
		if cert, err := x509.ParseCertificate(block.Bytes); err == nil {
			roots.editions[identityOf(cert)] = fingerprint
		}
	}
	if len(roots.fingerprints) == 0 {
		return nil, errors.New("no certificates; pass a PEM bundle such as curl's cacert.pem")
	}
	return roots, nil
}

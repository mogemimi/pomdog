// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/sha256"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"math/big"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

// newCertificate returns a self-signed certificate of key in PEM, with a
// comment line above it.
func newCertificate(t *testing.T, name string, key *ecdsa.PrivateKey, serial int64) (string, []byte) {
	t.Helper()
	template := &x509.Certificate{
		SerialNumber:          big.NewInt(serial),
		Subject:               pkix.Name{CommonName: name},
		NotBefore:             time.Now().Add(-time.Hour),
		NotAfter:              time.Now().Add(time.Hour),
		KeyUsage:              x509.KeyUsageCertSign,
		BasicConstraintsValid: true,
		IsCA:                  true,
	}
	der, err := x509.CreateCertificate(rand.Reader, template, template, &key.PublicKey, key)
	if err != nil {
		t.Fatalf("x509.CreateCertificate() failed: %v", err)
	}
	cert, err := x509.ParseCertificate(der)
	if err != nil {
		t.Fatalf("x509.ParseCertificate() failed: %v", err)
	}
	hash := sha256.Sum256(cert.RawSubjectPublicKeyInfo)
	return "# Label: \"" + name + "\"\n" + string(pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der})) + "\n", hash[:]
}

func newKey(t *testing.T) *ecdsa.PrivateKey {
	t.Helper()
	key, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		t.Fatalf("ecdsa.GenerateKey() failed: %v", err)
	}
	return key
}

// convert runs the tool on input, and returns the key hashes that it wrote.
func convert(t *testing.T, input string) ([]byte, error) {
	t.Helper()
	dir := t.TempDir()
	inFile := filepath.Join(dir, "distrusted.pem")
	outFile := filepath.Join(dir, "out", "distrusted.dkeys")
	if err := os.WriteFile(inFile, []byte(input), 0o644); err != nil {
		t.Fatal(err)
	}
	if err := run(inFile, outFile); err != nil {
		return nil, err
	}
	data, err := os.ReadFile(outFile)
	if err != nil {
		t.Fatal(err)
	}
	return schemas.GetRootAsDistrustedKeys(data, 0).KeyHashesBytes(), nil
}

func TestWritesSortedKeyHashesWithoutDuplicates(t *testing.T) {
	shared := newKey(t)
	first, sharedHash := newCertificate(t, "Authority", shared, 1)
	crossSigned, _ := newCertificate(t, "Authority", shared, 2)
	other, otherHash := newCertificate(t, "Other", newKey(t), 3)

	got, err := convert(t, "# Distrusted certificates\n\n"+first+crossSigned+other)
	if err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	want := [][]byte{sharedHash, otherHash}
	if bytes.Compare(want[0], want[1]) > 0 {
		want[0], want[1] = want[1], want[0]
	}
	if !bytes.Equal(got, bytes.Join(want, nil)) {
		t.Errorf("key hashes = %x, want %x", got, bytes.Join(want, nil))
	}
}

func TestRejectsInvalidInput(t *testing.T) {
	valid, _ := newCertificate(t, "Authority", newKey(t), 1)
	for _, c := range []struct {
		name  string
		input string
	}{
		{"no block", "# only a comment\n"},
		{"text outside a block", "text\n" + valid},
		{"missing end", strings.Replace(valid, endLine, "", 1)},
		{"invalid Base64", strings.Replace(valid, beginLine+"\n", beginLine+"\n*", 1)},
		{"not a certificate", beginLine + "\nAAECAwQF\n" + endLine + "\n"},
	} {
		t.Run(c.name, func(t *testing.T) {
			if got, err := convert(t, c.input); err == nil {
				t.Errorf("run() wrote %x, want an error", got)
			}
		})
	}
}

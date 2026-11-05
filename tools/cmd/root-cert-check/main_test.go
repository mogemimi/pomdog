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
	"fmt"
	"math/big"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

var now = time.Date(2026, time.November, 6, 0, 0, 0, 0, time.UTC)

// newRoot returns a self-signed root certificate in DER, named name, with key.
func newRoot(t *testing.T, name string, key *ecdsa.PrivateKey, serial int64, notAfter time.Time) []byte {
	t.Helper()
	template := &x509.Certificate{
		SerialNumber:          big.NewInt(serial),
		Subject:               pkix.Name{CommonName: name},
		NotBefore:             now.AddDate(-1, 0, 0),
		NotAfter:              notAfter,
		KeyUsage:              x509.KeyUsageCertSign,
		BasicConstraintsValid: true,
		IsCA:                  true,
	}
	der, err := x509.CreateCertificate(rand.Reader, template, template, &key.PublicKey, key)
	if err != nil {
		t.Fatalf("x509.CreateCertificate() failed: %v", err)
	}
	return der
}

func newKey(t *testing.T) *ecdsa.PrivateKey {
	t.Helper()
	key, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		t.Fatalf("ecdsa.GenerateKey() failed: %v", err)
	}
	return key
}

// entry returns der as an entry of a roots file, with a fingerprint comment
// that states the fingerprint of commented, or none when commented is nil.
func entry(der []byte, commented []byte) string {
	text := "# Label: \"test\"\n"
	if commented != nil {
		fingerprint := sha256.Sum256(commented)
		parts := make([]string, len(fingerprint))
		for i, b := range fingerprint {
			parts[i] = fmt.Sprintf("%02X", b)
		}
		text += fingerprintPrefix + " " + strings.Join(parts, ":") + "\n"
	}
	return text + string(pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der})) + "\n"
}

func bundle(ders ...[]byte) string {
	var text strings.Builder
	for _, der := range ders {
		text.WriteString(string(pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der})))
	}
	return text.String()
}

// check runs the tool on roots and mozilla, and returns its error count and
// output.
func check(t *testing.T, roots, mozilla string) (int, string) {
	t.Helper()
	dir := t.TempDir()
	rootsFile := filepath.Join(dir, "roots.pem")
	mozillaFile := filepath.Join(dir, "cacert.pem")
	if err := os.WriteFile(rootsFile, []byte(roots), 0o644); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(mozillaFile, []byte(mozilla), 0o644); err != nil {
		t.Fatal(err)
	}
	var out bytes.Buffer
	errorCount, err := run(&out, rootsFile, mozillaFile, 365, now)
	if err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	return errorCount, out.String()
}

func TestRootsThatMozillaTrustsPass(t *testing.T) {
	first := newRoot(t, "First", newKey(t), 1, now.AddDate(10, 0, 0))
	second := newRoot(t, "Second", newKey(t), 2, now.AddDate(10, 0, 0))
	other := newRoot(t, "Other", newKey(t), 3, now.AddDate(10, 0, 0))

	errorCount, out := check(t, entry(first, first)+entry(second, second), bundle(first, second, other))
	if errorCount != 0 {
		t.Errorf("errors = %d, want 0; output:\n%s", errorCount, out)
	}
	if !strings.Contains(out, "checked 2 roots against 3 Mozilla roots: 0 errors, 0 warnings") {
		t.Errorf("unexpected summary:\n%s", out)
	}
}

func TestRootThatMozillaDoesNotTrustFails(t *testing.T) {
	trusted := newRoot(t, "Trusted", newKey(t), 1, now.AddDate(10, 0, 0))
	dropped := newRoot(t, "Dropped", newKey(t), 2, now.AddDate(10, 0, 0))

	errorCount, out := check(t, entry(trusted, trusted)+entry(dropped, dropped), bundle(trusted))
	if (errorCount != 1) || !strings.Contains(out, "CN=Dropped: Mozilla does not trust this root") {
		t.Errorf("errors = %d, output:\n%s", errorCount, out)
	}
}

func TestOtherEditionFails(t *testing.T) {
	key := newKey(t)
	ours := newRoot(t, "Root", key, 1, now.AddDate(10, 0, 0))
	theirs := newRoot(t, "Root", key, 2, now.AddDate(10, 0, 0))

	errorCount, out := check(t, entry(ours, ours), bundle(theirs))
	want := fmt.Sprintf("another edition of this root, with the SHA-256 fingerprint %X", sha256.Sum256(theirs))
	if (errorCount != 1) || !strings.Contains(out, want) {
		t.Errorf("errors = %d, output:\n%s", errorCount, out)
	}
}

func TestFingerprintCommentMustMatch(t *testing.T) {
	first := newRoot(t, "First", newKey(t), 1, now.AddDate(10, 0, 0))
	second := newRoot(t, "Second", newKey(t), 2, now.AddDate(10, 0, 0))

	t.Run("stale comment", func(t *testing.T) {
		errorCount, out := check(t, entry(first, second), bundle(first))
		if (errorCount != 1) || !strings.Contains(out, "does not match the block") {
			t.Errorf("errors = %d, output:\n%s", errorCount, out)
		}
	})
	t.Run("missing comment", func(t *testing.T) {
		errorCount, out := check(t, entry(first, nil), bundle(first))
		if (errorCount != 1) || !strings.Contains(out, "comment above the block") {
			t.Errorf("errors = %d, output:\n%s", errorCount, out)
		}
	})
	t.Run("lowercase comment", func(t *testing.T) {
		text := entry(first, first)
		i := strings.Index(text, fingerprintPrefix) + len(fingerprintPrefix)
		j := strings.Index(text[i:], "\n") + i
		text = text[:i] + strings.ToLower(text[i:j]) + text[j:]
		if errorCount, out := check(t, text, bundle(first)); errorCount != 0 {
			t.Errorf("errors = %d, output:\n%s", errorCount, out)
		}
	})
}

func TestExpiry(t *testing.T) {
	soon := newRoot(t, "Soon", newKey(t), 1, now.AddDate(0, 0, 100))
	expired := newRoot(t, "Expired", newKey(t), 2, now.AddDate(0, 0, -1))

	errorCount, out := check(t, entry(soon, soon)+entry(expired, expired), bundle(soon, expired))
	if errorCount != 1 {
		t.Errorf("errors = %d, want 1; output:\n%s", errorCount, out)
	}
	if !strings.Contains(out, "warning: CN=Soon: expires on") || !strings.Contains(out, "error: CN=Expired: expired on") {
		t.Errorf("unexpected output:\n%s", out)
	}
}

func TestInputsWithoutCertificatesFail(t *testing.T) {
	root := newRoot(t, "Root", newKey(t), 1, now.AddDate(10, 0, 0))
	dir := t.TempDir()
	rootsFile := filepath.Join(dir, "roots.pem")
	emptyFile := filepath.Join(dir, "empty.pem")
	if err := os.WriteFile(rootsFile, []byte(entry(root, root)), 0o644); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(emptyFile, []byte("<html>not a bundle</html>\n"), 0o644); err != nil {
		t.Fatal(err)
	}

	var out bytes.Buffer
	if _, err := run(&out, rootsFile, emptyFile, 365, now); err == nil {
		t.Error("run() accepted a Mozilla bundle without certificates")
	}
	if _, err := run(&out, emptyFile, rootsFile, 365, now); err == nil {
		t.Error("run() accepted a roots file without certificates")
	}
}

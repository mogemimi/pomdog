// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/tls"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"fmt"
	"math/big"
	"net"
	"time"
)

// certificates holds the certificates of the TLS servers. The runner creates
// them at each start, so no key is kept on disk or in the repository.
type certificates struct {
	// caPEM is the root certificate, in PEM format, that the tests trust.
	caPEM []byte

	// server is the certificate for 127.0.0.1 that the root signs.
	server tls.Certificate

	// untrusted is a self-signed certificate for 127.0.0.1, which no trust
	// store of the tests holds.
	untrusted tls.Certificate
}

// newCertificates creates a root and the certificates of the TLS servers. The
// server certificates name only the IP address 127.0.0.1, so a client that
// connects to "localhost" sees a name that does not match.
func newCertificates() (*certificates, error) {
	caKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return nil, fmt.Errorf("failed to generate the CA key: %w", err)
	}
	caTemplate, err := newTemplate("Pomdog Network Test CA")
	if err != nil {
		return nil, err
	}
	caTemplate.KeyUsage = x509.KeyUsageCertSign | x509.KeyUsageCRLSign
	caTemplate.BasicConstraintsValid = true
	caTemplate.IsCA = true

	caDER, err := x509.CreateCertificate(rand.Reader, caTemplate, caTemplate, &caKey.PublicKey, caKey)
	if err != nil {
		return nil, fmt.Errorf("failed to create the CA certificate: %w", err)
	}
	ca, err := x509.ParseCertificate(caDER)
	if err != nil {
		return nil, fmt.Errorf("failed to parse the CA certificate: %w", err)
	}

	server, err := newServerCertificate("Pomdog Network Test Server", ca, caKey)
	if err != nil {
		return nil, err
	}
	untrusted, err := newServerCertificate("Pomdog Network Test Untrusted Server", nil, nil)
	if err != nil {
		return nil, err
	}

	return &certificates{
		caPEM:     pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: caDER}),
		server:    server,
		untrusted: untrusted,
	}, nil
}

// newTemplate returns a certificate template with a random serial number,
// valid from an hour ago for a day.
func newTemplate(commonName string) (*x509.Certificate, error) {
	serialNumber, err := rand.Int(rand.Reader, new(big.Int).Lsh(big.NewInt(1), 128))
	if err != nil {
		return nil, fmt.Errorf("failed to generate a serial number: %w", err)
	}

	// NOTE: Start an hour ago, so that a clock that is a little behind still
	// accepts the certificate.
	now := time.Now()
	return &x509.Certificate{
		SerialNumber: serialNumber,
		Subject:      pkix.Name{CommonName: commonName},
		NotBefore:    now.Add(-time.Hour),
		NotAfter:     now.Add(24 * time.Hour),
	}, nil
}

// newServerCertificate returns a server certificate for 127.0.0.1 that parent
// signs with parentKey, or a self-signed one when parent is nil.
func newServerCertificate(commonName string, parent *x509.Certificate, parentKey *ecdsa.PrivateKey) (tls.Certificate, error) {
	key, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return tls.Certificate{}, fmt.Errorf("failed to generate the key of %q: %w", commonName, err)
	}
	template, err := newTemplate(commonName)
	if err != nil {
		return tls.Certificate{}, err
	}
	template.KeyUsage = x509.KeyUsageDigitalSignature
	template.ExtKeyUsage = []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth}
	template.IPAddresses = []net.IP{net.IPv4(127, 0, 0, 1)}

	if parent == nil {
		parent = template
		parentKey = key
	}
	der, err := x509.CreateCertificate(rand.Reader, template, parent, &key.PublicKey, parentKey)
	if err != nil {
		return tls.Certificate{}, fmt.Errorf("failed to create the certificate of %q: %w", commonName, err)
	}
	return tls.Certificate{Certificate: [][]byte{der}, PrivateKey: key}, nil
}

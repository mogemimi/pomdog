// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"crypto"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"crypto/tls"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/hex"
	"encoding/pem"
	"fmt"
	"math/big"
	"net"
	"time"
)

// certificates holds the certificates of the TLS servers. The runner creates
// them at each start, so no key is kept on disk or in the repository.
type certificates struct {
	// caPEM holds the roots, in PEM format, that the tests trust: one with an
	// ECDSA key and one with an RSA key.
	caPEM []byte

	// server is the certificate for 127.0.0.1 with an ECDSA key, which the
	// ECDSA root signs.
	server tls.Certificate

	// rsaServer is the certificate for 127.0.0.1 with an RSA key, which the RSA
	// root signs.
	rsaServer tls.Certificate

	// untrusted is a self-signed certificate for 127.0.0.1, which no trust
	// store of the tests holds.
	untrusted tls.Certificate

	// caKeyHash and serverKeyHash are the SHA-256 hashes, in hexadecimal, of
	// the SubjectPublicKeyInfo of the ECDSA root and of server, which the tests
	// distrust.
	caKeyHash     string
	serverKeyHash string
}

// newCertificates creates the roots and the certificates of the TLS servers.
// The server certificates name only the IP address 127.0.0.1, so a client that
// connects to "localhost" sees a name that does not match.
func newCertificates() (*certificates, error) {
	caKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return nil, fmt.Errorf("failed to generate the CA key: %w", err)
	}
	ca, err := newCA("Pomdog Network Test CA", caKey)
	if err != nil {
		return nil, err
	}
	rsaCAKey, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		return nil, fmt.Errorf("failed to generate the RSA CA key: %w", err)
	}
	rsaCA, err := newCA("Pomdog Network Test RSA CA", rsaCAKey)
	if err != nil {
		return nil, err
	}

	server, err := newServerCertificate("Pomdog Network Test Server", ca, caKey)
	if err != nil {
		return nil, err
	}
	rsaServer, err := newServerCertificate("Pomdog Network Test RSA Server", rsaCA, rsaCAKey)
	if err != nil {
		return nil, err
	}
	untrusted, err := newServerCertificate("Pomdog Network Test Untrusted Server", nil, nil)
	if err != nil {
		return nil, err
	}
	serverCert, err := x509.ParseCertificate(server.Certificate[0])
	if err != nil {
		return nil, fmt.Errorf("failed to parse the server certificate: %w", err)
	}

	caPEM := pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: ca.Raw})
	caPEM = append(caPEM, pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: rsaCA.Raw})...)

	return &certificates{
		caPEM:         caPEM,
		server:        server,
		rsaServer:     rsaServer,
		untrusted:     untrusted,
		caKeyHash:     keyHash(ca),
		serverKeyHash: keyHash(serverCert),
	}, nil
}

// newCA returns a self-signed root certificate for key.
func newCA(commonName string, key crypto.Signer) (*x509.Certificate, error) {
	template, err := newTemplate(commonName)
	if err != nil {
		return nil, err
	}
	template.KeyUsage = x509.KeyUsageCertSign | x509.KeyUsageCRLSign
	template.BasicConstraintsValid = true
	template.IsCA = true

	der, err := x509.CreateCertificate(rand.Reader, template, template, key.Public(), key)
	if err != nil {
		return nil, fmt.Errorf("failed to create the certificate of %q: %w", commonName, err)
	}
	cert, err := x509.ParseCertificate(der)
	if err != nil {
		return nil, fmt.Errorf("failed to parse the certificate of %q: %w", commonName, err)
	}
	return cert, nil
}

// keyHash returns the SHA-256 hash of the SubjectPublicKeyInfo of cert in
// hexadecimal, as a .dkeys file lists it.
func keyHash(cert *x509.Certificate) string {
	hash := sha256.Sum256(cert.RawSubjectPublicKeyInfo)
	return hex.EncodeToString(hash[:])
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
// signs with parentKey, or a self-signed one when parent is nil. Its key is of
// the same type as parentKey, and an ECDSA one when parent is nil.
func newServerCertificate(commonName string, parent *x509.Certificate, parentKey crypto.Signer) (tls.Certificate, error) {
	var key crypto.Signer
	var err error
	if _, isRSA := parentKey.(*rsa.PrivateKey); isRSA {
		key, err = rsa.GenerateKey(rand.Reader, 2048)
	} else {
		key, err = ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	}
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
	der, err := x509.CreateCertificate(rand.Reader, template, parent, key.Public(), parentKey)
	if err != nil {
		return tls.Certificate{}, fmt.Errorf("failed to create the certificate of %q: %w", commonName, err)
	}
	return tls.Certificate{Certificate: [][]byte{der}, PrivateKey: key}, nil
}

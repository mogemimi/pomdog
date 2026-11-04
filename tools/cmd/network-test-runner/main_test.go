// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"crypto/tls"
	"crypto/x509"
	"errors"
	"fmt"
	"io"
	"net"
	"net/http"
	"os"
	"slices"
	"strings"
	"testing"
	"time"
)

// NOTE: When this variable is set, the test binary stands for the test
// executable, so the tests need no C++ build.
const fakeTestEnv = "NETWORK_TEST_RUNNER_FAKE_TEST"

func TestMain(m *testing.M) {
	if mode := os.Getenv(fakeTestEnv); mode != "" {
		os.Exit(fakeTest(mode, os.Args[1:]))
	}
	os.Exit(m.Run())
}

// fakeTest checks that args start with "--extra" and name each option, that
// NODE_EXTRA_CA_CERTS names the root in --ca-file, and that the HTTPS server
// answers with a certificate that the root verifies. With the "fail" mode, it
// fails without checking anything.
func fakeTest(mode string, args []string) int {
	if mode == "fail" {
		fmt.Fprintln(os.Stderr, "fake test failed")
		return 7
	}
	if (len(args) == 0) || (args[0] != "--extra") {
		return 2
	}

	values := map[string]string{}
	for _, arg := range args {
		if name, value, ok := strings.Cut(arg, "="); ok {
			values[name] = value
		}
	}
	for _, name := range []string{
		"--http-server",
		"--https-server",
		"--tcp-server",
		"--tls-server",
		"--untrusted-tls-server",
		"--udp-server",
		"--ca-file",
	} {
		if values[name] == "" {
			return 3
		}
	}
	if os.Getenv("NODE_EXTRA_CA_CERTS") != values["--ca-file"] {
		return 8
	}

	client, err := newTrustingClient(values["--ca-file"])
	if err != nil {
		return 4
	}
	resp, err := client.Get("https://" + values["--https-server"] + "/status/204")
	if err != nil {
		return 5
	}
	resp.Body.Close()
	if resp.StatusCode != http.StatusNoContent {
		return 6
	}
	return 0
}

// newTrustingClient returns an HTTP client that trusts only the root in
// caFile.
func newTrustingClient(caFile string) (*http.Client, error) {
	config, err := newTrustingConfig(caFile)
	if err != nil {
		return nil, err
	}
	return &http.Client{Transport: &http.Transport{TLSClientConfig: config}}, nil
}

// newTrustingConfig returns a TLS client configuration that trusts only the
// root in caFile.
func newTrustingConfig(caFile string) (*tls.Config, error) {
	caPEM, err := os.ReadFile(caFile)
	if err != nil {
		return nil, err
	}
	roots := x509.NewCertPool()
	if !roots.AppendCertsFromPEM(caPEM) {
		return nil, fmt.Errorf("%s holds no certificate", caFile)
	}
	return &tls.Config{RootCAs: roots}, nil
}

func TestRunPassesServerAddresses(t *testing.T) {
	t.Setenv(fakeTestEnv, "check")
	exitCode, err := run(os.Args[0], []string{"--extra"})
	if err != nil || exitCode != 0 {
		t.Fatalf("run() = %d, %v, want 0, nil", exitCode, err)
	}
}

func TestRunReturnsExitCode(t *testing.T) {
	t.Setenv(fakeTestEnv, "fail")
	exitCode, err := run(os.Args[0], nil)
	if err != nil || exitCode != 7 {
		t.Fatalf("run() = %d, %v, want 7, nil", exitCode, err)
	}
}

func startTestServers(t *testing.T) *servers {
	t.Helper()
	s, err := startServers()
	if err != nil {
		t.Fatalf("startServers() failed: %v", err)
	}
	t.Cleanup(s.close)
	return s
}

func fetch(t *testing.T, client *http.Client, req *http.Request) (*http.Response, string) {
	t.Helper()
	resp, err := client.Do(req)
	if err != nil {
		t.Fatalf("%s %s failed: %v", req.Method, req.URL, err)
	}
	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		t.Fatalf("io.ReadAll() failed: %v", err)
	}
	return resp, string(body)
}

func TestHTTPServer(t *testing.T) {
	s := startTestServers(t)
	base := "http://" + s.httpListener.Addr().String()
	client := &http.Client{}

	t.Run("echo", func(t *testing.T) {
		req, _ := http.NewRequest(http.MethodPost, base+"/echo?answer=42", strings.NewReader("hello"))
		req.Header.Set("X-Test", "hi")
		resp, body := fetch(t, client, req)

		if body != "hello" {
			t.Errorf("body = %q, want %q", body, "hello")
		}
		for name, want := range map[string]string{
			"X-Method":        "POST",
			"X-Request-URI":   "/echo?answer=42",
			"X-Host":          s.httpListener.Addr().String(),
			"X-Header-X-Test": "hi",
		} {
			if got := resp.Header.Get(name); got != want {
				t.Errorf("%s = %q, want %q", name, got, want)
			}
		}
	})
	t.Run("status", func(t *testing.T) {
		req, _ := http.NewRequest(http.MethodGet, base+"/status/404", nil)
		resp, body := fetch(t, client, req)
		if (resp.StatusCode != http.StatusNotFound) || (body != "Not Found") {
			t.Errorf("got %d %q, want 404 %q", resp.StatusCode, body, "Not Found")
		}
	})
	t.Run("bytes", func(t *testing.T) {
		req, _ := http.NewRequest(http.MethodGet, base+"/bytes/30", nil)
		resp, body := fetch(t, client, req)
		if want := "abcdefghijklmnopqrstuvwxyzabcd"; body != want {
			t.Errorf("body = %q, want %q", body, want)
		}
		if resp.ContentLength != 30 {
			t.Errorf("ContentLength = %d, want 30", resp.ContentLength)
		}
	})
	t.Run("chunked", func(t *testing.T) {
		req, _ := http.NewRequest(http.MethodGet, base+"/chunked/3", nil)
		resp, body := fetch(t, client, req)
		if body != "0,1,2," {
			t.Errorf("body = %q, want %q", body, "0,1,2,")
		}
		if !slices.Equal(resp.TransferEncoding, []string{"chunked"}) {
			t.Errorf("TransferEncoding = %v, want [chunked]", resp.TransferEncoding)
		}
	})
	t.Run("delay", func(t *testing.T) {
		req, _ := http.NewRequest(http.MethodGet, base+"/delay/1", nil)
		_, body := fetch(t, client, req)
		if body != "delayed" {
			t.Errorf("body = %q, want %q", body, "delayed")
		}
	})
	t.Run("connection ID", func(t *testing.T) {
		req, _ := http.NewRequest(http.MethodGet, base+"/echo", nil)
		first, _ := fetch(t, client, req)
		second, _ := fetch(t, client, req)
		if first.Header.Get("X-Connection-ID") != second.Header.Get("X-Connection-ID") {
			t.Errorf("a reused connection has the IDs %q and %q", first.Header.Get("X-Connection-ID"), second.Header.Get("X-Connection-ID"))
		}

		other := &http.Client{Transport: &http.Transport{}}
		third, _ := fetch(t, other, req)
		if first.Header.Get("X-Connection-ID") == third.Header.Get("X-Connection-ID") {
			t.Errorf("two connections have the same ID %q", third.Header.Get("X-Connection-ID"))
		}
	})
}

func TestHTTPSServer(t *testing.T) {
	s := startTestServers(t)
	client, err := newTrustingClient(s.caFile)
	if err != nil {
		t.Fatalf("newTrustingClient() failed: %v", err)
	}

	req, _ := http.NewRequest(http.MethodPost, "https://"+s.httpsListener.Addr().String()+"/echo", strings.NewReader("hello"))
	resp, body := fetch(t, client, req)
	if (resp.StatusCode != http.StatusOK) || (body != "hello") {
		t.Errorf("got %d %q, want 200 %q", resp.StatusCode, body, "hello")
	}
}

// checkLineEcho sends lines to c, which the server closes after "bye\n".
func checkLineEcho(t *testing.T, c net.Conn) {
	t.Helper()
	c.SetDeadline(time.Now().Add(10 * time.Second))

	if _, err := io.WriteString(c, "hello\nbye\nignored\n"); err != nil {
		t.Fatalf("write failed: %v", err)
	}

	// NOTE: The server closes the connection after "bye\n".
	buf, err := io.ReadAll(bufio.NewReader(c))
	if err != nil {
		t.Fatalf("io.ReadAll() failed: %v", err)
	}
	if got, want := string(buf), "hello\nbye\n"; got != want {
		t.Errorf("received %q, want %q", got, want)
	}
}

func TestTCPEcho(t *testing.T) {
	s := startTestServers(t)
	c, err := net.Dial("tcp", s.tcpListener.Addr().String())
	if err != nil {
		t.Fatalf("net.Dial() failed: %v", err)
	}
	defer c.Close()
	checkLineEcho(t, c)
}

func TestTLSEcho(t *testing.T) {
	s := startTestServers(t)
	config, err := newTrustingConfig(s.caFile)
	if err != nil {
		t.Fatalf("newTrustingConfig() failed: %v", err)
	}
	c, err := tls.Dial("tcp", s.tlsListener.Addr().String(), config)
	if err != nil {
		t.Fatalf("tls.Dial() failed: %v", err)
	}
	defer c.Close()
	checkLineEcho(t, c)
}

func TestTLSVerificationFails(t *testing.T) {
	s := startTestServers(t)
	config, err := newTrustingConfig(s.caFile)
	if err != nil {
		t.Fatalf("newTrustingConfig() failed: %v", err)
	}

	_, port, _ := net.SplitHostPort(s.tlsListener.Addr().String())
	for name, address := range map[string]string{
		// NOTE: The certificate names only the IP address.
		"name mismatch": net.JoinHostPort("localhost", port),
		"untrusted":     s.untrustedTLSListener.Addr().String(),
	} {
		t.Run(name, func(t *testing.T) {
			dialer := &tls.Dialer{NetDialer: &net.Dialer{Timeout: 10 * time.Second}, Config: config}
			c, err := dialer.Dial("tcp", address)
			if err == nil {
				c.Close()
				t.Fatalf("tls.Dial(%q) succeeded, want an error", address)
			}
			var verifyErr *tls.CertificateVerificationError
			if !errors.As(err, &verifyErr) {
				t.Errorf("tls.Dial(%q) failed with %v, want a verification error", address, err)
			}
		})
	}
}

func TestUDPEcho(t *testing.T) {
	s := startTestServers(t)
	c, err := net.Dial("udp", s.udpConn.LocalAddr().String())
	if err != nil {
		t.Fatalf("net.Dial() failed: %v", err)
	}
	defer c.Close()
	c.SetDeadline(time.Now().Add(10 * time.Second))

	if _, err := io.WriteString(c, "ping"); err != nil {
		t.Fatalf("write failed: %v", err)
	}
	buf := make([]byte, 16)
	n, err := c.Read(buf)
	if err != nil {
		t.Fatalf("read failed: %v", err)
	}
	if got := string(buf[:n]); got != "ping" {
		t.Errorf("received %q, want %q", got, "ping")
	}
}

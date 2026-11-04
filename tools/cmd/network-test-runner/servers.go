// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"context"
	"crypto/tls"
	"errors"
	"fmt"
	"io"
	"net"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"sync/atomic"
	"time"
)

// servers holds the local servers that the network tests talk to. Each one
// listens on a port of 127.0.0.1 that the system picks.
type servers struct {
	httpServer           *http.Server
	httpListener         net.Listener
	httpsServer          *http.Server
	httpsListener        net.Listener
	tcpListener          net.Listener
	tlsListener          net.Listener
	untrustedTLSListener net.Listener
	udpConn              net.PacketConn

	// closedTCPAddress is an address of 127.0.0.1 where nothing listens, so
	// that a connection to it is refused.
	closedTCPAddress string

	// caDir is the temporary directory that holds caFile.
	caDir string

	// caFile is the root certificate that the TLS servers chain up to, except
	// the untrusted one.
	caFile string
}

// startServers starts the servers, and writes the root certificate that the
// tests trust into a temporary file.
func startServers() (*servers, error) {
	certs, err := newCertificates()
	if err != nil {
		return nil, err
	}

	s := &servers{}
	if err := s.start(certs); err != nil {
		s.close()
		return nil, err
	}
	return s, nil
}

func (s *servers) start(certs *certificates) error {
	var err error
	if s.caDir, err = os.MkdirTemp("", "network-test-runner-"); err != nil {
		return fmt.Errorf("failed to create a temporary directory: %w", err)
	}
	s.caFile = filepath.Join(s.caDir, "ca.pem")
	if err := os.WriteFile(s.caFile, certs.caPEM, 0o644); err != nil {
		return fmt.Errorf("failed to write the CA certificate: %w", err)
	}

	serverTLS := &tls.Config{Certificates: []tls.Certificate{certs.server}}
	untrustedTLS := &tls.Config{Certificates: []tls.Certificate{certs.untrusted}}

	if s.httpListener, err = listen(nil); err != nil {
		return fmt.Errorf("failed to listen for HTTP: %w", err)
	}
	if s.httpsListener, err = listen(serverTLS); err != nil {
		return fmt.Errorf("failed to listen for HTTPS: %w", err)
	}
	if s.tcpListener, err = listen(nil); err != nil {
		return fmt.Errorf("failed to listen for TCP: %w", err)
	}
	if s.tlsListener, err = listen(serverTLS); err != nil {
		return fmt.Errorf("failed to listen for TLS: %w", err)
	}
	if s.untrustedTLSListener, err = listen(untrustedTLS); err != nil {
		return fmt.Errorf("failed to listen for untrusted TLS: %w", err)
	}
	if s.udpConn, err = net.ListenPacket("udp", "127.0.0.1:0"); err != nil {
		return fmt.Errorf("failed to listen for UDP: %w", err)
	}
	if s.closedTCPAddress, err = closedTCPAddress(); err != nil {
		return fmt.Errorf("failed to find a closed TCP port: %w", err)
	}

	s.httpServer = newHTTPServer()
	s.httpsServer = newHTTPServer()
	go s.httpServer.Serve(s.httpListener)
	go s.httpsServer.Serve(s.httpsListener)
	go serveLineEcho(s.tcpListener)
	go serveLineEcho(s.tlsListener)
	go serveLineEcho(s.untrustedTLSListener)
	go serveUDPEcho(s.udpConn)
	return nil
}

// listen listens for TCP connections on 127.0.0.1, and serves them with TLS
// when config is not nil.
func listen(config *tls.Config) (net.Listener, error) {
	l, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		return nil, err
	}
	if config != nil {
		return tls.NewListener(l, config), nil
	}
	return l, nil
}

// closedTCPAddress returns an address of 127.0.0.1 where nothing listens. It
// listens on a port that the system picks and closes it at once, so another
// program could take the port only in that short time.
func closedTCPAddress() (string, error) {
	l, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		return "", err
	}
	address := l.Addr().String()
	if err := l.Close(); err != nil {
		return "", err
	}
	return address, nil
}

// options returns the command-line options that pass the server addresses and
// the root certificate to the test executable.
func (s *servers) options() []string {
	return []string{
		"--http-server=" + s.httpListener.Addr().String(),
		"--https-server=" + s.httpsListener.Addr().String(),
		"--tcp-server=" + s.tcpListener.Addr().String(),
		"--tls-server=" + s.tlsListener.Addr().String(),
		"--untrusted-tls-server=" + s.untrustedTLSListener.Addr().String(),
		"--udp-server=" + s.udpConn.LocalAddr().String(),
		"--closed-tcp-server=" + s.closedTCPAddress,
		"--ca-file=" + s.caFile,
	}
}

// close stops the servers that started, and removes the root certificate.
func (s *servers) close() {
	for _, h := range []struct {
		server   *http.Server
		listener net.Listener
	}{
		{s.httpServer, s.httpListener},
		{s.httpsServer, s.httpsListener},
	} {
		if h.server != nil {
			h.server.Close()
		} else if h.listener != nil {
			h.listener.Close()
		}
	}
	for _, l := range []net.Listener{s.tcpListener, s.tlsListener, s.untrustedTLSListener} {
		if l != nil {
			l.Close()
		}
	}
	if s.udpConn != nil {
		s.udpConn.Close()
	}
	if s.caDir != "" {
		os.RemoveAll(s.caDir)
	}
}

type connectionIDKey struct{}

// newHTTPServer returns a server that answers the paths that the README lists.
// Each response has an X-Connection-ID header, which numbers the TCP
// connections, so the tests can tell when the client reuses a connection.
func newHTTPServer() *http.Server {
	var lastConnectionID atomic.Uint64

	mux := http.NewServeMux()
	mux.HandleFunc("/echo", handleEcho)
	mux.HandleFunc("/status/{code}", handleStatus)
	mux.HandleFunc("/bytes/{size}", handleBytes)
	mux.HandleFunc("/chunked/{count}", handleChunked)
	mux.HandleFunc("/delay/{milliseconds}", handleDelay)

	return &http.Server{
		Handler: http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			id := r.Context().Value(connectionIDKey{}).(uint64)
			w.Header().Set("X-Connection-ID", strconv.FormatUint(id, 10))
			mux.ServeHTTP(w, r)
		}),
		ConnContext: func(ctx context.Context, c net.Conn) context.Context {
			return context.WithValue(ctx, connectionIDKey{}, lastConnectionID.Add(1))
		},
	}
}

// handleEcho answers with the request body, and describes the request in
// X-Method, X-Request-URI, X-Host, and X-Header-<name> headers.
func handleEcho(w http.ResponseWriter, r *http.Request) {
	body, err := io.ReadAll(r.Body)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	h := w.Header()
	h.Set("X-Method", r.Method)
	h.Set("X-Request-URI", r.RequestURI)
	h.Set("X-Host", r.Host)
	for name, values := range r.Header {
		h.Set("X-Header-"+name, strings.Join(values, ", "))
	}
	h.Set("Content-Type", "application/octet-stream")
	h.Set("Content-Length", strconv.Itoa(len(body)))
	w.Write(body)
}

// handleStatus answers with the status code in the path, and its status text
// as the body unless the status allows no body.
func handleStatus(w http.ResponseWriter, r *http.Request) {
	code, err := strconv.Atoi(r.PathValue("code"))
	if (err != nil) || (code < 200) || (code > 599) {
		http.Error(w, "the status code must be from 200 to 599", http.StatusBadRequest)
		return
	}
	if (code == http.StatusNoContent) || (code == http.StatusNotModified) {
		w.WriteHeader(code)
		return
	}
	text := http.StatusText(code)
	w.Header().Set("Content-Type", "text/plain")
	w.Header().Set("Content-Length", strconv.Itoa(len(text)))
	w.WriteHeader(code)
	io.WriteString(w, text)
}

// handleBytes answers with a body of the size in the path, which repeats the
// letters from a to z, and a Content-Length header.
func handleBytes(w http.ResponseWriter, r *http.Request) {
	size, err := strconv.Atoi(r.PathValue("size"))
	if (err != nil) || (size < 0) {
		http.Error(w, "the size must be a number of bytes", http.StatusBadRequest)
		return
	}
	body := make([]byte, size)
	for i := range body {
		body[i] = byte('a' + (i % 26))
	}
	w.Header().Set("Content-Type", "application/octet-stream")
	w.Header().Set("Content-Length", strconv.Itoa(size))
	w.Write(body)
}

// handleChunked answers with the numbers from 0 to count-1, each followed by a
// comma, and sends each number as its own chunk.
func handleChunked(w http.ResponseWriter, r *http.Request) {
	count, err := strconv.Atoi(r.PathValue("count"))
	if (err != nil) || (count < 0) {
		http.Error(w, "the count must be a number of chunks", http.StatusBadRequest)
		return
	}
	w.Header().Set("Content-Type", "text/plain")

	// NOTE: Without a Content-Length header, each flush writes one chunk.
	rc := http.NewResponseController(w)
	for i := range count {
		fmt.Fprintf(w, "%d,", i)
		if err := rc.Flush(); err != nil {
			return
		}
	}
}

// handleDelay waits for the milliseconds in the path, then answers with
// "delayed".
func handleDelay(w http.ResponseWriter, r *http.Request) {
	milliseconds, err := strconv.Atoi(r.PathValue("milliseconds"))
	if (err != nil) || (milliseconds < 0) {
		http.Error(w, "the delay must be a number of milliseconds", http.StatusBadRequest)
		return
	}
	select {
	case <-time.After(time.Duration(milliseconds) * time.Millisecond):
	case <-r.Context().Done():
		return
	}
	const text = "delayed"
	w.Header().Set("Content-Type", "text/plain")
	w.Header().Set("Content-Length", strconv.Itoa(len(text)))
	io.WriteString(w, text)
}

// serveLineEcho echoes each line that ends with "\n" back to the client, and
// closes the connection after it echoes "bye\n". A TLS connection sends
// close_notify before it closes.
func serveLineEcho(l net.Listener) {
	for {
		c, err := l.Accept()
		if err != nil {
			return
		}
		go func() {
			defer c.Close()
			r := bufio.NewReader(c)
			for {
				line, err := r.ReadString('\n')
				if err != nil {
					return
				}
				if _, err := io.WriteString(c, line); err != nil {
					return
				}
				if line == "bye\n" {
					return
				}
			}
		}()
	}
}

// serveUDPEcho sends each datagram back to its sender.
func serveUDPEcho(c net.PacketConn) {
	buf := make([]byte, 65535)
	for {
		n, addr, err := c.ReadFrom(buf)
		if err != nil {
			if errors.Is(err, net.ErrClosed) {
				return
			}
			continue
		}
		c.WriteTo(buf[:n], addr)
	}
}

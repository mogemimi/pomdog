// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"context"
	"errors"
	"fmt"
	"io"
	"net"
	"net/http"
	"strconv"
	"strings"
	"sync/atomic"
	"time"
)

// servers holds the local servers that the network tests talk to. Each one
// listens on a port of 127.0.0.1 that the system picks.
type servers struct {
	httpServer   *http.Server
	httpListener net.Listener
	tcpListener  net.Listener
	udpConn      net.PacketConn
}

// startServers starts the HTTP, TCP, and UDP servers.
func startServers() (*servers, error) {
	s := &servers{}

	var err error
	if s.httpListener, err = net.Listen("tcp", "127.0.0.1:0"); err != nil {
		s.close()
		return nil, fmt.Errorf("failed to listen for HTTP: %w", err)
	}
	if s.tcpListener, err = net.Listen("tcp", "127.0.0.1:0"); err != nil {
		s.close()
		return nil, fmt.Errorf("failed to listen for TCP: %w", err)
	}
	if s.udpConn, err = net.ListenPacket("udp", "127.0.0.1:0"); err != nil {
		s.close()
		return nil, fmt.Errorf("failed to listen for UDP: %w", err)
	}

	s.httpServer = newHTTPServer()
	go s.httpServer.Serve(s.httpListener)
	go serveTCPEcho(s.tcpListener)
	go serveUDPEcho(s.udpConn)
	return s, nil
}

// options returns the command-line options that pass the server addresses to
// the test executable.
func (s *servers) options() []string {
	return []string{
		"--http-server=" + s.httpListener.Addr().String(),
		"--tcp-server=" + s.tcpListener.Addr().String(),
		"--udp-server=" + s.udpConn.LocalAddr().String(),
	}
}

// close stops the servers that started.
func (s *servers) close() {
	if s.httpServer != nil {
		s.httpServer.Close()
	} else if s.httpListener != nil {
		s.httpListener.Close()
	}
	if s.tcpListener != nil {
		s.tcpListener.Close()
	}
	if s.udpConn != nil {
		s.udpConn.Close()
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

// serveTCPEcho echoes each line that ends with "\n" back to the client, and
// closes the connection after it echoes "bye\n".
func serveTCPEcho(l net.Listener) {
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

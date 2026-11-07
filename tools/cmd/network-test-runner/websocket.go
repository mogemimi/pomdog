// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"crypto/sha1"
	"encoding/base64"
	"encoding/binary"
	"errors"
	"io"
	"net"
	"net/http"
	"strconv"
	"strings"
	"time"
	"unicode/utf8"
)

// The opcodes of WebSocket frames (RFC 6455, section 5.2).
const (
	opContinuation = 0x0
	opText         = 0x1
	opBinary       = 0x2
	opClose        = 0x8
	opPing         = 0x9
	opPong         = 0xA
)

// webSocketGUID is appended to the key of the client to compute
// Sec-WebSocket-Accept (RFC 6455, section 1.3).
const webSocketGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// maxWebSocketMessage limits the messages that the server reads.
const maxWebSocketMessage = 64 << 20

// webSocketTimeout ends a connection that the tests left open.
const webSocketTimeout = 30 * time.Second

// webSocketConn is a WebSocket connection that a handler took over from
// net/http.
type webSocketConn struct {
	conn net.Conn
	rw   *bufio.ReadWriter
}

// webSocketFrame is a frame that the client sent, with its payload unmasked.
type webSocketFrame struct {
	fin     bool
	opcode  byte
	payload []byte
}

// errClientClosed tells that the client sent a Close frame, which the server
// answered.
var errClientClosed = errors.New("the client closed the connection")

// registerWebSocketHandlers adds the WebSocket paths that the README lists.
func registerWebSocketHandlers(mux *http.ServeMux) {
	mux.HandleFunc("/ws/echo", handleWebSocketEcho)
	mux.HandleFunc("/ws/close", handleWebSocketClose)
	mux.HandleFunc("/ws/drop", handleWebSocketDrop)
	mux.HandleFunc("/ws/ping", handleWebSocketPing)
	mux.HandleFunc("/ws/fragments", handleWebSocketFragments)
	mux.HandleFunc("/ws/bytes/{size}", handleWebSocketBytes)
	mux.HandleFunc("/ws/bad/{kind}", handleWebSocketBad)
	mux.HandleFunc("/ws/handshake/{kind}", handleWebSocketHandshake)
	mux.HandleFunc("/ws/silent", handleWebSocketSilent)
}

// computeWebSocketAccept returns the Sec-WebSocket-Accept value for key.
func computeWebSocketAccept(key string) string {
	hash := sha1.Sum([]byte(key + webSocketGUID))
	return base64.StdEncoding.EncodeToString(hash[:])
}

// hasToken reports whether the comma-separated list in value has token,
// ignoring case.
func hasToken(value, token string) bool {
	for _, element := range strings.Split(value, ",") {
		if strings.EqualFold(strings.TrimSpace(element), token) {
			return true
		}
	}
	return false
}

// acceptWebSocket checks the handshake request, takes over the connection,
// and answers with the extra header lines in extra, such as a subprotocol.
// When accept is not empty, it replaces the Sec-WebSocket-Accept value.
func acceptWebSocket(w http.ResponseWriter, r *http.Request, accept string, extra []string) (*webSocketConn, error) {
	key := r.Header.Get("Sec-WebSocket-Key")
	nonce, err := base64.StdEncoding.DecodeString(key)
	switch {
	case r.Method != http.MethodGet:
		err = errors.New("the WebSocket handshake must be a GET request")
	case !hasToken(r.Header.Get("Upgrade"), "websocket"):
		err = errors.New("the WebSocket handshake has no Upgrade: websocket header")
	case !hasToken(r.Header.Get("Connection"), "Upgrade"):
		err = errors.New("the WebSocket handshake has no Connection: Upgrade header")
	case r.Header.Get("Sec-WebSocket-Version") != "13":
		err = errors.New("the WebSocket handshake must have Sec-WebSocket-Version: 13")
	case (err != nil) || (len(nonce) != 16):
		err = errors.New("the WebSocket handshake must have a 16-byte Sec-WebSocket-Key")
	}
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return nil, err
	}

	conn, rw, err := http.NewResponseController(w).Hijack()
	if err != nil {
		http.Error(w, "the connection cannot be taken over", http.StatusInternalServerError)
		return nil, err
	}
	conn.SetDeadline(time.Now().Add(webSocketTimeout))

	if accept == "" {
		accept = computeWebSocketAccept(key)
	}
	var b strings.Builder
	b.WriteString("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n")
	b.WriteString("Sec-WebSocket-Accept: " + accept + "\r\n")
	for _, line := range extra {
		b.WriteString(line + "\r\n")
	}
	b.WriteString("\r\n")
	rw.WriteString(b.String())
	if err := rw.Flush(); err != nil {
		conn.Close()
		return nil, err
	}
	return &webSocketConn{conn: conn, rw: rw}, nil
}

// readFrame reads a frame, which a client must mask.
func (c *webSocketConn) readFrame() (webSocketFrame, error) {
	var header [2]byte
	if _, err := io.ReadFull(c.rw, header[:]); err != nil {
		return webSocketFrame{}, err
	}
	frame := webSocketFrame{fin: (header[0] & 0x80) != 0, opcode: header[0] & 0x0f}
	if (header[0] & 0x70) != 0 {
		return frame, c.fail(1002, "the client sent a frame with a reserved bit")
	}
	if (header[1] & 0x80) == 0 {
		return frame, c.fail(1002, "the client sent a frame without a mask")
	}

	length := uint64(header[1] & 0x7f)
	switch length {
	case 126:
		var extended [2]byte
		if _, err := io.ReadFull(c.rw, extended[:]); err != nil {
			return frame, err
		}
		length = uint64(binary.BigEndian.Uint16(extended[:]))
	case 127:
		var extended [8]byte
		if _, err := io.ReadFull(c.rw, extended[:]); err != nil {
			return frame, err
		}
		length = binary.BigEndian.Uint64(extended[:])
	}
	if length > maxWebSocketMessage {
		return frame, c.fail(1009, "the client sent a frame that is too large")
	}

	var mask [4]byte
	if _, err := io.ReadFull(c.rw, mask[:]); err != nil {
		return frame, err
	}
	frame.payload = make([]byte, length)
	if _, err := io.ReadFull(c.rw, frame.payload); err != nil {
		return frame, err
	}
	for i := range frame.payload {
		frame.payload[i] ^= mask[i%4]
	}
	return frame, nil
}

// writeFrame writes a frame without a mask, as a server does.
func (c *webSocketConn) writeFrame(fin bool, opcode byte, payload []byte) error {
	first := opcode
	if fin {
		first |= 0x80
	}
	header := []byte{first}
	switch {
	case len(payload) < 126:
		header = append(header, byte(len(payload)))
	case len(payload) <= 0xffff:
		header = append(header, 126)
		header = binary.BigEndian.AppendUint16(header, uint16(len(payload)))
	default:
		header = append(header, 127)
		header = binary.BigEndian.AppendUint64(header, uint64(len(payload)))
	}
	c.rw.Write(header)
	c.rw.Write(payload)
	return c.rw.Flush()
}

// writeRaw writes bytes as they are, such as a frame that breaks the
// protocol.
func (c *webSocketConn) writeRaw(b []byte) error {
	c.rw.Write(b)
	return c.rw.Flush()
}

// makeClosePayload returns the payload of a Close frame with code and
// reason, or an empty payload when code is zero.
func makeClosePayload(code int, reason string) []byte {
	if code == 0 {
		return nil
	}
	return append(binary.BigEndian.AppendUint16(nil, uint16(code)), reason...)
}

// fail sends a Close frame with code, closes the connection, and returns an
// error with message.
func (c *webSocketConn) fail(code int, message string) error {
	c.writeFrame(true, opClose, makeClosePayload(code, message))
	c.conn.Close()
	return errors.New(message)
}

// isValidCloseCode reports whether code may appear in a Close frame.
func isValidCloseCode(code int) bool {
	return ((code >= 1000) && (code <= 1003)) || ((code >= 1007) && (code <= 1014)) || ((code >= 3000) && (code <= 4999))
}

// readMessage reads the next data message, putting fragments together. It
// answers Ping frames with Pong frames, and a Close frame with the same code
// before it closes the connection and returns errClientClosed.
func (c *webSocketConn) readMessage() (byte, []byte, error) {
	var opcode byte
	var message []byte
	inMessage := false
	for {
		frame, err := c.readFrame()
		if err != nil {
			return 0, nil, err
		}
		switch frame.opcode {
		case opPing:
			if err := c.writeFrame(true, opPong, frame.payload); err != nil {
				return 0, nil, err
			}
			continue
		case opPong:
			continue
		case opClose:
			if len(frame.payload) == 1 {
				return 0, nil, c.fail(1002, "the client sent a Close frame with a 1-byte payload")
			}
			if len(frame.payload) >= 2 {
				code := int(binary.BigEndian.Uint16(frame.payload))
				if !isValidCloseCode(code) {
					return 0, nil, c.fail(1002, "the client sent an invalid close code")
				}
				if !utf8.Valid(frame.payload[2:]) {
					return 0, nil, c.fail(1007, "the client sent a close reason that is not UTF-8")
				}
			}
			c.writeFrame(true, opClose, frame.payload)
			c.conn.Close()
			return 0, nil, errClientClosed
		case opContinuation:
			if !inMessage {
				return 0, nil, c.fail(1002, "the client sent a continuation without a message")
			}
		case opText, opBinary:
			if inMessage {
				return 0, nil, c.fail(1002, "the client started a message before the last one ended")
			}
			opcode = frame.opcode
			inMessage = true
		default:
			return 0, nil, c.fail(1002, "the client sent an unknown opcode")
		}
		message = append(message, frame.payload...)
		if len(message) > maxWebSocketMessage {
			return 0, nil, c.fail(1009, "the client sent a message that is too large")
		}
		if frame.fin {
			if (opcode == opText) && !utf8.Valid(message) {
				return 0, nil, c.fail(1007, "the client sent text that is not UTF-8")
			}
			return opcode, message, nil
		}
	}
}

// echo sends each message back with its type until the client closes.
func (c *webSocketConn) echo() {
	defer c.conn.Close()
	for {
		opcode, message, err := c.readMessage()
		if err != nil {
			return
		}
		if err := c.writeFrame(true, opcode, message); err != nil {
			return
		}
	}
}

// handleWebSocketEcho echoes each message. With a protocol query, it answers
// with that subprotocol, even one that the client did not offer; without
// one, it picks the first subprotocol that the client offered.
func handleWebSocketEcho(w http.ResponseWriter, r *http.Request) {
	protocol := r.URL.Query().Get("protocol")
	if protocol == "" {
		offered := strings.Split(r.Header.Get("Sec-WebSocket-Protocol"), ",")
		protocol = strings.TrimSpace(offered[0])
	}
	var extra []string
	if protocol != "" {
		extra = append(extra, "Sec-WebSocket-Protocol: "+protocol)
	}
	c, err := acceptWebSocket(w, r, "", extra)
	if err != nil {
		return
	}
	c.echo()
}

// handleWebSocketClose sends a Close frame with the code and the reason of
// the query, or without a code when the query has none, and closes the
// connection once the client answers.
func handleWebSocketClose(w http.ResponseWriter, r *http.Request) {
	code, _ := strconv.Atoi(r.URL.Query().Get("code"))
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	defer c.conn.Close()
	if err := c.writeFrame(true, opClose, makeClosePayload(code, r.URL.Query().Get("reason"))); err != nil {
		return
	}
	for {
		frame, err := c.readFrame()
		if (err != nil) || (frame.opcode == opClose) {
			return
		}
	}
}

// handleWebSocketDrop closes the TCP connection after the handshake, without
// a Close frame.
func handleWebSocketDrop(w http.ResponseWriter, r *http.Request) {
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	c.conn.Close()
}

// handleWebSocketPing sends a Ping frame with "hello", and once the Pong
// frame comes back, a text message "pong:" with its payload. Then it echoes.
func handleWebSocketPing(w http.ResponseWriter, r *http.Request) {
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	if err := c.writeFrame(true, opPing, []byte("hello")); err != nil {
		c.conn.Close()
		return
	}
	for {
		frame, err := c.readFrame()
		if err != nil {
			c.conn.Close()
			return
		}
		if frame.opcode == opPong {
			if err := c.writeFrame(true, opText, append([]byte("pong:"), frame.payload...)); err != nil {
				c.conn.Close()
				return
			}
			break
		}
	}
	c.echo()
}

// handleWebSocketFragments sends the text message "hello, world" in three
// fragments with a Ping frame between them. Then it echoes.
func handleWebSocketFragments(w http.ResponseWriter, r *http.Request) {
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	c.writeFrame(false, opText, []byte("hello"))
	c.writeFrame(true, opPing, []byte("between"))
	c.writeFrame(false, opContinuation, []byte(", wor"))
	c.writeFrame(true, opContinuation, []byte("ld"))
	c.echo()
}

// handleWebSocketBytes sends a binary message of the size in the path, which
// repeats the letters from a to z. Then it echoes.
func handleWebSocketBytes(w http.ResponseWriter, r *http.Request) {
	size, err := strconv.Atoi(r.PathValue("size"))
	if (err != nil) || (size < 0) {
		http.Error(w, "the size must be a number of bytes", http.StatusBadRequest)
		return
	}
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	if err := c.writeFrame(true, opBinary, letters(size)); err != nil {
		c.conn.Close()
		return
	}
	c.echo()
}

// handleWebSocketBad sends a frame that breaks the protocol in the way that
// the path names, then echoes until the client closes.
func handleWebSocketBad(w http.ResponseWriter, r *http.Request) {
	frames := map[string][]byte{
		// A frame with the MASK bit, which only a client sets.
		"masked": {0x81, 0x82, 0, 0, 0, 0, 'h', 'i'},
		// A text frame with RSV1, which needs an extension.
		"reserved": {0xc1, 0x02, 'h', 'i'},
		// A frame with opcode 3, which is reserved.
		"opcode": {0x83, 0x02, 'h', 'i'},
		// A text frame that is not UTF-8.
		"utf8": {0x81, 0x02, 0xc3, 0x28},
		// A continuation frame without a message to continue.
		"continuation": {0x80, 0x02, 'h', 'i'},
		// A Close frame with 1005, which a frame must not hold.
		"close-code": {0x88, 0x02, 0x03, 0xed},
		// A Ping frame that is fragmented.
		"fragmented-ping": {0x09, 0x02, 'h', 'i'},
	}
	frame, ok := frames[r.PathValue("kind")]
	if !ok {
		http.Error(w, "unknown kind of bad frame", http.StatusBadRequest)
		return
	}
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	if err := c.writeRaw(frame); err != nil {
		c.conn.Close()
		return
	}
	c.echo()
}

// handleWebSocketHandshake answers the handshake in a way that the path
// names, which the client must refuse.
func handleWebSocketHandshake(w http.ResponseWriter, r *http.Request) {
	accept := ""
	var extra []string
	switch r.PathValue("kind") {
	case "accept":
		accept = computeWebSocketAccept("not the key of the client")
	case "extension":
		extra = append(extra, "Sec-WebSocket-Extensions: permessage-deflate")
	default:
		http.Error(w, "unknown kind of bad handshake", http.StatusBadRequest)
		return
	}
	c, err := acceptWebSocket(w, r, accept, extra)
	if err != nil {
		return
	}
	c.echo()
}

// handleWebSocketSilent reads frames and answers none, not even a Close
// frame, until the client closes the TCP connection.
func handleWebSocketSilent(w http.ResponseWriter, r *http.Request) {
	c, err := acceptWebSocket(w, r, "", nil)
	if err != nil {
		return
	}
	defer c.conn.Close()
	for {
		if _, err := c.readFrame(); err != nil {
			return
		}
	}
}

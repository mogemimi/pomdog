# network-test-runner

Starts local servers for the Pomdog network tests, runs the tests with their
addresses, and exits with the exit code of the tests.

## Background

The network tests in `tests/network` build into `pomdog_network_tests`. They
talk to local servers instead of hosts on the internet, so their results do not
depend on the network or on how a public server answers. The servers listen on
`127.0.0.1` with ports that the system picks, and the runner passes the
addresses on the command line. `pomdog_network_tests` fails when an option is
missing.

The runner creates the root certificates and the TLS server certificates each
time it starts, and writes the roots to a temporary file that it removes at
exit. The server certificates name only the IP address `127.0.0.1`, so a test that
connects to `localhost` checks that a name mismatch fails.

## Installation

```sh
cd tools/cmd/network-test-runner
go build -o ../../../build/tools/network-test-runner
```

## Usage

```sh
network-test-runner <command> [arguments...]
```

The runner runs the command with the arguments unchanged, followed by these
options. It also sets `NODE_EXTRA_CA_CERTS` to the file of the roots, so that
Node.js trusts the servers when it runs the web build of the tests.

| Option | Server |
|---|---|
| `--http-server=<host:port>` | HTTP server that answers the paths below |
| `--https-server=<host:port>` | HTTPS server that answers the same paths |
| `--tls12-https-server=<host:port>` | HTTPS server as above that accepts only TLS 1.2 with `ECDHE-ECDSA-CHACHA20-POLY1305` |
| `--rsa-https-server=<host:port>` | HTTPS server as above with an RSA certificate that the RSA root signs |
| `--tcp-server=<host:port>` | TCP server that echoes each line and closes the connection after `bye\n` |
| `--tls-server=<host:port>` | TLS server that echoes as the TCP server does, and after `drop\n` closes the TCP connection without close_notify |
| `--untrusted-tls-server=<host:port>` | TLS server that echoes with a self-signed certificate |
| `--udp-server=<host:port>` | UDP server that sends each datagram back |
| `--closed-tcp-server=<host:port>` | Address where nothing listens, so that a TCP connection to it is refused |
| `--ca-file=<path>` | The roots in PEM format that the HTTPS and TLS servers chain up to, except the untrusted one: an ECDSA root and an RSA root |
| `--ca-key-hash=<hex>` | SHA-256 of the SubjectPublicKeyInfo of the ECDSA root, as a `.dkeys` file lists it |
| `--server-key-hash=<hex>` | SHA-256 of the SubjectPublicKeyInfo of the ECDSA certificate of the HTTPS and TLS servers |

The HTTP and HTTPS servers answer these paths. Each response has an `X-Connection-ID`
header that numbers the TCP connections, so a test can tell whether the client
reused a connection.

| Path | Response |
|---|---|
| `/echo` | The request body, with `X-Method`, `X-Request-URI`, `X-Host`, and `X-Header-<name>` headers that describe the request |
| `/status/{code}` | The status code from 200 to 599, with its status text as the body |
| `/bytes/{size}` | A body of `size` bytes that repeats `a` to `z`, with `Content-Length` |
| `/chunked/{count}` | `0,1,...,count-1,` with each number in its own chunk |
| `/redirect/{code}?location={url}` | The redirect status from 300 to 399, with `url` as the `Location` header, or without one when the query has none |
| `/redirects/{count}` | A redirect with status 302 to `/redirects/{count-1}`, and at zero the response of `/echo` |
| `/gzip/{size}` | The body of `/bytes/{size}` compressed with gzip, with `Content-Encoding: gzip` whatever the request accepts |
| `/close/{size}` | A body as `/bytes/{size}` has, without `Content-Length` or chunks, which ends when the server closes the connection |
| `/delay/{milliseconds}` | `delayed` after the delay |
| `/drop-kept` | The response of `/echo` on a new connection. On a connection that answered a request before, it closes the connection without a response |
| `/connection-close` | An empty body with `Connection: close`, and keeps the connection open, so that a request that the client sends on it gets no answer |

The same servers answer WebSocket handshakes on these paths. They check that
the client masks its frames, answer each Ping with a Pong, and answer a Close
frame with the same payload before they close the connection.

| Path | Server |
|---|---|
| `/ws/echo?protocol={name}` | Sends each message back with its type. Picks the subprotocol `name`, even one that the client did not offer, or without the query the first one that the client offered. An empty `name` answers with an empty header, and `none` with no header |
| `/ws/close?code={code}&reason={text}` | Sends a Close frame with `code` and `text`, or without a code when the query has none, and closes the connection after the client answers |
| `/ws/drop` | Closes the TCP connection after the handshake, without a Close frame |
| `/ws/ping` | Sends a Ping with `hello`, and once the Pong comes back, the text message `pong:` with its payload. Then echoes |
| `/ws/fragments` | Sends the text message `hello, world` in three fragments with a Ping between them. Then echoes |
| `/ws/bytes/{size}` | Sends a binary message of `size` bytes that repeats `a` to `z`. Then echoes |
| `/ws/bad/{kind}` | Sends a frame that breaks the protocol, then echoes: `masked`, `reserved` (RSV1), `opcode` (3), `utf8` (text that is not UTF-8), `continuation` (without a message), `close-code` (1005), or `fragmented-ping` |
| `/ws/handshake/{kind}` | Answers the handshake wrongly: `accept` with a wrong `Sec-WebSocket-Accept`, or `extension` with `Sec-WebSocket-Extensions: permessage-deflate` |
| `/ws/silent` | Reads frames and answers none, not even a Close frame |

### Examples

Run the network tests:

```sh
./build/tools/network-test-runner ./build/linux/tests/Debug/pomdog_network_tests
```

Pass doctest options, such as a test case filter:

```sh
./build/tools/network-test-runner ./build/linux/tests/Debug/pomdog_network_tests -tc="TCPStream*"
```

The command can also be Node.js with the web build of the tests, as
[Building with Emscripten](../../../docs/building-emscripten.md) shows.

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
| `--tcp-server=<host:port>` | TCP server that echoes each line and closes the connection after `bye\n` |
| `--tls-server=<host:port>` | TLS server that echoes as the TCP server does, and after `drop\n` closes the TCP connection without close_notify |
| `--untrusted-tls-server=<host:port>` | TLS server that echoes with a self-signed certificate |
| `--udp-server=<host:port>` | UDP server that sends each datagram back |
| `--closed-tcp-server=<host:port>` | Address where nothing listens, so that a TCP connection to it is refused |
| `--ca-file=<path>` | The root certificate in PEM format that the HTTPS and TLS servers chain up to, except the untrusted one |

The HTTP and HTTPS servers answer these paths. Each response has an `X-Connection-ID`
header that numbers the TCP connections, so a test can tell whether the client
reused a connection.

| Path | Response |
|---|---|
| `/echo` | The request body, with `X-Method`, `X-Request-URI`, `X-Host`, and `X-Header-<name>` headers that describe the request |
| `/status/{code}` | The status code from 200 to 599, with its status text as the body |
| `/bytes/{size}` | A body of `size` bytes that repeats `a` to `z`, with `Content-Length` |
| `/chunked/{count}` | `0,1,...,count-1,` with each number in its own chunk |
| `/delay/{milliseconds}` | `delayed` after the delay |

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

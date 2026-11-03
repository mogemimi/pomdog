# network-test-runner

Starts local servers for the Pomdog network tests, runs the test executable with
their addresses, and exits with the exit code of the tests.

## Background

The network tests in `tests/network` build into `pomdog_network_tests`. They
talk to local servers instead of hosts on the internet, so their results do not
depend on the network or on how a public server answers. The servers listen on
`127.0.0.1` with ports that the system picks, and the runner passes the
addresses on the command line. `pomdog_network_tests` fails when an address is
missing.

## Installation

```sh
cd tools/cmd/network-test-runner
go build -o ../../../build/tools/network-test-runner
```

## Usage

```sh
network-test-runner <test executable> [test options...]
```

The runner passes these options to the test executable, followed by the test
options unchanged:

| Option | Server |
|---|---|
| `--http-server=<host:port>` | HTTP server that answers the paths below |
| `--tcp-server=<host:port>` | TCP server that echoes each line and closes the connection after `bye\n` |
| `--udp-server=<host:port>` | UDP server that sends each datagram back |

The HTTP server answers these paths. Each response has an `X-Connection-ID`
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

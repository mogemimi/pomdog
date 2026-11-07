# Network Internals

This page describes how `pomdog/network` is built, for engine developers. For
the API and the behavior that applications see, see [Network](network.md).

## Layout

The public headers in `pomdog/network` declare interfaces, their events and
options, and factory functions. Each factory's `.cpp` file creates the
platform's implementation, which lives in `pomdog::network::detail` and never
appears in a public header:

| Directory | Contents | Platforms |
|:---|:---|:---|
| `win32/` | Winsock TCP and UDP streams, and `WinsockLibrary` | Windows |
| `posix/` | TCP and UDP streams on POSIX sockets | macOS, Linux |
| `mbedtls/` | TLS stream and trust store on Mbed TLS | Windows, macOS, Linux |
| `native/` | HTTP/1.1 client and WebSocket on top of `Stream` | Windows, macOS, Linux |
| `emscripten/` | HTTP client on `fetch()`, and the browser's WebSocket | Emscripten |

All platforms share the parsers and the input checks, such as `http_parser` and
`http_request_check`; the code behind them is specific to a backend.
[`pomdog_network.cmake`](../cmake/pomdog/pomdog_network.cmake) lists the sources
per platform. The web build compiles neither socket code nor Mbed TLS: the
stream factories return an error, and the trust store is an empty object.

Each Windows object that owns sockets holds a `WinsockLibrary`, because Winsock
counts `WSAStartup()` calls and stops at the matching last `WSACleanup()`.

## Threads and clocks

- Native streams run on the thread that calls `update()`, except the blocking
  part of connecting: for `connectTCPStream()`, `connectTLSStream()`, and
  `connectUDPStream()`, name resolution and the socket connection run on a
  detached thread of `ConnectWorker` (`connect_worker.h`), and `update()` takes
  the socket once it is ready. `listenUDPStream()` and `UDPStream::writeTo()`
  resolve addresses on the calling thread, so they can hold up a frame.
- `update()` gives up at the connect timeout even while name resolution blocks
  the thread, and destroying a stream returns at once. A thread that ends after
  that closes its socket itself, and holds its own `WinsockLibrary` on Windows.
  A thread that still runs when the program exits ends with the process.
- Mbed TLS runs only on the `update()` thread, so it needs no
  `MBEDTLS_THREADING_C`. The handshake advances without blocking in each
  `update()`, limited by the connect timeout.
- Timeouts use `std::chrono::steady_clock`, not the GameClock, which pauses with
  the game.

## Events and buffers

- An `HTTPClient` queues the errors that `send()` finds at once, such as an
  invalid URL, and reports them in the next `update()`.
- TCP and TLS `write()` send what the socket takes at once and keep the rest in
  a send buffer, which `update()` flushes before reading. The buffer drops its
  sent part once that is half of it, as `HTTPParser` does. A send failure closes
  the stream with an error in its Disconnected event, so `write()` fails only
  when the stream is not connected. A write closes the stream the same way
  while more than `maxPendingSendSize` bytes (`send_limits.h`) wait.
- `update()` reads until the socket would block, up to the limit per call in
  `receive_limits.h`. Each stream reads into its own buffer, allocated at the
  first read, which holds the largest UDP datagram. TCP and TLS join what they
  read into one Received event, queued before a Disconnected that the same
  `update()` finds. UDP counts each read as at least a byte, so that a flood of
  empty datagrams also ends the loop.
- A stream reads before it checks `idleTimeout`, so that the bytes that arrived
  during a long pause between two `update()` calls count as activity.

## HTTP client

- `HTTPParser` keeps the received bytes in one buffer with a read offset, and
  drops the read part once it is half of the buffer, so that each byte moves
  only a few times however the server splits the body. It limits the status
  line and header fields to 64 KiB, and the body to `maxResponseSize`.
- The native `HTTPClient` keeps idle connections by scheme, host, and port,
  except after an HTTP/1.0 response or one with `Connection: close`. Each
  `update()` closes those that the server closed, that timed out, or that
  received bytes, which belong to no request. `cancel()` keeps a connection that
  connected and sent nothing, and closes one that still connects or may hold
  half a response.
- A server may close a kept connection as a request goes out on it. When a
  reused connection closes before a byte of the response arrives, the session
  starts again on a new connection if the request did not go out, or if its
  method is idempotent and less than `idleTimeout` passed, since the stream may
  otherwise have timed out itself.
- The native client follows a redirect in `finishResponse()`, which replaces
  the request of the session with the one that `makeRedirectRequest()` in
  `http_redirect.cpp` returns, and starts it on the same session, so the event
  keeps the request ID. `makeRedirectRequest()` resolves `Location` by RFC 3986
  and applies the steps of HTTP-redirect fetch in the Fetch standard, so it
  needs no sockets to test. The next request reuses an idle connection to the
  same origin.
- `checkRequestHeaders()`, `checkRequestMethod()`, and `checkRequestBody()`
  hold the forbidden request-headers, methods, and bodies of the Fetch
  standard. Update them when the standard adds a name.
- Both clients and both WebSocket implementations parse the URL with
  `parseRequestURL()` in `http_request_check.cpp`. It fails what browsers
  refuse, and also spaces and control characters, which browsers percent-encode
  but which could add lines to a native request header.

## TLS

- Verification is `MBEDTLS_SSL_VERIFY_REQUIRED`. The stream passes the host to
  `mbedtls_ssl_set_hostname()` for SNI and the name check; Mbed TLS checks an IP
  address against the IP addresses of the certificate. A failure reports the
  reasons from `mbedtls_x509_crt_verify_info()`.
- After `MBEDTLS_ERR_SSL_WANT_WRITE`, Mbed TLS keeps the record and returns the
  length of the next call, so the stream retries with the same length
  (`pendingWriteSize_`) before it takes more bytes from the send buffer.
- Under TLS 1.3, `mbedtls_ssl_read()` and `mbedtls_ssl_write()` return
  `MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET` when the server sends a session
  ticket, and the stream calls them again.
- `mbedtls_ssl_read()` returns zero when the connection closes without
  close_notify, and the stream reports that as an error, so that a body that
  ends at the close cannot be cut short unnoticed (RFC 8446, section 6.1).
- With distrusted keys, the stream passes a callback to
  `mbedtls_ssl_conf_verify()`, which Mbed TLS calls for each certificate of the
  chain that it built. The callback hashes `pk_raw`, the SubjectPublicKeyInfo as
  the certificate holds it, which is what distrusted-keys-gen hashes, and sets
  `MBEDTLS_X509_BADCERT_OTHER` on a match. A hash that cannot be computed counts
  as a match. `createTrustStore()` rejects a `.dkeys` file whose hashes are not
  sorted without duplicates, so the lookup is a binary search.
- TLS streams and trust stores call `psa_crypto_init()`, which Mbed TLS 4 needs
  before any cryptography.
- Mbed TLS ignores SIGPIPE only in `mbedtls_net_connect()` and
  `mbedtls_net_bind()`, which Pomdog does not call. POSIX sends, including the
  TLS stream's send callback, pass `MSG_NOSIGNAL` on Linux, and sockets set
  `SO_NOSIGPIPE` on macOS.

## Mbed TLS

Mbed TLS and TF-PSA-Crypto generate some sources with Perl and Python in their
own CMake build. `cmake/mbedtls` compiles copies of those sources from
`thirdparty/vendor/mbedtls` instead, so a Pomdog build needs neither tool.

Mbed TLS reads Pomdog's configuration, `mbedtls_config.h` and `crypto_config.h`
in `thirdparty/vendor/mbedtls/pomdog`, instead of the upstream defaults.
`cmake/mbedtls` passes them as public `MBEDTLS_CONFIG_FILE` and
`TF_PSA_CRYPTO_CONFIG_FILE` definitions, so that code that includes Mbed TLS
headers sees the same options. Each file lists every option that it enables, so
an option that upstream adds stays off.

The configuration enables only a TLS 1.2 and TLS 1.3 client: ECDHE with X25519
and the NIST curves, AES-GCM and ChaCha20-Poly1305, and ECDSA and RSA
signatures. It has no TLS server, DTLS, PSK, session tickets, renegotiation, key
storage, or debug output. SHA-1 stays for `Sec-WebSocket-Accept`, and so that
the roots of Mozilla's store that sign themselves with SHA-1 parse; the default
X.509 profile still rejects SHA-1 signatures in a chain.

### Updating Mbed TLS

`thirdparty/mbedtls` and `thirdparty/vendor` are separate submodules, and an
update changes both. Run the commands from the repository root.

1. Check out the new version with its nested submodules:

   ```sh
   git -C thirdparty/mbedtls fetch --tags
   git -C thirdparty/mbedtls checkout <version>
   git -C thirdparty/mbedtls submodule update --init --recursive
   ```

2. Regenerate the sources in `thirdparty/vendor/mbedtls`, which needs Perl, and
   Python 3 with jinja2:

   ```sh
   ./tools/script/generate_mbedtls_sources.sh
   ```

3. Align the source list in `cmake/mbedtls/CMakeLists.txt` with the upstream
   `library` and `tf-psa-crypto` CMake files. Keep every upstream source; those
   of disabled features compile to nothing.
4. Compare `mbedtls_config.h` and `crypto_config.h` with the upstream
   `include/mbedtls/mbedtls_config.h` and
   `tf-psa-crypto/include/psa/crypto_config.h`, including
   `MBEDTLS_CONFIG_VERSION` and `TF_PSA_CRYPTO_CONFIG_VERSION`, and add only what
   Pomdog needs.
5. Build on every platform, and run `pomdog_tests` and `pomdog_network_tests`.
   See [Running the Tests](running-the-tests.md#network-tests).
6. Update [the list of open source software](open-source-software-used-in-pomdog.md)
   when the update brings in code under another license.
7. Commit `thirdparty/vendor`, and then commit both submodules and
   `cmake/mbedtls` in Pomdog.

## WebSocket

- The native socket sends the handshake request once its stream connects, and
  reads the 101 response with `parseResponseHeader()` of `http_parser`. It
  computes `Sec-WebSocket-Accept` with SHA-1 and masks each frame with a key
  from `psa_generate_random()`, so it calls `psa_crypto_init()` for ws URLs
  too.
- `WebSocketReader` in `web_socket_frame.cpp` keeps the received bytes with a
  read offset, as `HTTPParser` does. It fails with the code that the client
  sends in its Close frame: 1002 for a frame that breaks the protocol, 1007
  for text that is not UTF-8, which it checks once a message is complete, and
  1009 for a message larger than `maxMessageSize`, which the length in the
  frame header tells before the payload arrives.
- After the closing handshake, the server closes the TCP connection first (RFC
  6455, section 7.1.1). The native socket waits for that up to
  `connectTimeout`, then closes it itself; the Close frame of the server makes
  the closure clean either way.
- Of the Pings that one `update()` reads, the native socket answers only the
  last, as RFC 6455, section 5.5.3, allows, so that a flood of Pings cannot hold
  up a frame.
- `checkWebSocketProtocols()` and `checkWebSocketClose()` fail what the
  browser's `WebSocket` refuses, so that native code cannot depend on what the
  web rejects. The native socket drops a leading byte order mark of a received
  text message, as the UTF-8 decode of the Encoding standard does in browsers.

## Web

- `HTTPClientEmscripten` calls `fetch()` through `EM_JS`, so a request advances
  in JavaScript whether or not the game calls `update()`. JavaScript holds each
  request that ended, with its whole body, until `update()` copies it onto the
  WebAssembly heap, which costs memory and frame time for a large body.
- `redirectMode`, `cacheMode`, and `credentialsMode` become the `redirect`,
  `cache`, and `credentials` options of `fetch()`. `SameOrigin` fails before
  the request where `location` is undefined, which tells Node.js from a page or
  a worker.
- Each request has an `AbortController` for `cancel()`, for the idle timer,
  which restarts at the response headers and at each body chunk, and for
  `maxResponseSize`, which the reader loop checks at each chunk.
- `WebSocketEmscripten` keeps a queue of the events of each socket in
  JavaScript, which `update()` takes as the HTTP client does. JavaScript timers
  apply `connectTimeout` to the handshake and to `close()`, and a socket that
  closes removes its handlers, so it queues nothing more. Text passes to
  `send()` through `TextDecoder` with `ignoreBOM`, so that a byte order mark at
  its start stays.
- The `close()` of the browser takes only the codes that the application may
  send, so a socket that fails for `maxMessageSize` closes without a code.

## Verification

| What changed | Where to check it |
|:---|:---|
| Parsers and input checks, such as `http_parser` and `web_socket_frame` | `pomdog_tests`, which opens no sockets |
| Sockets, TLS, resending, and the native WebSocket | `pomdog_network_tests`, against the local servers of network-test-runner |
| The `fetch()` client and the browser's `WebSocket` | The web build of `pomdog_network_tests` under Node.js |
| Real servers and CORS | The `HTTPClient Test` scene of `examples/feature_showcase`, on each platform and in a browser |

See [Running the Tests](running-the-tests.md#network-tests) for the commands, and
[network-test-runner](../tools/cmd/network-test-runner/README.md) for its servers
and certificates.

- The web build holds only `http_client_test.cpp` and `web_socket_test.cpp`,
  without the tests of what the native client does itself, such as connection
  reuse, since the browser picks connections. The WebSocket of Node.js checks
  the frames of the runner's WebSocket server independently of the native
  client.
- The web build of `pomdog_network_tests` links with `ASYNCIFY`, so that
  `emscripten_sleep()` in the tests' wait loop returns to the event loop where
  `fetch()` settles, and with `EXIT_RUNTIME`, so that `emscripten_force_exit()`
  keeps the exit code. The web test executables build with `-fexceptions`, so
  that doctest catches a failed `REQUIRE` and runs the remaining tests.
- No scene covers WebSocket yet, so only Node.js runs the web `WebSocket`. No
  scene or test sends the cookies of `SameOrigin`, which needs a page.

# Network

`pomdog::network` sends HTTP requests, talks to WebSocket servers, and connects
to servers over TCP, TLS, and UDP. The application creates the clients and
streams that it needs and advances them from its game loop. Link
`pomdog::network` to use the library; see
[Using Pomdog with CMake](using-pomdog-with-cmake.md). For how the library is
built, see [Network Internals](network-internals.md).

## Choose an API

| Use | Object | Created by | Platforms |
|:---|:---|:---|:---|
| Ask a server and get one answer, such as a list of lobbies | `HTTPClient` | `createHTTPClient()` | All |
| Exchange messages in both directions, also in browsers | `WebSocket` | `connectWebSocket()` | All |
| Run your own protocol over a connection | `Stream` | `connectTCPStream()`, `connectTLSStream()` | Windows, macOS, Linux |
| Run your own protocol over datagrams | `UDPStream` | `connectUDPStream()`, `listenUDPStream()` | Windows, macOS, Linux |

Browsers offer only HTTP, through `fetch()`, and WebSocket, so the stream
functions return an error on the web. https requests and wss sockets also need a
`TrustStore`, which the next section creates.

## Send an HTTPS request

Create the trust store and the client once, such as in `Game::initialize()`. On
native platforms, the store holds the roots that verify servers. In browsers,
the browser verifies servers and the store holds nothing, but the same code
applies. [Shipping certificates](#shipping-certificates) tells where the two
files come from.

```cpp
// Without the error checks
auto [pemFile, pemFileErr] = vfs::open(
    fs, computeStringHash64("/assets"), computeStringHash64("certificates/root_certificates.pem"));
auto [pem, pemErr] = pemFile->view();
auto [keysFile, keysFileErr] = vfs::open(
    fs, computeStringHash64("/assets"), computeStringHash64("certificates/distrusted_keys.dkeys"));
auto [keys, keysErr] = keysFile->view();

network::TrustStoreDesc desc;
desc.rootCertificatesPEM = pem;
desc.distrustedKeys = keys;
auto trustStore = network::createTrustStore(desc);

network::HTTPClientOptions options;
options.trustStore = std::move(*trustStore);
httpClient_ = *network::createHTTPClient(options);
```

Send a request when the game needs the data, and keep its ID:

```cpp
network::HTTPRequest request;
request.method = network::HTTPMethod::Get;
request.url = "https://example.com/lobbies";
requestID_ = httpClient_->send(std::move(request));
```

Take the result in `Game::update()`:

```cpp
httpClient_->update();
while (auto event = httpClient_->pollEvent()) {
    if (event->requestID != requestID_) {
        continue;
    }
    if (!event->result) {
        // No response: an invalid URL, a failed connection, or a timeout.
        showError(*event->result.error());
    }
    else if (event->result->statusCode != 200) {
        // A response that this endpoint does not expect, such as 404.
        showStatus(event->result->status);
    }
    else {
        useBody(event->result->body);
    }
}
```

The [`HTTPClient Test` scene](../examples/feature_showcase/source/http_client_test/http_client_test.cpp)
of `feature_showcase` does this with error handling, on every platform.

## Updates and events

Results reach the game only through `update()` and `pollEvent()`. Call
`update()` once per frame from `Game::update()`, and then call `pollEvent()`
until the queue is empty. `Stream`, `UDPStream`, and `WebSocket` work the same
way as `HTTPClient`.

- Objects are not thread-safe. Use them from the thread that runs the game loop.
- Timeouts use a monotonic clock, so they keep running while the game clock is
  paused. `connectTimeout` includes name resolution.
- Destroying a stream or a client while it connects returns at once, even while
  name resolution blocks.

## HTTP

One client handles any number of requests at a time. `send()` returns a request
ID, and `HTTPEvent::requestID` tells which request a result belongs to.

- The `HTTPEvent` of a request arrives after a later `update()`, even when the
  request fails at once, such as for an invalid URL, so callers handle one path.
  A response with an error status, such as 404, is a response, not a failure.
- After `HTTPClient::cancel()`, no event for that request arrives, including
  one that already waits in the queue.
- The event holds the whole body once the request ends. No event reports a part
  of a body or the progress of a download. `HTTPClientOptions::maxResponseSize`,
  64 MiB by default, fails a request whose response body is larger, counting the
  decoded bytes.

### Requests that fail

Every platform checks a request before it sends it, so that a request that a
browser would refuse does not work only natively. The checks cover only the
request itself; a browser adds
[its own rules](#native-and-browser-differences), such as CORS.

- A request fails when its headers hold one that the client sets itself
  (`Host`, `Connection`, `Content-Length`, `Transfer-Encoding`, and
  `Accept-Encoding`), or one that browsers do not let `fetch()` send, such as
  `Cookie`, `Origin`, `Referer`, and the names that start with `Sec-` or
  `Proxy-`. Browsers would drop those without an error. A header value with a
  line break also fails.
- A request with the method CONNECT or TRACE fails, and so does a GET or HEAD
  request with a body, because browsers refuse them.
- A request fails when its URL has user information, such as
  `user:password@`, which browsers refuse, or a space or a control character,
  which browsers would percent-encode. Percent-encode them in the URL.

### Request options

- `HTTPRequest::redirectMode` is `Error` by default, which fails a response with
  status 301, 302, 303, 307, or 308 without a request to its location. `Follow`
  follows up to 20 redirects as `fetch()` does, and `HTTPResponse::url` tells
  the URL of the last request. Following sends the request's headers, such as
  an API key, to each location, and removes only `Authorization` when the
  origin changes, so follow only redirects that the game expects, such as from
  a download URL to a CDN.
- `HTTPRequest::credentialsMode` is `Omit` by default, so requests neither send
  nor store cookies. `SameOrigin` lets the browser send the cookies of the
  page's origin. Native platforms and Node.js have no page, so the request fails
  there.
- `HTTPRequest::cacheMode` is `Default` by default, which lets the browser cache
  responses as their headers, such as `Cache-Control`, allow. `NoStore` keeps a
  request out of the cache. Native clients have no cache.

### Native client

On Windows, macOS, and Linux, `HTTPClient` speaks HTTP/1.1 over its own
sockets:

- It keeps a connection open when `HTTPRequest::persistentConnection` is true
  and the response allows it, and reuses it for a later request to the same
  scheme, host, and port. A kept connection closes after `idleTimeout` without a
  request, or when the server closes it.
- When the server closes a kept connection before it answers, the client sends
  the request again on a new connection, except a POST request that went out,
  which the server may have applied, and a request that went out at least
  `idleTimeout` ago, which may have timed out.
- It reads a body by its `Content-Length`, in chunks, or until the server closes
  the connection. Responses to HEAD, and those with status 204 or 304, have no
  body.
- It has no cache and no cookies. It decodes no content coding, and fails a
  response whose body has one, such as gzip.

## WebSocket

`WebSocket` sends and receives messages by the WebSocket protocol (RFC 6455).
On Windows, macOS, and Linux it runs over Pomdog's TCP and TLS streams; on the
web it is the browser's `WebSocket`.

`connectWebSocket()` returns an error at once for an invalid URL or invalid
subprotocols, and for a wss URL without a trust store. After it succeeds, the
handshake still has to finish, so start sending at the Connected event.
`web_socket.h` shows the loop.

Every platform checks the URL, the subprotocols, and the arguments of `close()`
the same way. A browser adds
[its own rules](#native-and-browser-differences) to the connection.

- The URL is ws or wss, without a fragment, user information, a space, or a
  control character. A wss URL needs `WebSocketOptions::trustStore`, as an https
  request does.
- The handshake request has no headers of the application, since browsers let
  a page add none. To authenticate, connect over wss and send a token in the
  first message, and have the server accept nothing else until then. Keep
  long-lived tokens out of the URL, which servers and proxies may log. A
  redirect of the handshake fails.
- When a socket offers subprotocols, the connection fails unless the server
  picks one of them, as in browsers, though RFC 6455 lets a server pick none.
- `WebSocketOptions::connectTimeout` limits the time from name resolution to
  the server's handshake response, and how long `close()` waits for the server.
- Each Received event holds one message, text or binary. Text is valid UTF-8
  without a leading byte order mark. `sendText()` fails text that is not valid
  UTF-8. A message larger than `WebSocketOptions::maxMessageSize` closes the
  socket with an error.
- `close()` takes the code 1000 or one from 3000 to 4999, and a reason of up to
  123 bytes, and drops the messages that arrive after it. After the closing
  handshake, Disconnected holds the server's code and reason; without it, such
  as when the server drops the connection, it holds 1006 and an error.
- The client answers Pings and sends none, as browsers do. To notice a lost
  connection, exchange messages and close when none arrives in time.

## TCP, TLS, and UDP streams

A TCP or TLS stream reports Connected, Received for the data that arrived, and
Disconnected when it closes. A connection that fails reports only Disconnected.
Disconnected carries an error when the connection failed, timed out, or could
not send, and none when the peer closed it. A TLS stream also reports an error
when the peer closes without close_notify, since someone could have cut the data
short.

What one Received event holds depends on the object:

| Object | One Received event holds |
|:---|:---|
| `WebSocket` | One message |
| `UDPStream` | One datagram |
| `Stream` over TCP or TLS | The bytes that one `update()` read |

The events of a TCP or TLS stream do not follow the sender's writes: two writes
of `hello` and `world` can arrive as `helloworld`, or as `hel` and `loworld`. A
protocol over these streams marks where each message ends, such as with a length
prefix, and keeps partial bytes until the rest arrives.

- Each `update()` reads what has arrived, up to a limit that keeps a fast sender
  from holding up a frame; the rest waits for the next `update()`.
- `write()` of a TCP or TLS stream does not block. Bytes that the connection
  cannot take at once wait in the stream, and later `update()` calls send them.
  `close()` drops them and reports no Disconnected event. A write while more
  than 64 MiB wait closes the stream with an error, so that a peer that stops
  reading cannot use up the memory. One write may be larger.
- A TCP or TLS stream counts the bytes that arrived before an `update()` as
  activity for `idleTimeout`, so a long pause, such as a load, does not fail a
  connection that received data. The same goes for the native `HTTPClient`,
  which uses these streams.
- A `UDPStream` reports Connected and Received as well, but UDP does not tell
  when the peer goes away. Disconnected reports only a socket that failed, and
  `write()` and `writeTo()` return a send failure instead of closing the stream.
- A stream from `listenUDPStream()` receives datagrams from any sender. Answer
  with `writeTo()` and the `address` of the Received event.
- `listenUDPStream()` and `writeTo()` resolve a host name on the calling thread,
  which can hold up a frame. Pass them a numeric address where that matters.

## Native and browser differences

On the web, `HTTPClient` sends requests with `fetch()`, and `WebSocket` is the
browser's, so the browser's rules apply:

- A page can read a response from another origin only when the server allows the
  page's origin (CORS). Otherwise the request fails as an event, although the
  server may have received it. A response from another origin shows only the
  headers that CORS allows, such as `Content-Type`.
- A page served over https cannot request http URLs or open ws URLs. CORS
  does not apply to WebSocket, so the server checks `Origin` itself.

Other differences:

| | Native | Web |
|:---|:---|:---|
| Server verification | The roots and distrusted keys of the `TrustStore` | The browser's roots |
| `cacheMode` `Default` | No cache | The browser's cache |
| `credentialsMode` `SameOrigin` | Fails | The cookies of the page's origin; fails in Node.js |
| Content coding | Fails a body that has one | Negotiated and decoded, so the body may be longer than `contentLength` |
| `Follow` from https to http | Fails | Followed in Node.js; a page served over https blocks it |
| WebSocket handshake | Only the headers of the protocol | Also `Origin` with the page's origin, which the server may check, and the cookies of the server's site |
| WebSocket extensions | None | `permessage-deflate` when the server accepts it, which compresses messages |
| WebSocket Close frame sent on a failure | 1002, 1007, or 1009, as the protocol defines | The browser's choice; none for a message larger than `maxMessageSize` |
| WebSocket messages waiting to be sent | Closes the socket beyond 64 MiB | The browser's limit |
| `persistentConnection` | Keeps the connection | Ignored; the browser keeps connections |
| `HTTPClientOptions::connectTimeout` | Limits connecting | Unused |
| `HTTPClientOptions::idleTimeout` | Fails a request that sends and receives nothing | Fails a request that receives nothing |
| Response header names | As received, in order | Lowercase, sorted |
| `HTTPResponse::protocol` | As received, such as `HTTP/1.1` | Empty |
| `HTTPResponse::status` | `200 OK` | `200 OK`, or `200` over HTTP/2, which has no status text |

## Shipping certificates

Native platforms verify servers against the root certificates and the distrusted
keys that the game passes to `createTrustStore()`. In browsers, the browser
verifies servers with its own roots, and `createTrustStore()` reads neither
input. https requests and wss URLs still need the store there, so that the same
code runs on every platform.

Pomdog has no built-in root certificates, and verification cannot be turned
off. On native platforms, a TLS stream, an https request, and a wss socket
accept a server only when its certificate names the host and chains up to a
root in the store. Without a store, `connectTLSStream()` and
`connectWebSocket()` with a wss URL return an error, and an `HTTPClient` fails
https requests; http requests and ws URLs still work.

The application ships its roots:

- `assets/certificates/root_certificates.pem` holds the roots of the Pomdog
  examples. A game chooses the roots of the servers it talks to, and ships a new
  set when a server changes its certificate authority. The asset build checks
  the file and drops its comments with
  [pem-compact](../tools/cmd/pem-compact/README.md).
- Pomdog does not check revocation (CRL or OCSP). When a certificate authority is
  compromised, browsers block it, but a game keeps trusting every root in its
  store until it ships a new one. Choose only roots that the browsers trust, and
  remove a root when they stop trusting it.
- `assets/certificates/distrusted_certificates.pem` lists intermediate
  authorities that the browsers block while they still trust the root. The asset
  build turns it into `distrusted_keys.dkeys`, the SHA-256 hashes of their public
  keys, with [distrusted-keys-gen](../tools/cmd/distrusted-keys-gen/README.md).
  With those keys, native TLS connections reject a chain that holds one of them
  under any issuer, including a cross-signed copy, which shares its key.
  `createTrustStore()` fails when a root has a distrusted key.
- A store does not change after it is created. Clients and streams can share
  one store.

### Reading the files from the archive

The VFS can mount a directory of loose files over an archive, so that a loose
file takes priority over the archive entry with the same path. A game can use
this [overlay](asset-pipeline-and-runtime.md#virtual-file-system-vfs) during
development, or to load mods. For most assets that is the point, but a stray
`root_certificates.pem` in an overlay directory would change which servers the
game trusts.

To avoid that, put both files in the game's archive and read them by hash key,
as [Send an HTTPS request](#send-an-https-request) does. A lookup by hash key
reads only the archive, so the game does not pick up a file outside of it by
mistake. See
[Hash-Based Lookup](asset-pipeline-and-runtime.md#hash-based-lookup).

> **Note:** This covers only that mistake, and is not a strong security
> guarantee. Someone who can write to the game's files can also replace the
> archive or change the executable.

### Updating the roots

Check the roots against Mozilla's root store before each release, and whenever
the browsers distrust a certificate authority:

1. Download Mozilla's roots in PEM, such as curl's `cacert.pem` from
   <https://curl.se/ca/cacert.pem>, into `build/`.
2. Run [root-cert-check](../tools/cmd/root-cert-check/README.md) on the roots
   file. It fails for a root that Mozilla no longer trusts or holds in another
   edition, and for a fingerprint comment that does not match its block, and it
   warns about a root that expires within a year.
3. Remove or replace each root that it reports, and run it again.

The check compares roots only. It cannot see an intermediate authority that a
trusted root signed.

### Updating the distrust list

When the browsers block an intermediate authority without its root, add its
certificate to `distrusted_certificates.pem`, with comments that cite the
incident. One certificate covers its cross-signed copies, which share its key.
Remove an entry only when no root in the store can chain to it any longer.

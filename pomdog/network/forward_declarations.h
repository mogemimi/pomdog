// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::network {

class HTTPClient;
class Stream;
class TrustStore;
class UDPStream;
class WebSocket;

struct HTTPClientOptions;
struct HTTPEvent;
struct HTTPRequest;
struct HTTPResponse;
struct StreamEvent;
struct StreamOptions;
struct UDPStreamEvent;
struct WebSocketEvent;
struct WebSocketOptions;

enum class HTTPCacheMode : u8;
enum class HTTPCredentialsMode : u8;
enum class HTTPMethod : u8;
enum class HTTPRedirectMode : u8;
enum class StreamEventKind : u8;
enum class WebSocketMessageType : u8;

} // namespace pomdog::network

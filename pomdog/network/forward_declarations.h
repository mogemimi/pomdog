// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::network {

class HTTPClient;
class Stream;
class TrustStore;
class UDPStream;

struct HTTPClientOptions;
struct HTTPEvent;
struct HTTPRequest;
struct HTTPResponse;
struct StreamEvent;
struct StreamOptions;
struct UDPStreamEvent;

enum class HTTPMethod : u8;
enum class StreamEventKind : u8;

} // namespace pomdog::network

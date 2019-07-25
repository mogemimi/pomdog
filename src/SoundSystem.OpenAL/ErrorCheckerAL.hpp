// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class ErrorCheckerAL {
public:
    static void CheckError(const char* command, const char* filename, int line);
};

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog

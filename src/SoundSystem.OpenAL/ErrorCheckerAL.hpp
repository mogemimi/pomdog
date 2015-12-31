// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class ErrorCheckerAL {
public:
    static void CheckError(char const* command, char const* filename, int line);
};

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog

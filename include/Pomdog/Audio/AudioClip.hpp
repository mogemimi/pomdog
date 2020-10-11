// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT AudioClip {
public:
    virtual ~AudioClip() noexcept;

    /// Gets the length of the audio clip in seconds.
    [[nodiscard]] virtual Duration
    GetLength() const noexcept = 0;

    /// Gets the number of samples per second.
    [[nodiscard]] virtual int
    GetSampleRate() const noexcept = 0;

    /// Gets the number of bits per sample.
    [[nodiscard]] virtual int
    GetBitsPerSample() const noexcept = 0;

    /// Gets the number of channels in the audip clip.
    [[nodiscard]] virtual AudioChannels
    GetChannels() const noexcept = 0;
};

} // namespace Pomdog

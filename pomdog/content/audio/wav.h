// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class AudioContainer;
} // namespace pomdog

namespace pomdog::detail {
class AudioClipFile;
} // namespace pomdog::detail

namespace pomdog::vfs {
class File;
} // namespace pomdog::vfs

namespace pomdog {

/// Reads audio data from Waveform Audio File (.wav) format data.
/// @param file The span of bytes containing the WAV file data.
/// @return A tuple containing the decoded audio data and an error object if an error occurred.
[[nodiscard]] POMDOG_EXPORT std::tuple<AudioContainer, std::unique_ptr<Error>>
decodeWAV(std::span<const u8> file) noexcept;

/// Opens a WAV audio file for streaming.
/// @param file A file object representing the WAV file.
/// @return A tuple containing the audio clip file object and an error object if an error occurred.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::unique_ptr<detail::AudioClipFile>, std::unique_ptr<Error>>
openAudioClipFileWAV(std::shared_ptr<vfs::File> file) noexcept;

} // namespace pomdog

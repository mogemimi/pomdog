// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AudioClip;
class AudioEngine;
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads an audio clip into memory from a file.
/// Supports WAV and Ogg Vorbis formats.
/// @param fs The file system context used to locate the file.
/// @param audioEngine The audio engine for creating the audio clip.
/// @param filePath The virtual path to the audio file.
/// @return A tuple containing the loaded audio clip and an error object if an error occurred.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
loadAudioClip(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<AudioEngine>& audioEngine,
    std::string_view filePath) noexcept;

} // namespace pomdog

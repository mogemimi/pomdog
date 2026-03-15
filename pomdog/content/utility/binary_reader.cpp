// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/utility/binary_reader.h"

namespace pomdog::detail::BinaryReader {

[[nodiscard]] std::unique_ptr<Error>
seek(std::span<const u8>& bin, std::size_t offset) noexcept
{
    if (bin.size() < offset) {
        return errors::make("bin.size() must be >= offset");
    }
    bin = bin.subspan(offset);
    return nullptr;
}

} // namespace pomdog::detail::BinaryReader

// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard {

/// Keeps a clipboard inside the process, which other applications cannot see.
///
/// It suits tests, and applications that want copy and paste between their own
/// fields on platforms without a clipboard backend. It reads back what it last
/// wrote, and reports NoText before the first write.
class ClipboardServiceLocal final : public ClipboardService {
public:
    ClipboardServiceLocal() noexcept;

    ClipboardServiceLocal(const ClipboardServiceLocal&) = delete;
    ClipboardServiceLocal& operator=(const ClipboardServiceLocal&) = delete;

    ~ClipboardServiceLocal() override;

    [[nodiscard]] ClipboardCapabilities
    getCapabilities() const noexcept override;

    void
    update() override;

    [[nodiscard]] std::span<const ClipboardEvent>
    getEvents() const noexcept override;

    u64
    requestWriteText(std::string_view text) override;

    u64
    requestReadText() override;

private:
    std::vector<ClipboardEvent> pendingEvents_;
    std::vector<ClipboardEvent> events_;
    std::optional<std::string> text_;
    u64 nextRequestID_ = 1;
};

} // namespace pomdog::clipboard

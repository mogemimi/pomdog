// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/clipboard_service_local.h"
#include "pomdog/experimental/clipboard/line_breaks.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard {

ClipboardServiceLocal::ClipboardServiceLocal() noexcept = default;

ClipboardServiceLocal::~ClipboardServiceLocal() = default;

ClipboardCapabilities ClipboardServiceLocal::getCapabilities() const noexcept
{
    ClipboardCapabilities capabilities;
    capabilities.writeText = FeatureSupport::Available;
    capabilities.readText = FeatureSupport::Available;
    capabilities.pasteEvents = FeatureSupport::Unsupported;
    return capabilities;
}

void ClipboardServiceLocal::update()
{
    events_.clear();
    std::swap(events_, pendingEvents_);
}

std::span<const ClipboardEvent> ClipboardServiceLocal::getEvents() const noexcept
{
    return events_;
}

u64 ClipboardServiceLocal::requestWriteText(std::string_view text)
{
    std::string stored;
    detail::appendWithLF(text, stored);
    text_ = std::move(stored);

    ClipboardEvent event;
    event.kind = ClipboardEventKind::WriteCompleted;
    event.requestID = nextRequestID_++;
    event.status = ClipboardStatus::Succeeded;
    pendingEvents_.push_back(std::move(event));
    return pendingEvents_.back().requestID;
}

u64 ClipboardServiceLocal::requestReadText()
{
    ClipboardEvent event;
    event.kind = ClipboardEventKind::ReadCompleted;
    event.requestID = nextRequestID_++;
    if (text_) {
        event.status = ClipboardStatus::Succeeded;
        event.text = *text_;
    }
    else {
        event.status = ClipboardStatus::NoText;
    }
    pendingEvents_.push_back(std::move(event));
    return pendingEvents_.back().requestID;
}

} // namespace pomdog::clipboard

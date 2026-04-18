// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::cocoa {

KeyboardCocoa::KeyboardCocoa(const std::shared_ptr<KeyboardImpl>& impl)
    : impl_(impl)
{
    POMDOG_ASSERT(impl_ != nullptr);
}

void KeyboardCocoa::handleEvent(const SystemEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.kind) {
    case SystemEventKind::InputKeyEvent: {
        const auto ev = std::get<InputKeyEvent>(event.data);
        impl_->setKey(ev.key, ev.state);
        break;
    }
    case SystemEventKind::InputTextEvent: {
        const auto& ev = std::get<InputTextEvent>(event.data);
        impl_->appendTextInput(ev.text);
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::cocoa

source_group(signals REGULAR_EXPRESSION "pomdog/signals/*")

target_sources(pomdog_static PRIVATE
    # NOTE: signals
    ${POMDOG_SRC_DIR}/signals/connection.cpp
    ${POMDOG_SRC_DIR}/signals/connection.hpp
    ${POMDOG_SRC_DIR}/signals/connection_list.cpp
    ${POMDOG_SRC_DIR}/signals/connection_list.hpp
    ${POMDOG_SRC_DIR}/signals/delegate.hpp
    ${POMDOG_SRC_DIR}/signals/event_queue.hpp
    ${POMDOG_SRC_DIR}/signals/forward_declarations.hpp
    ${POMDOG_SRC_DIR}/signals/scoped_connection.cpp
    ${POMDOG_SRC_DIR}/signals/scoped_connection.hpp
    ${POMDOG_SRC_DIR}/signals/signal_helpers.hpp
    ${POMDOG_SRC_DIR}/signals/signal.hpp
    ${POMDOG_SRC_DIR}/signals/detail/delegate_body.hpp
    ${POMDOG_SRC_DIR}/signals/detail/signal_body.cpp
    ${POMDOG_SRC_DIR}/signals/detail/signal_body.hpp
)

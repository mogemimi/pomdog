source_group(signals REGULAR_EXPRESSION "pomdog/signals/*")

target_sources(pomdog_static PRIVATE
    # NOTE: signals
    ${POMDOG_SRC_DIR}/signals/connection.cpp
    ${POMDOG_SRC_DIR}/signals/connection.h
    ${POMDOG_SRC_DIR}/signals/connection_list.cpp
    ${POMDOG_SRC_DIR}/signals/connection_list.h
    ${POMDOG_SRC_DIR}/signals/delegate.h
    ${POMDOG_SRC_DIR}/signals/event_queue.h
    ${POMDOG_SRC_DIR}/signals/forward_declarations.h
    ${POMDOG_SRC_DIR}/signals/scoped_connection.cpp
    ${POMDOG_SRC_DIR}/signals/scoped_connection.h
    ${POMDOG_SRC_DIR}/signals/signal_helpers.h
    ${POMDOG_SRC_DIR}/signals/signal.h
    ${POMDOG_SRC_DIR}/signals/detail/delegate_body.h
    ${POMDOG_SRC_DIR}/signals/detail/signal_body.cpp
    ${POMDOG_SRC_DIR}/signals/detail/signal_body.h
)

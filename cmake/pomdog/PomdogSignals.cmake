target_sources(pomdog_static PRIVATE
    # NOTE: Signals
    ${POMDOG_INC_DIR}/Signals/Connection.hpp
    ${POMDOG_SRC_DIR}/Signals/Connection.cpp
    ${POMDOG_INC_DIR}/Signals/ConnectionList.hpp
    ${POMDOG_SRC_DIR}/Signals/ConnectionList.cpp
    ${POMDOG_INC_DIR}/Signals/Delegate.hpp
    ${POMDOG_INC_DIR}/Signals/Event.hpp
    ${POMDOG_INC_DIR}/Signals/EventQueue.hpp
    ${POMDOG_SRC_DIR}/Signals/EventQueue.cpp
    ${POMDOG_INC_DIR}/Signals/Helpers.hpp
    ${POMDOG_INC_DIR}/Signals/ScopedConnection.hpp
    ${POMDOG_SRC_DIR}/Signals/ScopedConnection.cpp
    ${POMDOG_INC_DIR}/Signals/Signal.hpp
    ${POMDOG_INC_DIR}/Signals/detail/DelegateBody.hpp
    ${POMDOG_INC_DIR}/Signals/detail/EventBody.hpp
    ${POMDOG_INC_DIR}/Signals/detail/ForwardDeclarations.hpp
    ${POMDOG_INC_DIR}/Signals/detail/SignalBody.hpp
)

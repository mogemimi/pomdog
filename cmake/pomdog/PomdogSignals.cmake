source_group(Signals REGULAR_EXPRESSION "(include/Pomdog|src)/Signals/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Signals
    ${POMDOG_INC_DIR}/Signals/Connection.hpp
    ${POMDOG_SRC_DIR}/Signals/Connection.cpp
    ${POMDOG_INC_DIR}/Signals/ConnectionList.hpp
    ${POMDOG_SRC_DIR}/Signals/ConnectionList.cpp
    ${POMDOG_INC_DIR}/Signals/Delegate.hpp
    ${POMDOG_INC_DIR}/Signals/EventQueue.hpp
    ${POMDOG_INC_DIR}/Signals/ForwardDeclarations.hpp
    ${POMDOG_INC_DIR}/Signals/Helpers.hpp
    ${POMDOG_INC_DIR}/Signals/ScopedConnection.hpp
    ${POMDOG_SRC_DIR}/Signals/ScopedConnection.cpp
    ${POMDOG_INC_DIR}/Signals/Signal.hpp
    ${POMDOG_INC_DIR}/Signals/Detail/DelegateBody.hpp
    ${POMDOG_INC_DIR}/Signals/Detail/SignalBody.hpp
    ${POMDOG_SRC_DIR}/Signals/Detail/SignalBody.cpp
)

target_sources(pomdog_static PRIVATE
    # NOTE: InputSystem
    ${POMDOG_SRC_DIR}/InputSystem/GamepadFactory.cpp
    ${POMDOG_SRC_DIR}/InputSystem/GamepadFactory.hpp
    ${POMDOG_SRC_DIR}/InputSystem/GamepadHelper.cpp
    ${POMDOG_SRC_DIR}/InputSystem/GamepadHelper.hpp
    ${POMDOG_SRC_DIR}/InputSystem/GamepadMappings.cpp
    ${POMDOG_SRC_DIR}/InputSystem/GamepadMappings.hpp
    ${POMDOG_SRC_DIR}/InputSystem/NativeGamepad.hpp

    $<$<PLATFORM_ID:Linux>:
        # NOTE: InputSystem.Linux
        ${POMDOG_SRC_DIR}/InputSystem.Linux/GamepadLinux.cpp
        ${POMDOG_SRC_DIR}/InputSystem.Linux/GamepadLinux.hpp
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: InputSystem.IOKit
        ${POMDOG_SRC_DIR}/InputSystem.IOKit/GamepadIOKit.cpp
        ${POMDOG_SRC_DIR}/InputSystem.IOKit/GamepadIOKit.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: InputSystem.DirectInput
        ${POMDOG_SRC_DIR}/InputSystem.DirectInput/GamepadDirectInput.cpp
        ${POMDOG_SRC_DIR}/InputSystem.DirectInput/GamepadDirectInput.hpp
        ${POMDOG_SRC_DIR}/InputSystem.DirectInput/PrerequisitesDirectInput.hpp
    >
)

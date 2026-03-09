source_group(vfs REGULAR_EXPRESSION "pomdog/vfs/*")

target_sources(pomdog_static PRIVATE
    # NOTE: vfs
    ${POMDOG_SRC_DIR}/vfs/file.h
    ${POMDOG_SRC_DIR}/vfs/file.cpp
    ${POMDOG_SRC_DIR}/vfs/file_archive.h
    ${POMDOG_SRC_DIR}/vfs/file_archive.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        ${POMDOG_SRC_DIR}/vfs/file_archive_mmap.h
        ${POMDOG_SRC_DIR}/vfs/file_archive_mmap.cpp
    >
    ${POMDOG_SRC_DIR}/vfs/file_physical.h
    ${POMDOG_SRC_DIR}/vfs/file_physical.cpp
    ${POMDOG_SRC_DIR}/vfs/file_system.h
    ${POMDOG_SRC_DIR}/vfs/file_system.cpp
    ${POMDOG_SRC_DIR}/vfs/mount_volume.h
    ${POMDOG_SRC_DIR}/vfs/mount_volume.cpp
)

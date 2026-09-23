# Copy AddressSanitizer runtime files to the target's output directory.
#
# Usage:
#   include(${POMDOG_DIR}/cmake/pomdog/asan.cmake)
#   pomdog_copy_asan_runtime(my_target)

function(pomdog_copy_asan_runtime target)
    if(NOT POMDOG_USE_ADDRESS_SANITIZER)
        return()
    endif()
    if(NOT MSVC)
        return()
    endif()

    cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH _asan_runtime_dir)

    set(_asan_files
        clang_rt.asan_dynamic-x86_64.dll
        clang_rt.asan_dbg_dynamic-x86_64.dll
        llvm-symbolizer.exe
    )

    foreach(_file IN LISTS _asan_files)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_asan_runtime_dir}/${_file}"
                "$<TARGET_FILE_DIR:${target}>"
            COMMENT "Copying ${_file} for AddressSanitizer"
        )
    endforeach()
endfunction()

cmake_minimum_required(VERSION 3.16)
project(Zuma LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(zuma
    main.cpp
    src/zuma.cpp
)

target_include_directories(zuma PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/incl"
)

if(MSVC)
    target_compile_options(zuma PRIVATE /utf-8)
endif()

# Keep the level files available when the program is started from the
# repository root, the build root, or the executable's own directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/config"
     DESTINATION "${CMAKE_BINARY_DIR}")

add_custom_command(TARGET zuma POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "$<TARGET_FILE_DIR:zuma>/config"
    COMMENT "Copying level files next to the executable"
)

option(ZUMA_BUILD_GUI "Build the raylib GUI" OFF)

if(ZUMA_BUILD_GUI)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    add_subdirectory(third_party/raylib EXCLUDE_FROM_ALL)

    add_executable(zuma_gui
        zuma_gui.cpp
        src/zuma.cpp
    )

    target_include_directories(zuma_gui PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/incl"
    )

    target_compile_features(zuma_gui PRIVATE cxx_std_17)
    target_link_libraries(zuma_gui PRIVATE raylib)

    if(WIN32)
        target_link_libraries(zuma_gui PRIVATE winmm gdi32 opengl32)
    endif()

    add_custom_command(TARGET zuma_gui POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/config"
            "$<TARGET_FILE_DIR:zuma_gui>/config"
    )
endif()
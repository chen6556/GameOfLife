cmake_minimum_required(VERSION 3.20.0)
project(GameOfLife VERSION 0.2.4)

include(CTest)
enable_testing()

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("/arch:AVX2")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

add_subdirectory(src)
include_directories(inc)

qt_add_executable(GameOfLife
    MANUAL_FINALIZATION
    src/main.cpp
    inc/MainWindow.hpp
    
    ${_SOURCES}
)

target_link_libraries(GameOfLife PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)

set_target_properties(GameOfLife PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(GameOfLife)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
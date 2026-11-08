cmake_minimum_required(VERSION 3.20)

project(real_copy_rust VERSION 6.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The original project relied on this environment variable for the vcpkg
# include directory.  It is kept so existing build scripts keep working.
if(DEFINED ENV{INCLUDE_VCPKG_for_me})
    include_directories($ENV{INCLUDE_VCPKG_for_me})
endif()

if(MSVC)
    add_compile_options(/permissive- /EHsc /utf-8 /W3)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
endif()

find_package(nlohmann_json CONFIG REQUIRED)
find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System)

# ---------------------------------------------------------------------------
#  Core simulation (no SFML, no console UI).  This is what the tests exercise.
# ---------------------------------------------------------------------------

add_library(copy_rust_core STATIC
    src/core/GameTypes.cpp
    src/core/Game.cpp
    src/coordinates_rust/Coordinates_rust.cpp
    src/coordinates_rust/loc_coordin.cpp
)

target_include_directories(copy_rust_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(copy_rust_core PUBLIC nlohmann_json::nlohmann_json)

# ---------------------------------------------------------------------------
#  Shared localization used by both front-ends.
# ---------------------------------------------------------------------------

add_library(copy_rust_text STATIC
    src/ui/Localization.cpp
)
target_include_directories(copy_rust_text PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---------------------------------------------------------------------------
#  SFML 3 front-end (the main game).
# ---------------------------------------------------------------------------

add_executable(real_copy_rust
    src/sfml_frontend/main.cpp
    src/sfml_frontend/SfmlApp.cpp
    src/sfml_frontend/WorldRenderer.cpp
    src/sfml_frontend/Ui.cpp
)

target_link_libraries(real_copy_rust PRIVATE
    copy_rust_core
    copy_rust_text
    SFML::Graphics
)

# ---------------------------------------------------------------------------
#  Console front-end (keeps the project usable on machines without a window).
# ---------------------------------------------------------------------------

add_executable(copy_rust_console
    src/console_frontend/main_console.cpp
    src/console_frontend/ConsoleApp.cpp
)

target_link_libraries(copy_rust_console PRIVATE
    copy_rust_core
    copy_rust_text
)

# ---------------------------------------------------------------------------
#  Tests
# ---------------------------------------------------------------------------

enable_testing()

add_executable(copy_rust_tests
    tests/test_core.cpp
)

target_link_libraries(copy_rust_tests PRIVATE copy_rust_core)

add_test(NAME core_tests COMMAND copy_rust_tests)
set_tests_properties(core_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    TIMEOUT 120
)

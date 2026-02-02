cmake_minimum_required(VERSION 3.20)

# =============================================================================
# Cross-compilation setup (MUST be before project())
# =============================================================================
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET_TRIPLE "x86_64-w64-mingw32")
set(MINGW_ROOT "/usr/x86_64-w64-mingw32" CACHE PATH "MinGW sysroot")

project(peb_walking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# Build type option: cmake -DDEBUG_BUILD=ON ..
# =============================================================================
option(DEBUG_BUILD "Build with debug symbols and stdlib support" OFF)

# =============================================================================
# Paths
# =============================================================================
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(PROJECT_SRC ${PROJECT_ROOT}/src)
set(NEUTRINO_ROOT ${PROJECT_ROOT}/..)

# =============================================================================
# Compiler flags
# =============================================================================
add_compile_options(
    --target=${TARGET_TRIPLE}
    -Wall
    -Wextra
)

if(DEBUG_BUILD)
    add_compile_options(-g -O0)
    add_compile_definitions(_DEBUG=1)
else()
    add_compile_options(-O2 -fno-builtin -ffreestanding)
    add_compile_definitions(_DEBUG=0)
endif()

add_compile_definitions(JM_XORSTR_HPP)

# =============================================================================
# Include directories
# =============================================================================
include_directories(
    # Public headers
    ${NEUTRINO_ROOT}/include
    # Private headers (local src directories)
    ${PROJECT_SRC}
    ${NEUTRINO_ROOT}/commons/src
)

# =============================================================================
# Linker flags
# =============================================================================
add_link_options(
    -Wl,-e,main
    --target=${TARGET_TRIPLE}
    -fuse-ld=lld
    -L${MINGW_ROOT}/lib
)

if(NOT DEBUG_BUILD)
    add_link_options(-nostdlib -nostartfiles)
endif()

# =============================================================================
# library setup
# =============================================================================
if(DEBUG_BUILD)
    set(LIBS -lmsvcrt -lkernel32 -lntdll)
else()
    set(LIBS -lmsvcrt -lkernel32 -lntdll)
endif()

# =============================================================================
# Base sources (shared by most tests)
# =============================================================================
file(GLOB BASE_SOURCES
    ${PROJECT_SRC}/*.cpp
    ${NEUTRINO_ROOT}/commons/src/*.cpp
)

# Force C++ compilation
set_source_files_properties(${BASE_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(peb_walking ${BASE_SOURCES})
target_link_libraries(peb_walking PRIVATE ${LIBS})

if(DEBUG_BUILD)
    set(OUTPUT_SUFFIX "_debug")
else()
    set(OUTPUT_SUFFIX "_release")
endif()

set_target_properties(peb_walking PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "peb_walking${OUTPUT_SUFFIX}"
    SUFFIX ".exe"
)

cmake_minimum_required(VERSION 3.20)

# =============================================================================
# Cross-compilation setup (MUST be before project())
# =============================================================================
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CXX_COMPILER /opt/Polaris-Obfuscator/src/build/bin/clang++ -mllvm -passes=fla,mba,indcall,bcf,sub)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET_TRIPLE "x86_64-w64-mingw32")
set(MINGW_ROOT "/usr/x86_64-w64-mingw32" CACHE PATH "MinGW sysroot")

project(anti_emulation LANGUAGES CXX)

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
    -ffunction-sections
    -fdata-sections
    -mno-stack-arg-probe
    -fno-asynchronous-unwind-tables
    -fPIC
)

if(DEBUG_BUILD)
    add_compile_options(-g -O0 -fno-exceptions -fno-rtti)
    add_compile_definitions(_DEBUG=1)
else()
    add_compile_options(-O0 -fno-builtin -ffreestanding -fno-ident -fno-exceptions -fno-rtti)
    add_compile_definitions(_DEBUG=0)
endif()

add_compile_definitions(JM_XORSTR_DISABLE_AVX_INTRINSICS)

# =============================================================================
# Include directories
# =============================================================================
include_directories(
    # Public headers
    ${NEUTRINO_ROOT}/include
    # Private headers (local src directories)
    ${PROJECT_SRC}
    ${NEUTRINO_ROOT}/commons/src
    # Internal headers (not part of public API)
    ${NEUTRINO_ROOT}/commons/include
)

# =============================================================================
# Linker flags
# =============================================================================
add_link_options(
    -Wl,-e,main
    --target=${TARGET_TRIPLE}
    -fuse-ld=lld
    -L${MINGW_ROOT}/lib
    -Wl,--gc-sections
    -Wl,--no-seh
    -Wl,--enable-stdcall-fixup
)

if(NOT DEBUG_BUILD)
    add_link_options(-nostdlib -nostartfiles -s -Wl,--build-id=none)
endif()

# =============================================================================
# library setup
# =============================================================================
if(DEBUG_BUILD)
    set(LIBS -lmsvcrt -lkernel32 -lntdll)
else()
    # Same libs as debug, but with nostdlib (order matters!)
    set(LIBS -lkernel32 -lntdll)
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

add_executable(anti_emulation ${BASE_SOURCES})
target_link_libraries(anti_emulation PRIVATE ${LIBS})

if(DEBUG_BUILD)
    set(OUTPUT_SUFFIX "_debug")
else()
    set(OUTPUT_SUFFIX "_release")
endif()

set_target_properties(anti_emulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "anti_emulation${OUTPUT_SUFFIX}"
    SUFFIX ".exe"
)

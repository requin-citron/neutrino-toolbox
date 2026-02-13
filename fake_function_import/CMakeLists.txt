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

project(fake_function_import LANGUAGES CXX)

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
    add_compile_options(-g -O1 -fno-exceptions -fno-rtti)
    add_compile_definitions(_DEBUG=1)
else()
    add_compile_options(-O1 -fno-builtin -ffreestanding -fno-ident -fno-exceptions -fno-rtti)
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
# Base sources (shared by all profiles)
# =============================================================================
file(GLOB COMMON_SOURCES
    ${NEUTRINO_ROOT}/commons/src/*.cpp
)

set(CORE_SOURCES
    ${PROJECT_SRC}/main.cpp
    ${PROJECT_SRC}/fake_function_import.cpp
)

# Force C++ compilation
set_source_files_properties(${COMMON_SOURCES} ${CORE_SOURCES} PROPERTIES LANGUAGE CXX)

if(DEBUG_BUILD)
    set(OUTPUT_SUFFIX "_debug")
else()
    set(OUTPUT_SUFFIX "_release")
endif()

# =============================================================================
# Profile 1: DOCUMENT (Document processor - GDI, COM, printing)
# =============================================================================
add_executable(fake_import_DOCUMENT
    ${CORE_SOURCES}
    ${COMMON_SOURCES}
    ${PROJECT_SRC}/profiles/profile_document.cpp
)

target_compile_definitions(fake_import_DOCUMENT PRIVATE PROFILE_DOCUMENT=1)

target_link_libraries(fake_import_DOCUMENT PRIVATE
    ${LIBS}
    -lgdi32
    -lcomdlg32
    -lwinspool
    -lole32
    -luser32
)

set_target_properties(fake_import_DOCUMENT PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "fake_import_DOCUMENT${OUTPUT_SUFFIX}"
    SUFFIX ".exe"
)

# =============================================================================
# Profile 2: NETWORK (Network client - HTTP, sockets, crypto)
# =============================================================================
add_executable(fake_import_NETWORK
    ${CORE_SOURCES}
    ${COMMON_SOURCES}
    ${PROJECT_SRC}/profiles/profile_network.cpp
)

target_compile_definitions(fake_import_NETWORK PRIVATE PROFILE_NETWORK=1)

target_link_libraries(fake_import_NETWORK PRIVATE
    ${LIBS}
    -lwinhttp
    -lws2_32
    -lcrypt32
    -lbcrypt
    -lwininet
    -ldnsapi
)

set_target_properties(fake_import_NETWORK PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "fake_import_NETWORK${OUTPUT_SUFFIX}"
    SUFFIX ".exe"
)

# =============================================================================
# Profile 3: SYSTEM (System utility - registry, services, processes)
# =============================================================================
add_executable(fake_import_SYSTEM
    ${CORE_SOURCES}
    ${COMMON_SOURCES}
    ${PROJECT_SRC}/profiles/profile_system.cpp
)

target_compile_definitions(fake_import_SYSTEM PRIVATE PROFILE_SYSTEM=1)

target_link_libraries(fake_import_SYSTEM PRIVATE
    ${LIBS}
    -ladvapi32
    -lpsapi
    -lwtsapi32
    -luserenv
    -lnetapi32
)

set_target_properties(fake_import_SYSTEM PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "fake_import_SYSTEM${OUTPUT_SUFFIX}"
    SUFFIX ".exe"
)

# =============================================================================
# Profile 4: EXPLORER (File browser with cloud sync/update checker)
# =============================================================================
add_executable(fake_import_EXPLORER
    ${CORE_SOURCES}
    ${COMMON_SOURCES}
    ${PROJECT_SRC}/profiles/profile_explorer.cpp
)

target_compile_definitions(fake_import_EXPLORER PRIVATE PROFILE_EXPLORER=1)

target_link_libraries(fake_import_EXPLORER PRIVATE
    ${LIBS}
    -lshell32
    -lshlwapi
    -luser32
    -lwinhttp
)

set_target_properties(fake_import_EXPLORER PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "fake_import_EXPLORER${OUTPUT_SUFFIX}"
    SUFFIX ".exe"
)

# =============================================================================
# Build summary
# =============================================================================
message(STATUS "===== Fake Function Import Build Configuration =====")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Debug build: ${DEBUG_BUILD}")
message(STATUS "Output suffix: ${OUTPUT_SUFFIX}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  - fake_import_DOCUMENT${OUTPUT_SUFFIX}.exe")
message(STATUS "  - fake_import_NETWORK${OUTPUT_SUFFIX}.exe")
message(STATUS "  - fake_import_SYSTEM${OUTPUT_SUFFIX}.exe")
message(STATUS "  - fake_import_EXPLORER${OUTPUT_SUFFIX}.exe")
message(STATUS "=======================================================")

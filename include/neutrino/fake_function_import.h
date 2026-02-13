/**
 * @file fake_function_import.h
 * @brief Public API for fake function import system
 *
 * Provides static WinAPI imports for credible IAT (Import Address Table) generation.
 * Profile is selected at compile-time via CMake defines.
 *
 * Usage in other projects:
 *   #include "neutrino/fake_function_import.h"
 *   execute_fake_imports();  // Call early in program startup
 *
 * Build integration (CMake):
 *   1. Add to include path: ${NEUTRINO_ROOT}/include
 *   2. Link profile object file or build with profile sources
 *   3. Define profile: -DPROFILE_DOCUMENT=1 (or NETWORK, SYSTEM, EXPLORER)
 *   4. Link required DLLs for selected profile (see below)
 *
 * Available Profiles:
 *
 *   PROFILE_DOCUMENT - Document processor application
 *     Story: PDF viewer, document editor with GDI rendering and printing
 *     DLLs: gdi32, comdlg32, winspool, ole32, user32
 *     Imports: ~27 static imports
 *     Suspicion: Low (very common application type)
 *     Wine: Stable
 *
 *   PROFILE_NETWORK - Network client application
 *     Story: HTTP client with cryptographic operations
 *     DLLs: winhttp, ws2_32, crypt32, bcrypt, wininet, dnsapi
 *     Imports: ~30+ static imports
 *     Suspicion: Low-Medium (legitimate network app)
 *     Wine: Stable
 *
 *   PROFILE_SYSTEM - System administration utility
 *     Story: System management tool for registry and process monitoring
 *     DLLs: advapi32, psapi, wtsapi32, userenv, netapi32
 *     Imports: ~25+ static imports
 *     Suspicion: Medium (admin tools, but legitimate)
 *     Wine: Stable
 *
 *   PROFILE_EXPLORER - Modern file browser with cloud sync
 *     Story: File explorer with cloud synchronization and update checker
 *     DLLs: shell32, shlwapi, user32, winhttp
 *     Imports: ~25-28 static imports
 *     Suspicion: VERY LOW (ultra-banal operations, OneDrive/Dropbox pattern)
 *     Wine: Stable
 *     Recommended: Best for EDR evasion (zero suspicion)
 *
 * CMake Integration Example:
 *
 *   # Option 1: Link prebuilt profile object
 *   target_link_libraries(your_project PRIVATE
 *       ${NEUTRINO_ROOT}/fake_function_import/build/CMakeFiles/fake_import_EXPLORER.dir/src/profiles/profile_explorer.cpp.obj
 *       ${NEUTRINO_ROOT}/fake_function_import/build/CMakeFiles/fake_import_EXPLORER.dir/src/fake_function_import.cpp.obj
 *       -lshell32 -lshlwapi -luser32 -lwinhttp  # For EXPLORER profile
 *   )
 *
 *   # Option 2: Build from source
 *   set(FAKE_IMPORT_SOURCES
 *       ${NEUTRINO_ROOT}/fake_function_import/src/fake_function_import.cpp
 *       ${NEUTRINO_ROOT}/fake_function_import/src/profiles/profile_explorer.cpp
 *   )
 *   target_sources(your_project PRIVATE ${FAKE_IMPORT_SOURCES})
 *   target_compile_definitions(your_project PRIVATE PROFILE_EXPLORER=1)
 *   target_link_libraries(your_project PRIVATE -lshell32 -lshlwapi -luser32 -lwinhttp)
 *
 * Runtime Overhead:
 *   - DOCUMENT: ~50-100ms
 *   - NETWORK: ~100-200ms (network timeouts possible)
 *   - SYSTEM: ~50-150ms
 *   - EXPLORER: ~50-100ms (instant if HTTP times out)
 *
 * Security Notes:
 *   - All operations use safe parameters (no destructive actions)
 *   - Silent error handling (continues on failure)
 *   - Obfuscated with Polaris annotations (bcf, mba, substitution)
 *   - Read-only operations (EXPLORER profile)
 *   - No crypto/hash operations in EXPLORER (zero EDR suspicion)
 */

#ifndef NEUTRINO_FAKE_FUNCTION_IMPORT_H
#define NEUTRINO_FAKE_FUNCTION_IMPORT_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Execute fake function imports for the selected profile
 *
 * This function calls Windows API functions with safe parameters to:
 * 1. Ensure static imports appear in the IAT for static analysis
 * 2. Create credible behavioral patterns for dynamic analysis
 * 3. Avoid triggering suspicious activity (all params are safe)
 *
 * Profile selection is compile-time via CMake defines:
 *   - PROFILE_DOCUMENT=1 → Document processing application
 *   - PROFILE_NETWORK=1 → Network client application
 *   - PROFILE_SYSTEM=1 → System administration utility
 *   - PROFILE_EXPLORER=1 → File browser with cloud sync (RECOMMENDED)
 *
 * Execution strategy: EXEC_FULL
 *   - All imported functions are called with safe parameters
 *   - Categories: Queries, Init/Cleanup pairs, Handle-based ops, Safe failures
 *   - Overhead: ~50-200ms depending on profile
 *   - Silent error handling (continues on failure)
 *
 * @note Call this function early in program execution (e.g., main startup)
 * @note Obfuscated with Polaris annotations for control flow protection
 * @note PROFILE_EXPLORER recommended for minimal EDR suspicion
 *
 * @example
 *   int main() {
 *       execute_fake_imports();  // Build credible IAT and behavioral pattern
 *       // ... your actual code here
 *       return 0;
 *   }
 */
VOID execute_fake_imports();

#ifdef __cplusplus
}
#endif

#endif // NEUTRINO_FAKE_FUNCTION_IMPORT_H

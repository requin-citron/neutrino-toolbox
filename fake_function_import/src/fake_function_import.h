/**
 * @file fake_function_import.h
 * @brief Public API for fake function import system
 *
 * Provides static WinAPI imports for credible IAT (Import Address Table) generation.
 * Profile is selected at compile-time via CMake defines.
 *
 * Usage:
 *   #include "fake_function_import.h"
 *   execute_fake_imports();  // Executes fake imports for selected profile
 *
 * Profiles:
 *   - PROFILE_DOCUMENT: Document processor (GDI, COM, printing)
 *   - PROFILE_NETWORK: Network client (HTTP, sockets, crypto)
 *   - PROFILE_SYSTEM: System utility (registry, services, processes)
 */

#ifndef FAKE_FUNCTION_IMPORT_H
#define FAKE_FUNCTION_IMPORT_H

#include "main.h"

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
 * Profile selection is compile-time via CMake:
 *   - PROFILE_DOCUMENT=1 → Document processing application
 *   - PROFILE_NETWORK=1 → Network client application
 *   - PROFILE_SYSTEM=1 → System administration utility
 *
 * Execution strategy: EXEC_FULL
 *   - All imported functions are called with safe parameters
 *   - Categories: Queries, Init/Cleanup pairs, Handle-based ops, Safe failures
 *   - Overhead: ~50-200ms depending on profile
 *   - Silent error handling (continues on failure)
 *
 * @note This function should be called early in program execution (e.g., main startup)
 * @note Obfuscated with Polaris annotations (bcf, mba, substitution)
 */
VOID execute_fake_imports();

#ifdef __cplusplus
}
#endif

#endif // FAKE_FUNCTION_IMPORT_H

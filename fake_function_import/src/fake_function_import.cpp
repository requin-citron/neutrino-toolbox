/**
 * @file fake_function_import.cpp
 * @brief Dispatcher for fake function import system
 *
 * Routes execute_fake_imports() call to the appropriate profile execution function
 * based on compile-time profile selection (PROFILE_DOCUMENT, PROFILE_NETWORK, etc.)
 */

#include "fake_function_import.h"

// ============================================================================
// Forward declarations for profile execution functions
// ============================================================================

#ifdef PROFILE_DOCUMENT
extern VOID execute_document_profile();
#endif

#ifdef PROFILE_NETWORK
extern VOID execute_network_profile();
#endif

#ifdef PROFILE_SYSTEM
extern VOID execute_system_profile();
#endif

#ifdef PROFILE_MEDIA
extern VOID execute_media_profile();
#endif

#ifdef PROFILE_EXPLORER
extern VOID execute_explorer_profile();
#endif

// ============================================================================
// Public API implementation
// ============================================================================

/**
 * @brief Execute fake imports for the selected profile
 *
 * Compile-time dispatcher that calls the appropriate profile execution function.
 * The profile is selected via CMake defines (PROFILE_DOCUMENT, PROFILE_NETWORK, etc.)
 *
 * Execution is obfuscated with Polaris annotations and uses safe parameters throughout.
 */
__attribute__((__annotate__(("substitution"))))
VOID execute_fake_imports() {
#ifdef PROFILE_DOCUMENT
    // Execute document processing profile
    _dbg("Executing PROFILE_DOCUMENT fake imports");
    execute_document_profile();

#elif defined(PROFILE_NETWORK)
    // Execute network client profile
    _dbg("Executing PROFILE_NETWORK fake imports");
    execute_network_profile();

#elif defined(PROFILE_SYSTEM)
    // Execute system utility profile
    _dbg("Executing PROFILE_SYSTEM fake imports");
    execute_system_profile();

#elif defined(PROFILE_MEDIA)
    // Execute media application profile
    _dbg("Executing PROFILE_MEDIA fake imports");
    execute_media_profile();

#elif defined(PROFILE_EXPLORER)
    // Execute file explorer profile
    _dbg("Executing PROFILE_EXPLORER fake imports");
    execute_explorer_profile();

#else
    // No profile selected - compile error
    #error "No profile selected. Define one of: PROFILE_DOCUMENT, PROFILE_NETWORK, PROFILE_SYSTEM, PROFILE_MEDIA, PROFILE_EXPLORER"
#endif

    _dbg("Fake imports execution completed");
}

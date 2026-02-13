/**
 * @file profile_document.cpp
 * @brief PROFILE_DOCUMENT - Document processor application profile
 *
 * Story: PDF viewer, document editor with GDI rendering, printing, and COM support
 * DLLs: gdi32.dll, comdlg32.dll, winspool.drv, ole32.dll, user32.dll
 * ~27 static imports for credible IAT
 */

#include "../main.h"

// ============================================================================
// Linker directives - Link required DLLs for document processing profile
// ============================================================================
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "user32.lib")

// ============================================================================
// Static imports - These will appear in the IAT
// ============================================================================

extern "C" {
    // GDI32.DLL - Graphics rendering for document display
    __declspec(dllimport) HDC WINAPI CreateCompatibleDC(HDC hdc);
    __declspec(dllimport) HBITMAP WINAPI CreateCompatibleBitmap(HDC hdc, int cx, int cy);
    __declspec(dllimport) BOOL WINAPI BitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
    __declspec(dllimport) HGDIOBJ WINAPI SelectObject(HDC hdc, HGDIOBJ h);
    __declspec(dllimport) BOOL WINAPI DeleteDC(HDC hdc);
    __declspec(dllimport) BOOL WINAPI TextOutW(HDC hdc, int x, int y, LPCWSTR lpString, int c);
    __declspec(dllimport) COLORREF WINAPI SetTextColor(HDC hdc, COLORREF color);
    __declspec(dllimport) HFONT WINAPI CreateFontW(int cHeight, int cWidth, int cEscapement, int cOrientation,
                                                     int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut,
                                                     DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision,
                                                     DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName);
    __declspec(dllimport) int WINAPI GetDeviceCaps(HDC hdc, int index);
    __declspec(dllimport) int WINAPI SetBkMode(HDC hdc, int mode);
    __declspec(dllimport) HPEN WINAPI CreatePen(int iStyle, int cWidth, COLORREF color);
    __declspec(dllimport) HBRUSH WINAPI CreateSolidBrush(COLORREF color);
    __declspec(dllimport) BOOL WINAPI Rectangle(HDC hdc, int left, int top, int right, int bottom);
    __declspec(dllimport) BOOL WINAPI Ellipse(HDC hdc, int left, int top, int right, int bottom);
    __declspec(dllimport) COLORREF WINAPI SetBkColor(HDC hdc, COLORREF color);
    __declspec(dllimport) BOOL WINAPI DeleteObject(HGDIOBJ ho);

    // COMDLG32.DLL - File open/save dialogs for document operations
    __declspec(dllimport) BOOL WINAPI GetOpenFileNameW(LPOPENFILENAMEW lpofn);
    __declspec(dllimport) BOOL WINAPI GetSaveFileNameW(LPOPENFILENAMEW lpofn);
    __declspec(dllimport) DWORD WINAPI CommDlgExtendedError(void);

    // USER32.DLL - Window management and UI
    __declspec(dllimport) HDC WINAPI GetDC(HWND hWnd);
    __declspec(dllimport) int WINAPI ReleaseDC(HWND hWnd, HDC hDC);
    __declspec(dllimport) int WINAPI GetSystemMetrics(int nIndex);
    __declspec(dllimport) BOOL WINAPI GetCursorPos(LPPOINT lpPoint);
    __declspec(dllimport) BOOL WINAPI ClientToScreen(HWND hWnd, LPPOINT lpPoint);
    __declspec(dllimport) BOOL WINAPI GetWindowRect(HWND hWnd, LPRECT lpRect);

    // WINSPOOL.DRV - Printing support
    __declspec(dllimport) BOOL WINAPI OpenPrinterW(LPWSTR pPrinterName, LPHANDLE phPrinter, LPPRINTER_DEFAULTSW pDefault);
    __declspec(dllimport) BOOL WINAPI ClosePrinter(HANDLE hPrinter);
    __declspec(dllimport) BOOL WINAPI EnumPrintersW(DWORD Flags, LPWSTR Name, DWORD Level, LPBYTE pPrinterEnum,
                                                     DWORD cbBuf, LPDWORD pcbNeeded, LPDWORD pcReturned);

    // OLE32.DLL - COM support for OLE documents
    __declspec(dllimport) HRESULT WINAPI CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
    __declspec(dllimport) void WINAPI CoUninitialize(void);
    __declspec(dllimport) HRESULT WINAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext,
                                                           REFIID riid, LPVOID *ppv);
}

// ============================================================================
// Execution logic - EXEC_FULL strategy with safe parameters
// ============================================================================

/**
 * @brief Execute document profile fake imports
 *
 * Categories executed:
 * 1. Queries (no side-effects): GetDeviceCaps, GetSystemMetrics
 * 2. Init/Cleanup pairs: CoInitializeEx/CoUninitialize
 * 3. Handle-based: GetDC/ReleaseDC, CreateDC/DeleteDC
 * 4. Safe failures: EnumPrinters with NULL buffer (expected failure)
 *
 * Obfuscated with Polaris annotations for control flow flattening
 */
__attribute__((__annotate__(("substitution,bcf"))))
VOID execute_document_profile() {
    // ========================================================================
    // Category 1: Query functions (read-only, no side-effects)
    // ========================================================================

    // Get screen device capabilities
    HDC hScreenDC = GetDC(NULL);
    if (hScreenDC) {
        // Obfuscated constant calculation
        int horizIndex = (0x4 << 1);  // HORZRES = 8
        int vertIndex = (0x5 << 1);   // VERTRES = 10

        int screenWidth = GetDeviceCaps(hScreenDC, horizIndex);
        int screenHeight = GetDeviceCaps(hScreenDC, vertIndex);

        // Get system metrics (safe queries)
        int smCxScreen = GetSystemMetrics((0x0));  // SM_CXSCREEN
        int smCyScreen = GetSystemMetrics((0x1));  // SM_CYSCREEN

        ReleaseDC(NULL, hScreenDC);
    }

    // ========================================================================
    // Category 2: Init/Cleanup pairs (COM initialization)
    // ========================================================================

    // Initialize COM for OLE document support
    DWORD coInit = (0x2 | 0x0);  // COINIT_APARTMENTTHREADED = 0x2
    HRESULT hrCom = CoInitializeEx(NULL, coInit);
    if (SUCCEEDED(hrCom)) {
        // COM initialized successfully, cleanup immediately
        CoUninitialize();
    }

    // ========================================================================
    // Category 3: Handle-based operations (create, query, cleanup)
    // ========================================================================

    // Create compatible DC for rendering operations
    HDC hdc = GetDC(NULL);
    if (hdc) {
        HDC memDC = CreateCompatibleDC(hdc);
        if (memDC) {
            // Create GDI objects for document rendering
            int width = (0x64);   // 100 pixels
            int height = (0x64);  // 100 pixels

            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
            if (hBitmap) {
                HGDIOBJ oldBitmap = SelectObject(memDC, hBitmap);

                // Set text rendering properties
                COLORREF textColor = RGB(0x0, 0x0, 0x0);  // Black
                COLORREF bgColor = RGB(0xFF, 0xFF, 0xFF); // White
                SetTextColor(memDC, textColor);
                SetBkColor(memDC, bgColor);
                SetBkMode(memDC, (0x1));  // TRANSPARENT

                // Create pen and brush for drawing
                HPEN hPen = CreatePen((0x0), (0x1), RGB(0x0, 0x0, 0x0));  // PS_SOLID
                HBRUSH hBrush = CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));

                if (hPen && hBrush) {
                    HGDIOBJ oldPen = SelectObject(memDC, hPen);
                    HGDIOBJ oldBrush = SelectObject(memDC, hBrush);

                    // Draw shapes (document rendering simulation)
                    Rectangle(memDC, 10, 10, 50, 50);
                    Ellipse(memDC, 60, 10, 90, 40);

                    // Restore and cleanup
                    SelectObject(memDC, oldPen);
                    SelectObject(memDC, oldBrush);
                    DeleteObject(hPen);
                    DeleteObject(hBrush);
                }

                // Text output (document text rendering)
                const WCHAR* sampleText = L"Doc";
                TextOutW(memDC, 10, 60, sampleText, 3);

                // Cleanup bitmap
                SelectObject(memDC, oldBitmap);
                DeleteObject(hBitmap);
            }

            DeleteDC(memDC);
        }

        ReleaseDC(NULL, hdc);
    }

    // ========================================================================
    // Category 4: Safe failures (expected errors with NULL/invalid params)
    // ========================================================================

    // Enumerate printers (safe query that may fail on systems without printers)
    DWORD cbNeeded = 0;
    DWORD cReturned = 0;
    DWORD enumFlags = (0x2);  // PRINTER_ENUM_LOCAL

    // First call with NULL buffer to get required size (expected failure)
    EnumPrintersW(enumFlags, NULL, 2, NULL, 0, &cbNeeded, &cReturned);
    // We don't allocate buffer - just checking if function is available

    // Query cursor position (safe read-only operation)
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // Get window rectangle with NULL handle (safe failure)
    RECT rect;
    GetWindowRect(NULL, &rect);

    // Test file dialog error function (safe query)
    DWORD dlgError = CommDlgExtendedError();
}

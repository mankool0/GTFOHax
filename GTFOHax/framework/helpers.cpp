// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Helper functions

#include "pch-il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <codecvt>
#include <chrono>
#include "helpers.h"

// Log file location
extern const LPCWSTR LOG_FILE;

// Helper function to get the module base address
uintptr_t il2cppi_get_base_address() {
    return (uintptr_t) GetModuleHandleW(L"GameAssembly.dll");
}

// Helper function to append text to a file
void il2cppi_log_write(std::string text) {
    HANDLE hfile = CreateFileW(LOG_FILE, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hfile == INVALID_HANDLE_VALUE)
        MessageBoxW(0, L"Could not open log file", 0, 0);

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    char dateBuffer[100];
    tm timeInfo;
    localtime_s(&timeInfo, &time_t_now);
    std::strftime(dateBuffer, sizeof(dateBuffer), "%F %T %Z\t", &timeInfo);
    std::string dateString(dateBuffer);
    DWORD written;
    WriteFile(hfile, dateString.c_str(), (DWORD)dateString.length(), &written, NULL);
    WriteFile(hfile, text.c_str(), (DWORD) text.length(), &written, NULL);
    WriteFile(hfile, "\r\n", 2, &written, NULL);
    CloseHandle(hfile);
}

// Helper function to open a new console window and redirect stdout there
void il2cppi_new_console() {
    AllocConsole();
    freopen_s((FILE**) stdout, "CONOUT$", "w", stdout);
}

// Helper function to convert Il2CppString to std::string
std::string il2cppi_to_string(Il2CppString* str) {
    if (!str || !str->chars || str->length == 0) return "";

    const wchar_t* wstr = reinterpret_cast<const wchar_t*>(str->chars);
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, str->length, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) return "";

    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, str->length, &result[0], size_needed, nullptr, nullptr);
    return result;
}

// Helper function to convert System.String to std::string
std::string il2cppi_to_string(app::String* str) {
    return il2cppi_to_string(reinterpret_cast<Il2CppString*>(str));
}
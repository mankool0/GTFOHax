#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <filesystem>
#include <optional>
#include <memory>
#include <sstream>
#include "resource.h"

namespace fs = std::filesystem;

// RAII wrapper for Windows handles
struct HandleDeleter {
    void operator()(HANDLE handle) const {
        if (handle && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }
};
using UniqueHandle = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleDeleter>;

// Find process by name
std::optional<DWORD> FindProcess(const std::wstring& processName) {
    UniqueHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    if (!snapshot || snapshot.get() == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }

    PROCESSENTRY32W entry{ .dwSize = sizeof(PROCESSENTRY32W) };
    
    if (Process32FirstW(snapshot.get(), &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                return entry.th32ProcessID;
            }
        } while (Process32NextW(snapshot.get(), &entry));
    }

    return std::nullopt;
}

// Extract DLL from embedded resource
std::optional<fs::path> ExtractDLL() {
    // Find the DLL resource
    HRSRC hResource = FindResourceW(nullptr, MAKEINTRESOURCEW(IDR_DLL), MAKEINTRESOURCEW(10));
    if (!hResource) return std::nullopt;

    HGLOBAL hLoadedResource = LoadResource(nullptr, hResource);
    if (!hLoadedResource) return std::nullopt;

    void* pResourceData = LockResource(hLoadedResource);
    if (!pResourceData) return std::nullopt;

    DWORD resourceSize = SizeofResource(nullptr, hResource);
    if (resourceSize == 0) return std::nullopt;

    // Create temp file
    wchar_t tempPath[MAX_PATH];
    wchar_t tempFile[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    GetTempFileNameW(tempPath, L"GTF", 0, tempFile);
    
    fs::path dllPath(tempFile);
    dllPath.replace_extension(L".dll");

    // Write to file
    HANDLE hFile = CreateFileW(dllPath.c_str(), GENERIC_WRITE, 0, nullptr, 
                               CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return std::nullopt;

    DWORD bytesWritten;
    bool success = WriteFile(hFile, pResourceData, resourceSize, &bytesWritten, nullptr);
    CloseHandle(hFile);

    if (!success || bytesWritten != resourceSize) {
        DeleteFileW(dllPath.c_str());
        return std::nullopt;
    }

    return dllPath;
}

// Perform DLL injection
bool InjectDLL(DWORD targetPID, const fs::path& dllPath) {
    UniqueHandle process(OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION,
        FALSE, targetPID
    ));
    if (!process) return false;

    const auto dllPathStr = dllPath.wstring();
    const size_t pathSize = (dllPathStr.length() + 1) * sizeof(wchar_t);
    
    LPVOID remoteMem = VirtualAllocEx(process.get(), nullptr, pathSize, 
                                      MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) return false;

    auto cleanup = [hProcess = process.get()](void* ptr) { 
        if (ptr) VirtualFreeEx(hProcess, ptr, 0, MEM_RELEASE); 
    };
    std::unique_ptr<void, decltype(cleanup)> memGuard(remoteMem, cleanup);

    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(process.get(), remoteMem, dllPathStr.c_str(), pathSize, &bytesWritten)) {
        return false;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) return false;

    auto loadLibFunc = GetProcAddress(kernel32, "LoadLibraryW");
    if (!loadLibFunc) return false;

    UniqueHandle thread(CreateRemoteThread(
        process.get(), nullptr, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibFunc),
        remoteMem, 0, nullptr
    ));
    if (!thread) return false;

    if (WaitForSingleObject(thread.get(), 8000) != WAIT_OBJECT_0) return false;

    DWORD exitCode = 0;
    return GetExitCodeThread(thread.get(), &exitCode) && exitCode != 0;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
    // Extract DLL
    auto dllPath = ExtractDLL();
    if (!dllPath) {
        MessageBoxW(nullptr, L"Failed to extract DLL from resources", 
                   L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Find process
    auto pid = FindProcess(L"GTFO.exe");
    if (!pid) {
        MessageBoxW(nullptr, L"GTFO.exe is not running", 
                   L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Inject
    bool success = InjectDLL(*pid, *dllPath);
    
    // Cleanup temp DLL file
    DeleteFileW(dllPath->c_str());
    
    if (success) {
        MessageBoxW(nullptr, L"Injection successful!", 
                   L"Success", MB_OK | MB_ICONINFORMATION);
        return 0;
    } else {
        MessageBoxW(nullptr, L"Injection failed\n\nTry running as Administrator", 
                   L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
}

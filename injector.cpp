#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

DWORD FindProcessByName(const wchar_t* procName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[ERROR] Failed to create process snapshot" << std::endl;
        return 0;
    }

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(PROCESSENTRY32W);

    DWORD foundPID = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, procName) == 0) {
                foundPID = entry.th32ProcessID;
                std::wcout << L"[SUCCESS] Found " << procName << L" (PID: " << foundPID << L")" << std::endl;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);

    if (!foundPID) {
        std::wcerr << L"[ERROR] Process " << procName << L" not running" << std::endl;
    }

    return foundPID;
}

int PerformInjection(DWORD targetPID, const wchar_t* libraryPath) {
    std::wcout << L"[INFO] Attempting injection into PID " << targetPID << std::endl;
    std::wcout << L"[INFO] Library: " << libraryPath << std::endl;

    HANDLE procHandle = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION,
        FALSE,
        targetPID
    );

    if (!procHandle || procHandle == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[ERROR] Cannot open process (Error: " << GetLastError() << L")" << std::endl;
        std::wcerr << L"[HINT] Try running as Administrator" << std::endl;
        return 1;
    }

    size_t pathSize = (wcslen(libraryPath) + 1) * sizeof(wchar_t);

    LPVOID remoteMem = VirtualAllocEx(
        procHandle,
        nullptr,
        pathSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!remoteMem) {
        std::wcerr << L"[ERROR] VirtualAllocEx failed (Error: " << GetLastError() << L")" << std::endl;
        CloseHandle(procHandle);
        return 1;
    }

    std::wcout << L"[INFO] Allocated memory at 0x" << std::hex << (uintptr_t)remoteMem << std::dec << std::endl;

    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(procHandle, remoteMem, libraryPath, pathSize, &bytesWritten) || bytesWritten != pathSize) {
        std::wcerr << L"[ERROR] WriteProcessMemory failed" << std::endl;
        VirtualFreeEx(procHandle, remoteMem, 0, MEM_RELEASE);
        CloseHandle(procHandle);
        return 1;
    }

    std::wcout << L"[INFO] Wrote " << bytesWritten << L" bytes to target process" << std::endl;

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) {
        std::wcerr << L"[ERROR] Failed to get kernel32.dll handle" << std::endl;
        VirtualFreeEx(procHandle, remoteMem, 0, MEM_RELEASE);
        CloseHandle(procHandle);
        return 1;
    }

    LPVOID loadLibFunc = (LPVOID)GetProcAddress(kernel32, "LoadLibraryW");
    if (!loadLibFunc) {
        std::wcerr << L"[ERROR] Failed to locate LoadLibraryW" << std::endl;
        VirtualFreeEx(procHandle, remoteMem, 0, MEM_RELEASE);
        CloseHandle(procHandle);
        return 1;
    }

    std::wcout << L"[INFO] LoadLibraryW address: 0x" << std::hex << (uintptr_t)loadLibFunc << std::dec << std::endl;

    HANDLE remoteThread = CreateRemoteThread(
        procHandle,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)loadLibFunc,
        remoteMem,
        0,
        nullptr
    );

    if (!remoteThread) {
        std::wcerr << L"[ERROR] CreateRemoteThread failed (Error: " << GetLastError() << L")" << std::endl;
        VirtualFreeEx(procHandle, remoteMem, 0, MEM_RELEASE);
        CloseHandle(procHandle);
        return 1;
    }

    std::wcout << L"[INFO] Remote thread created, waiting for completion..." << std::endl;

    DWORD waitResult = WaitForSingleObject(remoteThread, 8000);
    int retCode = 0;

    if (waitResult == WAIT_OBJECT_0) {
        DWORD exitCode = 0;
        if (GetExitCodeThread(remoteThread, &exitCode) && exitCode != 0) {
            std::wcout << L"[SUCCESS] DLL loaded at 0x" << std::hex << exitCode << std::dec << std::endl;
            retCode = 0;
        } else {
            std::wcerr << L"[ERROR] LoadLibraryW returned NULL - injection failed" << std::endl;
            std::wcerr << L"[HINT] Check DLL architecture (x64 vs x86) and dependencies" << std::endl;
            retCode = 1;
        }
    } else if (waitResult == WAIT_TIMEOUT) {
        std::wcerr << L"[WARNING] Thread timeout - injection status unknown" << std::endl;
        retCode = 1;
    } else {
        std::wcerr << L"[ERROR] Wait failed (Error: " << GetLastError() << L")" << std::endl;
        retCode = 1;
    }

    VirtualFreeEx(procHandle, remoteMem, 0, MEM_RELEASE);
    CloseHandle(remoteThread);
    CloseHandle(procHandle);

    return retCode;
}

int wmain(int argc, wchar_t* argv[]) {
    std::wcout << L"========================================" << std::endl;
    std::wcout << L"  GTFOHax Injector" << std::endl;
    std::wcout << L"========================================" << std::endl;

    std::wstring processName = L"GTFO.exe";
    std::wstring dllPath;

    if (argc >= 3) {
        processName = argv[1];
        dllPath = argv[2];
    } else {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        fs::path injectorPath(exePath);
        dllPath = (injectorPath.parent_path() / L"GTFOHax.dll").wstring();

        if (argc == 1) {
            std::wcout << L"Usage: injector.exe <process_name> <dll_path>" << std::endl;
            std::wcout << L"Using default values..." << std::endl;
        }
    }

    if (!fs::exists(dllPath)) {
        std::wcerr << L"[ERROR] DLL not found: " << dllPath << std::endl;

        if (!GetConsoleWindow()) {
            MessageBoxW(nullptr,
                       (L"DLL file not found:\n" + dllPath).c_str(),
                       L"Injection Error",
                       MB_OK | MB_ICONERROR);
        }
        return 1;
    }

    std::wcout << L"[INFO] DLL path: " << dllPath << std::endl;

    DWORD pid = FindProcessByName(processName.c_str());
    if (pid == 0) {
        if (!GetConsoleWindow()) {
            MessageBoxW(nullptr,
                       (L"Process not found: " + processName).c_str(),
                       L"Injection Error",
                       MB_OK | MB_ICONERROR);
        }
        return 1;
    }

    int result = PerformInjection(pid, dllPath.c_str());

    if (!GetConsoleWindow()) {
        MessageBoxW(nullptr,
                   result == 0 ? L"Injection successful!" : L"Injection failed - see console for details",
                   result == 0 ? L"Success" : L"Error",
                   result == 0 ? MB_OK | MB_ICONINFORMATION : MB_OK | MB_ICONERROR);
    }

    return result;
}

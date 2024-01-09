// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Custom injected code entry point
#include "..\framework\pch-il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <vector>
#include "..\framework\helpers.h"
#include "..\hooks.h"
#include "..\hacks\esp.h"
#include "..\config\config.h"


// Set the name of your log file here
extern const LPCWSTR LOG_FILE = L"GTFOHax.log";


// Custom injected code entry point
void Run()
{
    // Initialize thread data - DO NOT REMOVE
    il2cpp_thread_attach(il2cpp_domain_get());

    // If you would like to write to a log file, specify the name above and use il2cppi_log_write()
    // il2cppi_log_write("Startup");

    // If you would like to output to a new console window, use il2cppi_new_console() to open one and redirect stdout
    //il2cppi_new_console();

    Hooks::InitHooks();
    ESP::Init();
    Config::Load();

    while (G::running)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    kiero::shutdown();
    Hooks::RemoveHooks();

    CloseHandle(G::runThread);
    FreeLibraryAndExitThread(G::hModule, 0);
}
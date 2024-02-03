#include "globals.h"

namespace G {
    bool running = true;
    HMODULE hModule;
    HANDLE runThread;
    Present oPresent = NULL;
    ID3D11Device* pDevice = NULL;
    ID3D11DeviceContext* pContext = NULL;
    ID3D11RenderTargetView* mainRenderTargetView = NULL;
    WNDPROC oWndProc = NULL;
    HWND windowHwnd = NULL;
    hSetCursorPos oSetCursorPos = NULL;

    std::string configPath = "GTFOHax.json";

    std::mutex worldItemsMtx;
    std::mutex worldArtifMtx;
    std::mutex worldCarryMtx;
    std::mutex worldKeyMtx;
    std::mutex worldGenericMtx;
    std::mutex worldResourcePackMtx;
    std::mutex worldTerminalsMtx;
    std::mutex worldHSUMtx;
    std::mutex worldBulkheadMtx;
    std::mutex enemyVecMtx;
    std::mutex imguiMtx;
    std::mutex enemyAimMtx;

    bool initialized = false;
    bool showMenu = true;
    KeyBind menuKey(KeyBind::INSERT);
    KeyBind unloadKey(KeyBind::DEL);

    bool watermark = true;
    int watermarkCorner = 0;

    app::Camera* mainCamera = NULL;
    app::Matrix4x4 w2CamMatrix;
    app::Matrix4x4 projMatrix;
    app::Matrix4x4 viewMatrix;
    app::PlayerAgent* localPlayer = NULL;
    int32_t screenHeight = NULL;
    int32_t screenWidth = NULL;

    ImFont* defaultFont;
    ImFont* espFont;

    std::queue<std::function<void()>> callbacks;
}
#include "gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

LRESULT CALLBACK windowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {
        case WM_SIZE:
        {
            if (gui::m_device && wideParameter != SIZE_MINIMIZED)
            {
                gui::m_parameters.BackBufferWidth = LOWORD(longParameter);
                gui::m_parameters.BackBufferHeight = HIWORD(longParameter);
                gui::resetDevice();
            }
        } return 0;

        case WM_SYSCOMMAND:
        {
            if ((wideParameter & 0xfff0) == SC_KEYMENU)
                return 0;
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } return 0;

        case WM_LBUTTONDOWN: 
        {
            gui::m_position = MAKEPOINTS(longParameter);
        } return 0;
    
        case WM_MOUSEMOVE:
        {
            if (wideParameter == MK_LBUTTON)
            {
                const auto points = MAKEPOINTS(longParameter);
                auto rect = RECT{};

                GetWindowRect(window, &rect);

                rect.left += points.x - gui::m_position.x;
                rect.top += points.y - gui::m_position.y;

                if (gui::m_position.x >= 0 && gui::m_position.x <= gui::m_width && gui::m_position.y >= 0 && gui::m_position.y <= 19)
                    SetWindowPos(gui::m_window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
            }
        } return 0;
    }

    return DefWindowProcW(window, message, wideParameter, longParameter);
};

void gui::createWindow(const char* windowName, const char* className) noexcept
{
    m_windowClass.cbSize = sizeof(WNDCLASSEXA);
    m_windowClass.style = CS_CLASSDC;
    m_windowClass.lpfnWndProc = windowProcess;
    m_windowClass.cbClsExtra = 0;
    m_windowClass.cbWndExtra = 0;
    m_windowClass.hInstance = GetModuleHandleA(0);
    m_windowClass.hIcon = 0;
    m_windowClass.hCursor = 0;
    m_windowClass.hbrBackground = 0;
    m_windowClass.lpszMenuName = 0;
    m_windowClass.lpszClassName = className;
    m_windowClass.hIconSm = 0;
     
    m_window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, m_width, m_height, 0, 0, m_windowClass.hInstance, 0);

    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);
}

void gui::destroyWindow() noexcept
{
    DestroyWindow(m_window);
    UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}

bool gui::createDevice() noexcept
{
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!m_d3d)
        return false;

    ZeroMemory(&m_parameters, sizeof(m_parameters));

    m_parameters.Windowed = TRUE;
    m_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
    m_parameters.EnableAutoDepthStencil = TRUE;
    m_parameters.AutoDepthStencilFormat = D3DFMT_D16;
    m_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_parameters, &m_device) < 0)
        return false;

    return true;
}

void gui::resetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = m_device->Reset(&m_parameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::destroyDevice() noexcept
{
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_d3d)
    {
        m_d3d->Release();
        m_d3d = nullptr;
    }
}

void gui::createImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = NULL;

    ImGui::StyleColorsLight();

    ImGui_ImplWin32_Init(m_window);
    ImGui_ImplDX9_Init(m_device);
}

void gui::destroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::gui(const std::string& windowName) noexcept
{
    createWindow(windowName.c_str(), (windowName + std::string(" Class")).c_str());
    createDevice();
    createImGui();
}

void gui::destroy() noexcept
{
    destroyImGui();
    destroyDevice();
    destroyWindow();
}

void gui::beginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::endRender() noexcept
{
    ImGui::EndFrame();

    m_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    m_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (m_device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        m_device->EndScene();
    }

    const auto result = m_device->Present(0, 0, 0, 0);

    if (result == D3DERR_DEVICELOST && m_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        resetDevice();
}

void gui::render() noexcept
{
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ (float)m_width, (float)m_height });
    ImGui::Begin("ImGuiWrapper", &m_exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
    ImGui::Button("click");
    ImGui::End();
}

bool gui::exit() noexcept
{
    return m_exit;
}

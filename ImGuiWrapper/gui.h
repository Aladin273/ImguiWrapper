#pragma once

#include <d3d9.h>
#include <string>

// TODO: opengl
// TODO: linux
// TODO: android
// TODO: std_image
// TODO: class architecture

namespace gui
{
    constexpr int m_width = 500;
    constexpr int m_height = 300;

    inline bool m_exit = true;

    inline HWND m_window = nullptr;
    inline WNDCLASSEXA m_windowClass = { };

    inline POINTS m_position = { };

    inline PDIRECT3D9 m_d3d = nullptr;
    inline LPDIRECT3DDEVICE9 m_device = nullptr;
    inline D3DPRESENT_PARAMETERS m_parameters = { };

    void gui(const std::string& windowName) noexcept;
    void destroy() noexcept;

    void beginRender() noexcept;
    void endRender() noexcept;
    void render() noexcept;
    bool exit() noexcept;

    void createWindow(const char* windowName, const char* className) noexcept;
    void destroyWindow() noexcept;

    bool createDevice() noexcept;
    void destroyDevice() noexcept;
    void resetDevice() noexcept;

    void createImGui() noexcept;
    void destroyImGui() noexcept;
};


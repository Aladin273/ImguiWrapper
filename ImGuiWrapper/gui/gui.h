#pragma once

#include <d3d9.h>

class gui
{
private:
    void createWindow(const char* windowName, const char* className) noexcept;
    void destroyWindow() noexcept;

    bool createDevice() noexcept;
    void resetDevice() noexcept;
    void destroyDevice() noexcept;

    void createImGui() noexcept;
    void destroyImGui() noexcept;

    void beginRender() noexcept;
    void endRender() noexcept;
    void render() noexcept;

    int m_width = 500;
    int m_height = 300;

    bool m_exit = true;

    HWND m_window = nullptr;
    WNDCLASSEXA m_class = { };

    POINTS m_position = { };

    PDIRECT3D9 m_d3d9 = nullptr;
    LPDIRECT3DDEVICE9 m_device = nullptr;
    D3DPRESENT_PARAMETERS m_parameters = { };
};


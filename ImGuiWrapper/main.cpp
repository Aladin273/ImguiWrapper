#include "gui.h"

#include <thread>

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
    //gui::gui("Hello ImGuiWrapper");
    gui::createWindow("Hello ImGuiWrapper", "Hello ImGuiWrapper Class");
    gui::createDevice();
    gui::createImGui();
    
    while (gui::exit())
    {
        gui::beginRender();
        gui::render();
        gui::endRender();
    
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    gui::destroyWindow();
    gui::destroyDevice();
    gui::destroyImGui();

    return EXIT_SUCCESS;
}
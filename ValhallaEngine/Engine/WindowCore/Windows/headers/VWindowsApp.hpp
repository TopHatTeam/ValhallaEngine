// -------------------------------------------------
//
//  MIT License
//
//  VWindowsApp.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/14/26.
//
// -------------------------------------------------

#pragma once 

#include <Windows.h> // Ignore this warning about Windows.h on Mac or Linux, they don't have this header

namespace ValhallaEngine::Windows
{
    struct VWindowsApp
    {
    public:
        VWindowsApp() = default;

        static void DisplaySplash(const char* filepath);

        static void HideSplash();

        static void Desconstruct();

        static int RunApplication(HINSTANCE hInstance, int nCmdShow);
    protected:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        static HWND ApplicationWindow;
    };
}


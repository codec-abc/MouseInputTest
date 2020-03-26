#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <iostream>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int paint_count = 0;
int mouse_count = 0;
UINT timerID = 9999;
int mouse_x = 0;
int mouse_y = 0;

COLORREF buildColorRef(char r, char g, char b) {
    return ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)));
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = hwnd;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    timerID = SetTimer(hwnd,             // handle to main window 
        timerID,            // timer identifier 
        400,                // interval as ms
        (TIMERPROC)NULL);     // no timer callback 

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.
        HBRUSH brush = CreateSolidBrush(buildColorRef(paint_count % 255,0,0));
        FillRect(hdc, &ps.rcPaint, brush); // (HBRUSH)(COLOR_WINDOW + 1)
        EndPaint(hwnd, &ps);

        {
            wchar_t buffer[50];
            swprintf(buffer, L"paint %d\n", paint_count);
            OutputDebugString(buffer);
            paint_count++;
        }

        Sleep(3000);
        break;
    }
    case WM_INPUT:
    {
        //OutputDebugString(TEXT("raw input event\n"));
        UINT dwSize = 1;

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
            sizeof(RAWINPUTHEADER));

        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == NULL)
        {
            return 0;
        }

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
            sizeof(RAWINPUTHEADER)) != dwSize)
            OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEKEYBOARD)
        {
            //hResult = StringCchPrintf(szTempOutput, STRSAFE_MAX_CCH, TEXT(" Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n"),
            //    raw->data.keyboard.MakeCode,
            //    raw->data.keyboard.Flags,
            //    raw->data.keyboard.Reserved,
            //    raw->data.keyboard.ExtraInformation,
            //    raw->data.keyboard.Message,
            //    raw->data.keyboard.VKey);
            //if (FAILED(hResult))
            //{
            //    // TODO: write error handler
            //}
            //OutputDebugString(szTempOutput);
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            wchar_t buffer[1000];
            swprintf(buffer,
                TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"),
                raw->data.mouse.usFlags,
                raw->data.mouse.ulButtons,
                raw->data.mouse.usButtonFlags,
                raw->data.mouse.usButtonData,
                raw->data.mouse.ulRawButtons,
                raw->data.mouse.lLastX,
                raw->data.mouse.lLastY,
                raw->data.mouse.ulExtraInformation);

            //OutputDebugString(buffer);

            LONG x = raw->data.mouse.lLastX;
            LONG y = raw->data.mouse.lLastY;
            USHORT button_flags = raw->data.mouse.usButtonFlags;

            swprintf(buffer,
                TEXT("Mouse! move: %04x %04x, left button state change: %s\n"),
                x,
                y,
                button_flags == 
                    0x0001 ? 
                        TEXT("button down") :
                        button_flags == 0x0002 ? TEXT("button up") : TEXT("no change"));

            OutputDebugString(buffer);

            //if (FAILED(hResult))
            //{
            //    // TODO: write error handler
            //}
            //OutputDebugString(szTempOutput);

            //OutputDebugString(TEXT("mouse event\n"));

            /*   {
                wchar_t buffer[50];
                swprintf(buffer, L"mouse %d\n", mouse_count);
                OutputDebugString(buffer);
                mouse_count++;
            }*/

            //BOOL updateWindow = UpdateWindow(hwnd);

            //{
            //    wchar_t buffer[50];
            //    swprintf(buffer, L"update window %d\n", updateWindow);
            //    OutputDebugString(buffer);
            //}

            // RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
        }

        delete[] lpb;
        return 0;
    }
    case WM_TIMER:
    {
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
        return 0;
    }
    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

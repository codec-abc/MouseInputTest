#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <iostream>

bool sleep = true;
int sleepDuration = 1400;
int mouseCursorWidth = 3;
int mouseClickCursorWidth = 4;

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
int mouse_x = 400;
int mouse_y = 400;
bool has_mouse_click = false;
int mouse_click_x = mouse_x;
int mouse_click_y = mouse_y;

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
        2,                // interval as ms
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
    //OutputDebugString(TEXT("running WindowProc\n"));
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
        
        HBRUSH brush = CreateSolidBrush(buildColorRef(
            sleep ? paint_count % 255 : 255, 
            0, 
            0)
        );

        FillRect(hdc, &ps.rcPaint, brush);

        struct tagRECT a;

       
        if (has_mouse_click) {
            brush = CreateSolidBrush(buildColorRef(0, 0, 255));
            a.bottom = mouse_click_y - mouseClickCursorWidth;
            a.top = mouse_click_y + mouseClickCursorWidth;
            a.left = mouse_click_x - mouseClickCursorWidth;
            a.right = mouse_click_x + mouseClickCursorWidth;
            FillRect(hdc, &a, brush);
        }
        
        brush = CreateSolidBrush(buildColorRef(255, 255, 255));

        a.bottom = mouse_y - mouseCursorWidth;
        a.top = mouse_y + mouseCursorWidth;
        a.left = mouse_x - mouseCursorWidth;
        a.right = mouse_x + mouseCursorWidth;
        FillRect(hdc, &a, brush);

        EndPaint(hwnd, &ps);

        {
            wchar_t buffer[50];
            swprintf(buffer, L"paint %d\n", paint_count);
            OutputDebugString(buffer);
            paint_count += 50;
        }

        if (sleep) {
            Sleep(sleepDuration);
        }
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

            //swprintf(buffer,
            //    TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"),
            //    raw->data.mouse.usFlags,
            //    raw->data.mouse.ulButtons,
            //    raw->data.mouse.usButtonFlags,
            //    raw->data.mouse.usButtonData,
            //    raw->data.mouse.ulRawButtons,
            //    raw->data.mouse.lLastX,
            //    raw->data.mouse.lLastY,
            //    raw->data.mouse.ulExtraInformation);

            //OutputDebugString(buffer);

            LONG x = raw->data.mouse.lLastX;
            LONG y = raw->data.mouse.lLastY;
            USHORT button_flags = raw->data.mouse.usButtonFlags;

            mouse_x = mouse_x + x;
            mouse_y = mouse_y + y;

            if (button_flags == 0x0001) {
                has_mouse_click = true;
                mouse_click_x = mouse_x;
                mouse_click_y = mouse_y;
            }

            swprintf(buffer,
                TEXT("Mouse! move: %04x %04x, left button state change: %s\n"),
                x,
                y,
                button_flags == 
                    0x0001 ? 
                        TEXT("button down") :
                        button_flags == 0x0002 ? TEXT("button up") : TEXT("no change"));

            OutputDebugString(buffer);
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

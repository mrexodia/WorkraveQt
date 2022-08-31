#include "Helpers.h"

#include <QCoreApplication>

#if defined(Q_OS_WIN)

#include <Windows.h>

struct InputState
{
    DWORD tickCount;
    BYTE keyState[256];
    POINT cursorPos;

    InputState()
    {
        GetKeyboardState(keyState);
        GetCursorPos(&cursorPos);
        tickCount = GetTickCount();
    }
};

static int getIdleTimeImpl()
{
    static InputState oldState;
    InputState curState;

    auto keyPressed = memcmp(oldState.keyState, curState.keyState, sizeof(oldState.keyState)) != 0;
    auto cursorMoved = memcmp(&oldState.cursorPos, &curState.cursorPos, sizeof(oldState.cursorPos)) != 0;

    if(keyPressed || cursorMoved)
        oldState = curState;

    return (curState.tickCount - oldState.tickCount) / 1000;
}

static void lockScreenImpl()
{
	LockWorkStation();
}

static HHOOK g_hKeyboardHook = nullptr;

// Source: https://docs.microsoft.com/en-us/windows/win32/dxtecharts/disabling-shortcut-keys-in-games
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode != HC_ACTION)  // do not process message
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

    bool bEatKeystroke = false;
    auto p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    switch (wParam)
    {
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    {
        // Block Alt+Space
        bEatKeystroke = p->vkCode == VK_SPACE && (p->flags & LLKHF_ALTDOWN) != 0;
        break;
    }

    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        // Block the Windows key
        bEatKeystroke = (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN);
        // Note that this will not block the Xbox Game Bar hotkeys (Win+G, Win+Alt+R, etc.)
        break;
    }
    }

    if (bEatKeystroke)
        return 1;
    else
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

static void startBreakImpl()
{
    if(!g_hKeyboardHook)
        g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
}

static void stopBreakImpl()
{
    if(g_hKeyboardHook)
    {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = nullptr;
    }
}

#elif defined(Q_OS_MAC)

#include "macos_platform.h"

static void startBreakImpl()
{
}

static void stopBreakImpl()
{
}

#else

#include <QMessageBox>

static int getIdleTimeImpl()
{
    return 0;
}

static void lockScreenImpl()
{
	QMessageBox::information(nullptr, "TODO", "Not yet implemented!");
}

static void startBreakImpl()
{
}

static void stopBreakImpl()
{
}

#endif // Q_OS_WIN

namespace Helpers
{
QString timeFormat(int seconds)
{
    auto minutes = seconds / 60;
    seconds = seconds % 60;
    return QString().sprintf("%d:%02d", minutes, seconds);
}

int getIdleTimeS()
{
    return getIdleTimeImpl();
}

void restartApplication()
{
	QCoreApplication::exit(EXIT_RESTART);
}

void lockScreen()
{
    lockScreenImpl();
}

void startBreak()
{
    startBreakImpl();
}

void stopBreak()
{
    stopBreakImpl();
}

} // namespace Helpers

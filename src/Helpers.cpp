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

#elif defined(Q_OS_MAC)

#include "macos_platform.h"

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

} // namespace Helpers

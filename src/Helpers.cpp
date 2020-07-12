#include "Helpers.h"

#ifdef Q_OS_WIN

#include <Windows.h>

// https://chromium.googlesource.com/chromium/src.git/+/62.0.3178.1/ui/base/idle/idle_win.cc#15
static int getIdleTimeImpl()
{
    LASTINPUTINFO last_input_info = { 0 };
    last_input_info.cbSize = sizeof(LASTINPUTINFO);
    DWORD current_idle_time = 0;
    if (::GetLastInputInfo(&last_input_info))
    {
        DWORD now = ::GetTickCount();
        if (now < last_input_info.dwTime)
        {
            // GetTickCount() wraps around every 49.7 days -- assume it wrapped just
            // once.
            const DWORD kMaxDWORD = static_cast<DWORD>(-1);
            DWORD time_before_wrap = kMaxDWORD - last_input_info.dwTime;
            DWORD time_after_wrap = now;
            // The sum is always smaller than kMaxDWORD.
            current_idle_time = time_before_wrap + time_after_wrap;
        }
        else
        {
            current_idle_time = now - last_input_info.dwTime;
        }
        // Convert from ms to seconds.
        current_idle_time /= 1000;
    }
    return (int)current_idle_time;
}
#else

static int getIdleTimeImpl()
{
    return 0;
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

int getIdleTimeMs()
{
    return getIdleTimeImpl();
}

} // namespace Helpers

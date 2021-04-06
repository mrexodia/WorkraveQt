#pragma once

#include <QString>

namespace Helpers
{
QString timeFormat(int seconds);
int getIdleTimeMs();
void restartApplication();
void lockScreen();
static constexpr int EXIT_RESTART = -1;
} // namespace Helpers

#pragma once

#include <QString>
#include <QDebug>

namespace Helpers
{
QString timeFormat(int seconds);
int getIdleTimeS();
void restartApplication();
void lockScreen();
void startBreak();
void stopBreak();
static constexpr int EXIT_RESTART = -1;
} // namespace Helpers

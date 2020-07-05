#include "Configuration.h"
#include <QSettings>

void Configuration::load()
{
    QSettings settings;
#define setting(x, d) m ## x = settings.value(#x, d).toInt()
    setting(MicroBreakCycle, 10);
    setting(MicroBreakNotification, 5);
    setting(MicroBreakDuration, 15);
    setting(RestBreakCycle, 60);
    setting(RestBreakNotification, 40);
    setting(RestBreakDuration, 15);
#undef setting
}

void Configuration::save()
{
    QSettings settings;
#define setting(x) settings.setValue(#x, m ## x)
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
}

void Configuration::dump()
{
#define setting(x) printf("%s: %d\n", #x, m ## x)
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
}

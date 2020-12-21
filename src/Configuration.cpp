#include "Configuration.h"
#include <QSettings>
#include <QDebug>

void Configuration::load()
{
    if(mIsTestConfiguration)
    {
        qDebug() << "Configuration::load() does nothing when using a test configuration";
        return;
    }

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
    if(mIsTestConfiguration)
    {
        qDebug() << "Configuration::save() does nothing when using a test configuration";
        return;
    }

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
    if(mIsTestConfiguration)
    {
        qDebug() << "Configuration::dump() this is a test configuration";
    }

#define setting(x) printf("%s: %d\n", #x, m ## x)
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
#undef setting
}

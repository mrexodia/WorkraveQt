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
    Configuration defaults;
#define setting(x) m ## x = settings.value(#x, defaults.m ## x).toInt()
    setting(MicroBreakCycle);
    setting(MicroBreakNotification);
    setting(MicroBreakDuration);
    setting(RestBreakCycle);
    setting(RestBreakNotification);
    setting(RestBreakDuration);
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

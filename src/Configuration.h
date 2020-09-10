#pragma once

struct Configuration
{
    int mMicroBreakCycle = 15 * 60;
    int mMicroBreakNotification = mMicroBreakCycle - 60;
    int mMicroBreakDuration = 45;

    int mRestBreakCycle = 45 * 60;
    int mRestBreakNotification = mRestBreakCycle - 60;
    int mRestBreakDuration = 7 * 60 + 30;

    void load();
    void save();
    void dump();
};


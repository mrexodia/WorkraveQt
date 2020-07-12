#pragma once

struct Configuration
{
    int mMicroBreakCycle = 10;
    int mMicroBreakNotification = mMicroBreakCycle - 5;
    int mMicroBreakDuration = 15;

    int mRestBreakCycle = 60;
    int mRestBreakNotification = mRestBreakCycle - 20;
    int mRestBreakDuration = 20;

    void load();
    void save();
    void dump();
};


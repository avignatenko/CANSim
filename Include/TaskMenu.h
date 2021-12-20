#pragma once

#include "Common.h"

class SerialCommands;

class TaskMenu
{
public:
    static void init(Scheduler& sh);

    static TaskMenu& instance();

    void start();

    using SimAddressCallback = uint16_t (*)(uint16_t);
    void setSimAddressCallback(SimAddressCallback callback);

protected:
    TaskMenu(Scheduler& sh);
    virtual ~TaskMenu() {}

protected:
    static TaskMenu* instance_;
    SerialCommands* cmdLine_ = nullptr;

    virtual void helpCallback(SerialCommands* sender);

private:
    void loopMenuCallback();
    static void loopMenuCallbackStatic();

    static void errorCallback(SerialCommands* sender, const char* command);
    static void cmdSimAddressCallback(SerialCommands* sender);
    static void cmdHelpCallback(SerialCommands* sender);

private:
    Task task_;

    SimAddressCallback simAddressCallback_ = nullptr;
};
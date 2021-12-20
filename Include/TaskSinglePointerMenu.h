#pragma once

#include "Common.h"
#include "TaskMenu.h"

class TaskSinglePointerMenu : public TaskMenu
{
public:
    static void init(Scheduler& sh);

    static TaskSinglePointerMenu& instance();

    using PosCallback = void (*)(int32_t);
    void setPosCallback(PosCallback callback);
    using LPosCallback = void (*)(float);
    void setLPosCallback(LPosCallback callback);
    using InteractiveCallback = void (*)(int16_t);
    void setInteractiveCallback(InteractiveCallback callback);

protected:
    virtual void helpCallback(SerialCommands* sender) override;
    TaskSinglePointerMenu(Scheduler& sh);

private:
    static void cmdPosCallback(SerialCommands* sender);
    static void cmdCCWCallback(SerialCommands* sender);
    static void cmdCCWFastCallback(SerialCommands* sender);
    static void cmdCWCallback(SerialCommands* sender);
    static void cmdCWFastCallback(SerialCommands* sender);

private:
    PosCallback posCallback_ = nullptr;
    InteractiveCallback interactiveCallback_ = nullptr;
};
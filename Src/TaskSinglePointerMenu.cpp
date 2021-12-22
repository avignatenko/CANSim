#if 0
#include "TaskSinglePointerMenu.h"

#include <SerialCommands.h>

void TaskSinglePointerMenu::init(Scheduler& sh)
{
    instance_ = new TaskSinglePointerMenu(sh);
}

TaskSinglePointerMenu& TaskSinglePointerMenu::instance()
{
    return (TaskSinglePointerMenu&)*instance_;
}

void TaskSinglePointerMenu::helpCallback(SerialCommands* sender)
{
    return TaskMenu::helpCallback(sender);
}
TaskSinglePointerMenu::TaskSinglePointerMenu(Scheduler& sh) : TaskMenu(sh)
{
    static SerialCommand s_cmdPos("pos", &TaskSinglePointerMenu::cmdPosCallback);
    static SerialCommand s_cmdPosCCW("q", &TaskSinglePointerMenu::cmdCCWCallback, true);
    static SerialCommand s_cmdPosCCWFast("Q", &TaskSinglePointerMenu::cmdCCWFastCallback, true);
    static SerialCommand s_cmdPosCW("w", &TaskSinglePointerMenu::cmdCWCallback, true);
    static SerialCommand s_cmdPosCWFast("W", &TaskSinglePointerMenu::cmdCWFastCallback, true);

    cmdLine_->AddCommand(&s_cmdPos);
    cmdLine_->AddCommand(&s_cmdPosCCW);
    cmdLine_->AddCommand(&s_cmdPosCCWFast);
    cmdLine_->AddCommand(&s_cmdPosCW);
    cmdLine_->AddCommand(&s_cmdPosCWFast);
}

void TaskSinglePointerMenu::cmdPosCallback(SerialCommands* sender)
{
    const char* posStr = sender->Next();
    if (!posStr)
    {
        posStr = "0";
    }

    int32_t pos = atol(posStr);

    if (TaskSinglePointerMenu::instance().posCallback_) TaskSinglePointerMenu::instance().posCallback_(pos);
}

void TaskSinglePointerMenu::cmdCCWCallback(SerialCommands* sender)
{
    if (TaskSinglePointerMenu::instance().interactiveCallback_)
        TaskSinglePointerMenu::instance().interactiveCallback_(-1);
}
void TaskSinglePointerMenu::cmdCCWFastCallback(SerialCommands* sender)
{
    if (TaskSinglePointerMenu::instance().interactiveCallback_)
        TaskSinglePointerMenu::instance().interactiveCallback_(-10);
}
void TaskSinglePointerMenu::cmdCWCallback(SerialCommands* sender)
{
    if (TaskSinglePointerMenu::instance().interactiveCallback_)
        TaskSinglePointerMenu::instance().interactiveCallback_(1);
}
void TaskSinglePointerMenu::cmdCWFastCallback(SerialCommands* sender)
{
    if (TaskSinglePointerMenu::instance().interactiveCallback_)
        TaskSinglePointerMenu::instance().interactiveCallback_(10);
}

void TaskSinglePointerMenu::setPosCallback(PosCallback callback)
{
    posCallback_ = callback;
}

void TaskSinglePointerMenu::setInteractiveCallback(InteractiveCallback callback)
{
    interactiveCallback_ = callback;
}

#endif
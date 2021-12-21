#pragma once

#include "Common.h"

#include <Array.h>

class SerialCommands;

class TaskMenu : private Task
{
public:
    static void init(Scheduler& sh);

    static TaskMenu& instance();

    void start();

    using VarGetCallback = float (*)();
    using VarSetCallback = void (*)(float);
    void addVar(const char* name, VarGetCallback get, VarSetCallback set);

protected:
    TaskMenu(Scheduler& sh);
    virtual ~TaskMenu() {}

protected:
    static TaskMenu* instance_;
    SerialCommands* cmdLine_ = nullptr;

    virtual void helpCallback(SerialCommands* sender);

private:
    virtual bool Callback() override;

    static void errorCallback(SerialCommands* sender, const char* command);
    static void cmdVarCallback(SerialCommands* sender);
    static void cmdHelpCallback(SerialCommands* sender);

private:
    struct Var
    {
        const char* name = "";
        VarGetCallback getter = nullptr;
        VarSetCallback setter = nullptr;
    };

    Array<Var, 20> vars_;
};
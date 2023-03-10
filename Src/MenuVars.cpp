
#include "MenuVars.h"


VarsMenu::VarsMenu(VarsStorageBase& storage, SerialCommands& menu) : vars_(storage)
{
    menu.AddCommand(new SerialCommand("var", &VarsMenu::cmdVarCallback, false, this));
}


void VarsMenu::cmdVarCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<VarsMenu*>(data);
    me->varCallback(sender);
}


void VarsMenu::varCallback(SerialCommands* sender)
{
    auto& s = *sender->GetSerial();

    const char* nameStr = sender->Next();
    if (!nameStr)
    {
        // list of variables and their values
        for (uint8_t i = 0; i < vars_.numVars(); ++i)
        {
            s.print(vars_.getVarName(i));
            s.print(' ');
            s.println(vars_.getVar(i));
        }
        return;
    }

    // find this variable
    int8_t idx = -1;
    for (uint8_t i = 0; i < vars_.numVars(); ++i)
    {
        if (strcmp(vars_.getVarName(i), nameStr) == 0)
        {
            idx = i;
            break;
        }
    }
    if (idx < 0)
    {
        sender->GetSerial()->println(F("Error: no such variable"));
        return;
    }

    const char* valueStr = sender->Next();

    if (!valueStr)
    {
        s.println(vars_.getVar(idx));
        return;
    }

    float value = atof(valueStr);
    vars_.setVar(idx, value);
    s.println(vars_.getVar(idx));
}

 String VarsMenu::help()
 {
    return F("var [name] [value] get/set device variable");
 }
  
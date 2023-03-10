#pragma once

#include "Common.h"

#include <EEPROM.h>
#include <SerialCommands.h>
#include <etl/array.h>
#include <etl/delegate.h>

class VarsStorageBase
{
public:
    virtual uint8_t numVars() = 0;
    virtual float getVar(uint8_t idx) = 0;
    virtual const char* getVarName(uint8_t idx) = 0;
    virtual void setVar(uint8_t idx, float value) = 0;
};

template <size_t SIZE, size_t OFFSET>
class VarsStorage : public VarsStorageBase
{
public:
    VarsStorage(etl::array<const char*, SIZE> vars) : vars_(vars) {}

    virtual uint8_t numVars() override { return SIZE; }
    virtual float getVar(uint8_t idx) override;
    virtual const char* getVarName(uint8_t idx) override;
    virtual void setVar(uint8_t idx, float value) override;

    void setVarCallback(etl::delegate<void(uint8_t, float)> callback) { callback_ = callback; }

private:
    etl::array<const char*, SIZE> vars_;
    etl::delegate<void(uint8_t, float)> callback_;
};

template <size_t SIZE, size_t OFFSET>
float VarsStorage<SIZE, OFFSET>::getVar(uint8_t idx)
{
    float var;
    EEPROM.get(OFFSET + idx * sizeof(float), var);
    return var;
}

template <size_t SIZE, size_t OFFSET>
const char* VarsStorage<SIZE, OFFSET>::getVarName(uint8_t idx)
{
    return vars_[idx];
}

template <size_t SIZE, size_t OFFSET>
void VarsStorage<SIZE, OFFSET>::setVar(uint8_t idx, float value)
{
    EEPROM.put(OFFSET + idx * sizeof(float), value);
    callback_.call_if(idx, value);
}

class VarsMenu
{
public:
    VarsMenu(VarsStorageBase& storage, SerialCommands& menu);

    String help();
    
private:
    void varCallback(SerialCommands* sender);
    static void cmdVarCallback(SerialCommands* sender, void* data);

private:
    VarsStorageBase& vars_;
};

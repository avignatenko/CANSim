#pragma once

#include "Common.h"
#include "FastDelegate.h"
#include "TaskErrorLed.h"

class MCP2515;

// CAM-Sim message format
// CAN id (high bit -> low bit, total 29 bits, only extended format supported):
// 4 bits: (25 .. 28) priority (0 .. 15, less is more priority)
// 5 bits: (20 .. 24) port
// 10 bits (10 .. 19): dst address (0 .. 1023)
// 10 bits (0 .. 9): src address (0 .. 1023)
// + CAN payload (0 - 8 bytes)
//
// Address reservation:
// 0 - unknown
// 1 - 15 master
// 16 - 1023 clients
class TaskCAN
{
public:
    TaskCAN(TaskErrorLed& taskErrorLed, Scheduler& sh, Pin& spiPort, Pin& intPort, uint16_t simaddress,
            bool receiveUnknown = false);

    void start();

    uint8_t sendMessage(byte priority, byte port, uint16_t dstSimAddress, byte len, byte* payload);

    void setReceiveCallback(fastdelegate::FastDelegate6<byte, byte, uint16_t, uint16_t, byte, byte*> callback);
    void setErrorCAllback(fastdelegate::FastDelegate1<byte> callback);
    uint16_t simAddress();
    void setSimAddress(uint16_t simAddress);

    void setReceiveUnknown(bool receiveUnknown);

private:
    void updateCANFilters();
    bool loopCANReceiveCallback();
    bool loopCANCheckCallback();

    void parseBuffer(uint32_t id, byte len, byte* buffer);

private:
    TaskErrorLed& taskErrorLed_;

    class TaskWithClassCallback : public Task
    {
    public:
        TaskWithClassCallback(TaskCAN* obj, bool (TaskCAN::*callback)(), unsigned long aInterval = 0,
                              long aIterations = 0, Scheduler* aScheduler = NULL, bool aEnable = false)
            : Task(aInterval, aIterations, aScheduler, aEnable), callback_(callback), obj_(obj)
        {
        }

        bool Callback() override { return (obj_->*callback_)(); }

    private:
        bool (TaskCAN::*callback_)() = nullptr;
        TaskCAN* obj_ = nullptr;
    };

    TaskWithClassCallback taskCANReceive_;
    TaskWithClassCallback taskCANCheckError_;
    MCP2515* mcp2515_;
    uint16_t simaddress_;  // 0 .. 1023
    Pin& intPort_;
    bool receiveUnknown_;

    fastdelegate::FastDelegate6<byte, byte, uint16_t, uint16_t, byte, byte*> callback_;
    fastdelegate::FastDelegate1<byte> errorCallback_;
};
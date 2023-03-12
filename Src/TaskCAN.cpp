#include "TaskCAN.h"

#include "mcp_can.h"

#include <etl/queue_spsc_isr.h>

namespace
{
struct Data
{
    uint32_t id;
    uint8_t len;
    byte buf[8];
};

struct InterruptControl
{
    static void lock() { noInterrupts(); }
    static void unlock() { interrupts(); }
};

constexpr uint8_t BUFFER_SIZE = 6;
etl::queue_spsc_isr<Data, BUFFER_SIZE, InterruptControl, etl::memory_model::MEMORY_MODEL_SMALL> s_buffer;
TaskCAN* s_instance = nullptr;

}  // namespace

void TaskCAN::onCANInterrupt()
{
    Data data;
    // read data,  len: data length, buf: data buf
    byte result = s_instance->mcpCAN_->readMsgBuf(&data.id, &data.len, data.buf);
    if (result != CAN_OK) return;
    s_buffer.push_from_isr(data);  // here we potentially drop messages
}

bool TaskCAN::loopCANReceiveCallback()
{
    Data val;
    if (s_buffer.pop(val))
    {
        parseBuffer(val.id & 0x1FFFFFFF, val.len, val.buf);
        return true;
    }
    else
        return false;
}

bool TaskCAN::loopCANCheckCallback()
{
    InterruptControl::lock();
    uint8_t error = mcpCAN_->getError();
    InterruptControl::unlock();

    if (error & MCP_EFLG_ERRORMASK)
    {
        if (errorCallback_) errorCallback_(error);
        taskErrorLed_.addError(TaskErrorLed::ERROR_CAN);
    }
    else
    {
        taskErrorLed_.removeError(TaskErrorLed::ERROR_CAN);
    }

    return true;
}

TaskCAN::TaskCAN(TaskErrorLed& taskErrorLed, Scheduler& sh, byte spiPort, byte intPort, uint16_t simaddress,
                 bool receiveUnknown)
    : taskErrorLed_(taskErrorLed),
      taskCANReceive_(this, &TaskCAN::loopCANReceiveCallback, TASK_IMMEDIATE, TASK_FOREVER, &sh, false),
      taskCANCheckError_(this, &TaskCAN::loopCANCheckCallback, 1000 * TASK_MILLISECOND, TASK_FOREVER, &sh, false),
      mcpCAN_(new MCP_CAN(spiPort)),
      simaddress_(simaddress),
      intPort_(intPort),
      receiveUnknown_(receiveUnknown)

{
    s_instance = this;
}

void TaskCAN::start()
{
    pinMode(intPort_, INPUT);

    int8_t retriesLeft = 5;
    for (; retriesLeft >= 0; --retriesLeft)
    {
        if (CAN_OK == mcpCAN_->begin(MCP_STDEXT, CAN_1000KBPS, MCP_8MHZ))  // init can bus : baudrate = 500k
        {
            break;
        }
        else
        {
            delay(500);
            continue;
        }
    }

    if (retriesLeft < 0)
    {
        taskErrorLed_.addError(TaskErrorLed::ERROR_CAN);
        return;
    }

    updateCANFilters();

    attachInterrupt(digitalPinToInterrupt(intPort_), &TaskCAN::onCANInterrupt, FALLING);

    mcpCAN_->setMode(MCP_NORMAL);

    taskCANReceive_.enable();
    taskCANCheckError_.enable();
}

void TaskCAN::updateCANFilters()
{
    uint32_t mask = 1023L << 10;
    uint32_t addrFilter = static_cast<uint32_t>(simaddress_) << 10;

    mcpCAN_->init_Mask(0, 1, mask);        // Init first mask...
    mcpCAN_->init_Filt(0, 1, addrFilter);  // Init first filter...
    mcpCAN_->init_Filt(1, 1, addrFilter);  // Init second filter...

    mcpCAN_->init_Mask(1, 1, mask);                              // Init second mask...
    mcpCAN_->init_Filt(2, 1, receiveUnknown_ ? 0 : addrFilter);  // Init third filter...
    mcpCAN_->init_Filt(3, 1, addrFilter);                        // Init fouth filter...
    mcpCAN_->init_Filt(4, 1, addrFilter);                        // Init fifth filter...
    mcpCAN_->init_Filt(5, 1, addrFilter);                        // Init sixth filter...

    mcpCAN_->setMode(MCP_NORMAL);
}

uint8_t TaskCAN::sendMessage(byte priority, byte port, uint16_t dstSimAddress, byte len, byte* payload)
{
    if (!taskCANReceive_.isEnabled()) return MCP2515_FAIL;

    uint32_t msg = 0;
    // 4 bits: (25 .. 28) priority (0 .. 15)
    msg |= (static_cast<uint32_t>(priority) & 0b1111) << 25;
    // 5 bits: (20 .. 24) port
    msg |= (static_cast<uint32_t>(port) & 0b11111) << 20;
    // 10 bits (10 .. 19): dst address (0 .. 1023)
    msg |= (static_cast<uint32_t>(dstSimAddress) & 0b1111111111) << 10;
    // 10 bits (0 .. 9): src address (0 .. 1023)
    msg |= (static_cast<uint32_t>(simaddress_) & 0b1111111111) << 0;

    InterruptControl::lock();
    uint8_t res = mcpCAN_->sendMsgBuf(msg, 1, len, payload);
    InterruptControl::unlock();
    return res;
}

void TaskCAN::setReceiveCallback(fastdelegate::FastDelegate6<byte, byte, uint16_t, uint16_t, byte, byte*> callback)
{
    callback_ = callback;
}

void TaskCAN::setErrorCAllback(fastdelegate::FastDelegate1<byte> callback)
{
    errorCallback_ = callback;
}

void TaskCAN::parseBuffer(uint32_t id, byte len, byte* buffer)
{
    // 4 bits: (25 .. 28) priority (0 .. 15)
    // 5 bits: (20 .. 24) port
    // 10 bits (10 .. 19): dst address (0 .. 1023)
    // 10 bits (0 .. 9): src address (0 .. 1023)
    uint16_t srcAddress = id & 0b1111111111;
    uint16_t dstAddress = (id >> 10) & 0b1111111111;

    uint16_t port = (id >> 20) & 0b11111;
    uint16_t priority = (id >> 25) & 0b1111;

    if (callback_) callback_(priority, port, srcAddress, dstAddress, len, buffer);
}

uint16_t TaskCAN::simAddress()
{
    return simaddress_;
}

void TaskCAN::setSimAddress(uint16_t simAddress)
{
    simaddress_ = simAddress;

    if (taskCANReceive_.isEnabled()) updateCANFilters();
}

void TaskCAN::setReceiveUnknown(bool receiveUnknown)
{
    receiveUnknown_ = receiveUnknown;
    if (taskCANReceive_.isEnabled()) updateCANFilters();
}

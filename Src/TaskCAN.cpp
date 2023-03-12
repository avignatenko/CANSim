#include "TaskCAN.h"

// #include "mcp_can.h"
#include "mcp2515.h"

volatile bool s_interrupt = false;
void irqHandler()
{
    s_interrupt = true;
}

bool TaskCAN::loopCANReceiveCallback()
{
    static can_frame frame;

#if 0
    if (mcp2515_->readMessage(&frame) == MCP2515::ERROR_OK)
    {
        Serial.print(frame.can_id, HEX);  // print ID
        Serial.print(" ");
        Serial.print(frame.can_dlc, HEX);  // print DLC
        Serial.print(" ");

        for (int i = 0; i < frame.can_dlc; i++)
        {  // print the data
            Serial.print(frame.data[i], HEX);
            Serial.print(" ");
        }

        Serial.println();
    }
#endif 

#if 1
    // check if data coming
    if (!digitalRead(intPort_))
    {
        s_interrupt = false;

        uint8_t irq = mcp2515_->getInterrupts();

        if (irq & MCP2515::CANINTF_RX0IF)
        {
            if (mcp2515_->readMessage(MCP2515::RXB0, &frame) == MCP2515::ERROR_OK)
            {
                // frame contains received from RXB0 message
                parseBuffer(frame.can_id & 0x1FFFFFFF, frame.can_dlc, frame.data);
            }
        }

        if (irq & MCP2515::CANINTF_RX1IF)
        {
            if (mcp2515_->readMessage(MCP2515::RXB1, &frame) == MCP2515::ERROR_OK)
            {
                // frame contains received from RXB1 message
                parseBuffer(frame.can_id & 0x1FFFFFFF, frame.can_dlc, frame.data);
            }
        }
    }

#endif
    return true;
}

bool TaskCAN::loopCANCheckCallback()
{
    byte error = mcp2515_->checkError();

    if (error != 0)
    {
        if (errorCallback_) errorCallback_(mcp2515_->getErrorFlags());
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
      mcp2515_(new MCP2515(spiPort)),
      simaddress_(simaddress),
      intPort_(intPort),
      receiveUnknown_(receiveUnknown)

{
}

void TaskCAN::start()
{
    pinMode(intPort_, INPUT);

    MCP2515::ERROR e = mcp2515_->reset();

    if (e != MCP2515::ERROR_OK)
    {
        taskErrorLed_.addError(TaskErrorLed::ERROR_CAN);
        return;
    }

    e = mcp2515_->setBitrate(CAN_1000KBPS, MCP_8MHZ);

    if (e != MCP2515::ERROR_OK)
    {
        taskErrorLed_.addError(TaskErrorLed::ERROR_CAN);
        return;
    }

    attachInterrupt(intPort_, irqHandler, FALLING);

    updateCANFilters();

    taskCANReceive_.enable();
    taskCANCheckError_.enable();
}

void TaskCAN::updateCANFilters()
{
    uint32_t mask = 1023L << 10;
    uint32_t addrFilter = static_cast<uint32_t>(simaddress_) << 10;

    mcp2515_->setFilterMask(MCP2515::MASK0, 1, mask);
    mcp2515_->setFilter(MCP2515::RXF0, 1, receiveUnknown_ ? 0 : addrFilter);
    mcp2515_->setFilter(MCP2515::RXF1, 1, addrFilter);

    mcp2515_->setFilterMask(MCP2515::MASK1, 1, mask);
    mcp2515_->setFilter(MCP2515::RXF2, 1, receiveUnknown_ ? 0 : addrFilter);
    mcp2515_->setFilter(MCP2515::RXF3, 1, addrFilter);
    mcp2515_->setFilter(MCP2515::RXF4, 1, addrFilter);
    mcp2515_->setFilter(MCP2515::RXF5, 1, addrFilter);

    MCP2515::ERROR e = mcp2515_->setNormalMode();

    if (e != MCP2515::ERROR_OK)
    {
        taskErrorLed_.addError(TaskErrorLed::ERROR_CAN);
        return;
    }
}

uint8_t TaskCAN::sendMessage(byte priority, byte port, uint16_t dstSimAddress, byte len, byte* payload)
{
    if (!taskCANReceive_.isEnabled()) return;

    uint32_t msg = 0;
    // 4 bits: (25 .. 28) priority (0 .. 15)
    msg |= (static_cast<uint32_t>(priority) & 0b1111) << 25;
    // 5 bits: (20 .. 24) port
    msg |= (static_cast<uint32_t>(port) & 0b11111) << 20;
    // 10 bits (10 .. 19): dst address (0 .. 1023)
    msg |= (static_cast<uint32_t>(dstSimAddress) & 0b1111111111) << 10;
    // 10 bits (0 .. 9): src address (0 .. 1023)
    msg |= (static_cast<uint32_t>(simaddress_) & 0b1111111111) << 0;

    static can_frame frame;
    frame.can_id = msg | CAN_EFF_FLAG;
    frame.can_dlc = len;
    ::memcpy(frame.data, payload, len);

    return mcp2515_->sendMessage(&frame);
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

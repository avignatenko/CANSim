#include "TaskEncoder.h"

#include <RotaryEncoder.h>

RotaryEncoder encoder(11, 12, RotaryEncoder::LatchMode::FOUR3);

bool TaskEncoder::Callback()
{
    encoder_->tick();

    if (!callback_) return false;

    RotaryEncoder::Direction dir = encoder_->getDirection();
    if (dir == RotaryEncoder::Direction::NOROTATION) return false;

    callback_(static_cast<uint8_t>(dir), encoder_->getMillisBetweenRotations());

    return true;
}

TaskEncoder::TaskEncoder(Scheduler& sh, byte pinA, byte pinB)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false),
      encoder_(new RotaryEncoder(pinA, pinB, RotaryEncoder::LatchMode::FOUR3))
{
}

void TaskEncoder::start()
{
    enable();
}

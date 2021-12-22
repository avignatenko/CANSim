#include "TaskStepperX27.h"

#include <SwitecX25.h>

bool TaskStepperX27::Callback()
{
    motor_->update();
    return true;
}

TaskStepperX27::TaskStepperX27(Scheduler& sh, byte p1, byte p2, byte p3, byte p4)
    : Task(1 * TASK_MILLISECOND, TASK_FOREVER,  &sh, false)
{
    // standard X25.168 range 315 degrees at 1/3 degree steps
    // FIXME: solder in proper way, to have A0, A1, A2, A3!
    // https://guy.carpenter.id.au/gaugette/2012/04/04/making-wiring-harnesses/
    motor_ = new SwitecX25(315 * 3, p1, p2, p3, p4);
}

void TaskStepperX27::start()
{
    // run the motor against the stops
    // motor_->zero();  // FIXME: need to impplement this async

    motor_->currentStep = motor_->steps - 1;
    for (unsigned int i = 0; i < motor_->steps; i++)
    {
        motor_->stepDown();
        delayMicroseconds(3000);
    }

    setPosition(0);

    enable();
}

void TaskStepperX27::setPosition(uint16_t steps)
{
    motor_->setPosition(steps);
}

uint16_t TaskStepperX27::totalSteps() const
{
    return motor_->steps;
}

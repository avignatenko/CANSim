#include "TaskStepperX27Driver.h"

#include <SwitecX12.h>

void TaskStepperX27Driver::loopStepperCallbackStatic()
{
    TaskStepperX27Driver& me = TaskStepperX27Driver::instance();
    me.loopStepperCallback();
}

void TaskStepperX27Driver::loopStepperCallback()
{
    motor_->update();
}

TaskStepperX27Driver* TaskStepperX27Driver::instance_ = nullptr;

TaskStepperX27Driver::TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset)
    : task_(800, TASK_FOREVER, &loopStepperCallbackStatic, &sh, false)
{
    Log.traceln("TaskStepper::TaskStepper");

    // standard X25.168 range 315 degrees at 1/3 degree steps, but 1/12 degree with driver
    motor_ = new SwitecX12(315 * 12, step, dir);
    resetPin_ = reset;
}

void TaskStepperX27Driver::init(Scheduler& sh, byte step, byte dir, byte reset)
{
    instance_ = new TaskStepperX27Driver(sh, step, dir, reset);
}

void TaskStepperX27Driver::start()
{
    digitalWrite(resetPin_, HIGH);

    // fixme: why 300microsec don't work?

    motor_->currentStep = motor_->steps - 1;
    int dir = -1;
    int count = motor_->currentStep - 0;

    for (int i = 0; i < count; i++)
    {
        motor_->step(dir);
        delayMicroseconds(800);
    }

    motor_->targetStep = 0;
    motor_->vel = 0;
    motor_->dir = 0;

    setPosition(0);

    task_.enable();
}

void TaskStepperX27Driver::setPosition(uint16_t steps)
{
    Log.traceln("TaskStepperX27Driver::setPosition %d", steps);
    motor_->setPosition(steps);
}

uint16_t TaskStepperX27Driver::position() const
{
    return motor_->targetStep;
}

uint16_t TaskStepperX27Driver::totalSteps() const
{
    return motor_->steps;
}

TaskStepperX27Driver& TaskStepperX27Driver::instance()
{
    return *instance_;
}

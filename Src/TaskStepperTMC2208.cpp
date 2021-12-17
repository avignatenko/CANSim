#include "TaskStepperTMC2208.h"

#include <AccelStepper.h>

void TaskStepperTMC2208::loopStepperCallbackStatic()
{
    TaskStepperTMC2208& me = TaskStepperTMC2208::instance();
    me.loopStepperCallback();
}

void TaskStepperTMC2208::loopStepperCallback()
{
    motor_->run();
}

TaskStepperTMC2208* TaskStepperTMC2208::instance_ = nullptr;

TaskStepperTMC2208::TaskStepperTMC2208(Scheduler& sh, byte step, byte dir, byte reset, float speed, float acceleration)
    : task_(TASK_IMMEDIATE, TASK_FOREVER, &loopStepperCallbackStatic, &sh, false)
{
    motor_ = new AccelStepper(AccelStepper::DRIVER, step, dir);

    motor_->setPinsInverted(true, false, true);  // invert direction
    motor_->setEnablePin(reset);
}

void TaskStepperTMC2208::init(Scheduler& sh, byte step, byte dir, byte reset, float speed, float acceleration)
{
    instance_ = new TaskStepperTMC2208(sh, step, dir, reset, speed, acceleration);
}

void TaskStepperTMC2208::start()
{
   
    motor_->enableOutputs();

    constexpr int kMaxSpeed = 20000;  // steps per second
    constexpr int kAcceleration = 5000;

    motor_->setCurrentPosition(0);

    // set normal speed & acceleration
    motor_->setMaxSpeed(kMaxSpeed);
    motor_->setAcceleration(kAcceleration);

    task_.enable();
}

void TaskStepperTMC2208::setPosition(int32_t steps)
{
    motor_->moveTo(steps);
}

void TaskStepperTMC2208::resetPosition(int32_t position)
{
    motor_->setCurrentPosition(position);
}

int32_t TaskStepperTMC2208::position() const
{
    return motor_->currentPosition();
}

TaskStepperTMC2208& TaskStepperTMC2208::instance()
{
    return *instance_;
}

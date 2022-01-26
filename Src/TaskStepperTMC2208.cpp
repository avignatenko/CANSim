#include "TaskStepperTMC2208.h"

#include <AccelStepper.h>

bool TaskStepperTMC2208::Callback()
{
    motor_->run();
    return true;
}

TaskStepperTMC2208::TaskStepperTMC2208(Scheduler& sh, byte step, byte dir, byte reset)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false)
{
    motor_ = new AccelStepper(AccelStepper::DRIVER, step, dir);

    motor_->setPinsInverted(true, false, true);  // invert direction
    motor_->setEnablePin(reset);
}

void TaskStepperTMC2208::start()
{
   
    motor_->enableOutputs();

    constexpr int kMaxSpeed = 2500;  // steps per second
    constexpr int kAcceleration = 5000;

    motor_->setCurrentPosition(0);

    // set normal speed & acceleration
    motor_->setMaxSpeed(kMaxSpeed);
    motor_->setAcceleration(kAcceleration);

    enable();
}

void TaskStepperTMC2208::setPosition(int32_t steps)
{
    motor_->moveTo(steps);
}

void TaskStepperTMC2208::stop()
{
    motor_->stop();
}

void TaskStepperTMC2208::resetPosition(int32_t position)
{
    motor_->setCurrentPosition(position);
}

int32_t TaskStepperTMC2208::position() const
{
    return motor_->currentPosition();
}

int32_t TaskStepperTMC2208::targetPosition() const
{
    return motor_->targetPosition();
}

void TaskStepperTMC2208::setSpeed(float speed)
{
    motor_->setMaxSpeed(speed);
}

void TaskStepperTMC2208::setAcceleration(float acceleration)
{
    motor_->setAcceleration(acceleration);
}
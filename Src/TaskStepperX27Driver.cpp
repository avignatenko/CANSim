#include "TaskStepperX27Driver.h"

#include <AccelStepper.h>

constexpr int kTotalSteps = 315 * 12;

bool TaskStepperX27Driver::Callback()
{
    motor_->run();
    return true;
}

TaskStepperX27Driver::TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false)
{
    // standard X25.168 range 315 degrees at 1/3 degree steps, but 1/12 degree with driver
    motor_ = new AccelStepper(AccelStepper::DRIVER, step, dir);

    motor_->setPinsInverted(true);  // invert direction
    motor_->setEnablePin(reset);
}

void TaskStepperX27Driver::start()
{
    motor_->enableOutputs();

    // per datasheet speed limit 600 deg / second = 600 * 12 steps / second
    constexpr int kMaxSpeed = 600 * 12 * 0.7;  // steps per second
    constexpr int kAcceleration = 9000;

    // set calibration speed & accelration
    motor_->setMaxSpeed(kMaxSpeed / 3);
    motor_->setAcceleration(kAcceleration / 2);

    motor_->setCurrentPosition(kTotalSteps - 1);
    motor_->moveTo(0);
    motor_->runToPosition();

    // set normal speed & acceleration
    motor_->setMaxSpeed(kMaxSpeed);
    motor_->setAcceleration(kAcceleration);

    enable();
}

void TaskStepperX27Driver::setPosition(int16_t steps)
{
    motor_->moveTo(constrain(steps, 0, kTotalSteps - 1));
}

int16_t TaskStepperX27Driver::position() const
{
    return motor_->currentPosition();
}

uint16_t TaskStepperX27Driver::totalSteps() const
{
    return kTotalSteps;
}
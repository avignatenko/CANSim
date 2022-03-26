#include "TaskStepperX27Driver.h"

#include <AccelStepper.h>

constexpr int kTotalSteps = 315 * 12;

bool TaskStepperX27Driver::Callback()
{
    motor_->run();
    return true;
}

TaskStepperX27Driver::TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset, bool limit, bool invertDir)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false), limit_(limit)
{
    // standard X25.168 range 315 degrees at 1/3 degree steps, but 1/12 degree with driver
    motor_ = new AccelStepper(AccelStepper::DRIVER, step, dir);

    motor_->setPinsInverted(invertDir);  // invert direction
    motor_->setEnablePin(reset);

    // per datasheet speed limit 600 deg / second = 600 * 12 steps / second
    constexpr int kMaxSpeed = 600 * 12 * 0.7;  // steps per second
    constexpr int kAcceleration = 9000;
    maxSpeed_ = kMaxSpeed;
    maxAcceleration_ = kAcceleration;
}

void TaskStepperX27Driver::start()
{
    motor_->enableOutputs();

    motor_->setCurrentPosition(0);

    if (limit_)
    {
        // set calibration speed & accelration
        motor_->setMaxSpeed(maxSpeed_ / 3);
        motor_->setAcceleration(maxAcceleration_ / 2);

        motor_->setCurrentPosition(kTotalSteps - 1);
        motor_->moveTo(0);
        motor_->runToPosition();
    }

    // set normal speed & acceleration
    motor_->setMaxSpeed(maxSpeed_);
    motor_->setAcceleration(maxAcceleration_);

    enable();
}

void TaskStepperX27Driver::setMaxSpeed(int maxSpeed)
{
    maxSpeed_ = maxSpeed;
    motor_->setMaxSpeed(maxSpeed_);
}

void TaskStepperX27Driver::setMaxAcceleration(int maxAcceleration)
{
    maxAcceleration_ = maxAcceleration;
    motor_->setAcceleration(maxAcceleration_);
}

void TaskStepperX27Driver::resetPosition(int16_t position)
{
    motor_->setCurrentPosition(position);
}

void TaskStepperX27Driver::setPosition(int16_t steps)
{
    motor_->moveTo(limit_ ? constrain(steps, 0, kTotalSteps - 1) : steps);
}

int16_t TaskStepperX27Driver::position() const
{
    return motor_->currentPosition();
}

int16_t TaskStepperX27Driver::targetPosition() const
{
    return motor_->targetPosition();
}

uint16_t TaskStepperX27Driver::totalSteps() const
{
    return kTotalSteps;
}

void TaskStepperX27Driver::stop()
{
    motor_->stop();
}

#include "TaskStepperX27Driver.h"

#include <AccelStepper.h>

constexpr int kTotalSteps = 315 * 12;

void TaskStepperX27Driver::loopStepperCallbackStatic()
{
    TaskStepperX27Driver& me = TaskStepperX27Driver::instance();
    me.loopStepperCallback();
}

void TaskStepperX27Driver::loopStepperCallback()
{
    motor_->run();
}

TaskStepperX27Driver* TaskStepperX27Driver::instance_ = nullptr;

TaskStepperX27Driver::TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset)
    : task_(TASK_IMMEDIATE, TASK_FOREVER, &loopStepperCallbackStatic, &sh, false)
{
    Log.traceln("TaskStepper::TaskStepper");

    // standard X25.168 range 315 degrees at 1/3 degree steps, but 1/12 degree with driver
    motor_ = new AccelStepper(AccelStepper::DRIVER, step, dir);

    resetPin_ = reset;
}

void TaskStepperX27Driver::init(Scheduler& sh, byte step, byte dir, byte reset)
{
    instance_ = new TaskStepperX27Driver(sh, step, dir, reset);
}

void TaskStepperX27Driver::start()
{
    // enable driver
    digitalWrite(resetPin_, HIGH);

    motor_->setEnablePin(resetPin_);

    motor_->enableOutputs();
    motor_->setPinsInverted(true);  // invert direction

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

    task_.enable();
}

void TaskStepperX27Driver::setPosition(uint16_t steps)
{
    Log.traceln("TaskStepperX27Driver::setPosition %d", steps);
    motor_->moveTo(constrain(steps, 0, kTotalSteps - 1));
}

uint16_t TaskStepperX27Driver::position() const
{
    return motor_->currentPosition();
}

uint16_t TaskStepperX27Driver::totalSteps() const
{
    return kTotalSteps;
}

TaskStepperX27Driver& TaskStepperX27Driver::instance()
{
    return *instance_;
}

#include "TaskStepperX27.h"

#include <AccelStepper.h>

void TaskStepperX27::loopStepperCallbackStatic()
{
    TaskStepperX27& me = TaskStepperX27::instance();
    me.loopStepperCallback();
}

void TaskStepperX27::loopStepperCallback()
{
    motor_->run();
}

const int kMaxSteps = 315 * 4;

TaskStepperX27* TaskStepperX27::instance_ = nullptr;

TaskStepperX27::TaskStepperX27(Scheduler& sh, byte p1, byte p2, byte p3, byte p4)
    : task_(0, TASK_FOREVER, &loopStepperCallbackStatic, &sh, false)
{
    Log.traceln("TaskStepper::TaskStepper");

    // standard X25.168 range 315 degrees at 1/3 degree steps
    // FIXME: solder in proper way, to have A0, A1, A2, A3!
    // https://guy.carpenter.id.au/gaugette/2012/04/04/making-wiring-harnesses/
    // motor_ = new SwitecX25(315 * 3, p1, p2, p3, p4);
    motor_ = new AccelStepper(AccelStepper::HALF4WIRE, p2, p1, p4, p3);
    // motor_ = new AccelStepper(AccelStepper::HALF4WIRE, p1, p2, p3, p4);
}

void TaskStepperX27::init(Scheduler& sh, byte p1, byte p2, byte p3, byte p4)
{
    instance_ = new TaskStepperX27(sh, p1, p2, p3, p4);
}

void TaskStepperX27::start()
{
    // run the motor against the stops
    // motor_->zero();  // FIXME: need to impplement this async

    motor_->setMaxSpeed(500);
    motor_->setAcceleration(2000);
    motor_->setCurrentPosition(kMaxSteps - 1);
    motor_->runToNewPosition(0);

    motor_->setMaxSpeed(1500);

    task_.enable();
}

void TaskStepperX27::setPosition(uint16_t position)
{
    Log.traceln("TaskStepperX27::setPosition %d", position);

    int steps = (uint32_t)position * (kMaxSteps - 1) / 65535;
    Log.traceln("TaskStepperX27::setPosition steps %d", steps);
    motor_->moveTo(steps);
}

TaskStepperX27& TaskStepperX27::instance()
{
    return *instance_;
}

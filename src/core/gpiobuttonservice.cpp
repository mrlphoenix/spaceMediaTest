#include "gpiobuttonservice.h"
#include <wiringPi.h>
#include <QDebug>

#include <fcntl.h>

bool GPIOButtonService::checkPin(int pinId)
{
    return digitalRead(pinId) ;
}

void GPIOButtonService::checkPinSlot()
{
    bool currentResult = !checkPin(9);
    if (currentResult && !prevState)
        emit buttonPressed();
    prevState = currentResult;
}

GPIOButtonService::GPIOButtonService(QObject *parent) : QObject(parent)
{
    prevState = false;
}

/*
InputService::InputService(QObject *parent)
{

}

void InputService::run()
{

}
*/

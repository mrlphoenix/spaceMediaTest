#include "gpiobuttonservice.h"
#include <wiringPi.h>
#include <QDebug>

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

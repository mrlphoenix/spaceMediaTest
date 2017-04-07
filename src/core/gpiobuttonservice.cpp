#include "gpiobuttonservice.h"
#include <wiringPi.h>
#include <QDebug>
#include <QDateTime>

#include <fcntl.h>
#include <stdio.h>
#include <linux/input.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <QFileInfo>

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


InputService::InputService(QObject *parent)
{

}

void InputService::run()
{
    qDebug() << "Inputservice::run";
    int ff = open("/dev/input/event0", O_RDONLY);
    input_event event;
    while (1)
    {
        read(ff,&event,sizeof(input_event));
        if (event.value < 2 && event.type == 1)
        {
            state[event.code] = event.value;
            if (event.value)
                emit keyDown(event.code);
            else
                emit keyUp(event.code);

          //  qDebug() << "InputService::run" << event.code << event.value;
        }
    }
}



void InputDeviceControlService::run()
{
    qDebug() << "InputDeviceControlService::run";
    while (1)
    {
        if (QFileInfo::exists("/dev/input/event0"))
        {
            if (!deviceFound)
            {
                deviceFound = true;
                emit deviceConnected();
            }
        }
        else
        {
            if (deviceFound)
            {
                deviceFound = false;
                emit deviceDisconnected();
            }
        }
        this->thread()->sleep(1);
    }
}

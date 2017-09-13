#ifndef GPIOBUTTONSERVICE_H
#define GPIOBUTTONSERVICE_H

#include <QObject>
#include <QThread>
#include <QHash>

class GPIOButtonService : public QObject
{
    Q_OBJECT
public:
    GPIOButtonService(QObject * parent = 0);
    ~GPIOButtonService(){}
    bool checkPin(int);
public slots:
    void checkPinSlot();
signals:
    void buttonPressed();
private:
    bool prevState;
};

class InputService : public QObject
{
    Q_OBJECT
public:
    InputService(QObject * parent = 0);
    ~InputService(){}
signals:
    void keyDown(int keyId);
    void keyUp(int keyId);
    void keyPressed(int keyId, int isDown);
public slots:
    void run();
private:
    QHash<int, int> state;
};

class InputDeviceControlService : public QObject
{
    Q_OBJECT
public:
    InputDeviceControlService(QObject * parent = 0):QObject(parent){deviceFound = false;}
    ~InputDeviceControlService(){}
signals:
    void deviceConnected();
    void deviceDisconnected();
public slots:
    void run();
private:
    bool deviceFound;
};

class KeyboardComboService : public QObject
{
    Q_OBJECT
public:
    KeyboardComboService(QObject * parent):QObject(parent) {}
    ~KeyboardComboService(){}
    bool testCombo(QList<int> codes);
private:
    QHash<int, int> keysState;
};

#endif // GPIOBUTTONSERVICE_H

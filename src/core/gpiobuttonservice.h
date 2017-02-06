#ifndef GPIOBUTTONSERVICE_H
#define GPIOBUTTONSERVICE_H

#include <QObject>
#include <QThread>

class GPIOButtonService : public QObject
{
    Q_OBJECT
public:
    GPIOButtonService(QObject * parent = 0);
    ~GPIOButtonService(){}
    bool checkPin(int pinId);
public slots:
    void checkPinSlot();
signals:
    void buttonPressed();
private:
    bool prevState;
};

/*
class InputService : public QObject
{
    Q_OBJECT
public:
    InputService(QObject * parent = 0);
    ~InputService(){}
    void run();
signals:
    void keyDown(int keyId);
    void keyUp(int keyId);
    void keyPressed(int keyId, int isDown);
    //
    QHash<int, int> state;
};
*/
#endif // GPIOBUTTONSERVICE_H

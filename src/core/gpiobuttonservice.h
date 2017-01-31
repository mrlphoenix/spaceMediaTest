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
#endif // GPIOBUTTONSERVICE_H

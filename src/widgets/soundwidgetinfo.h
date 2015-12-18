#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QString>
class SoundWidgetInfo
{
public:
    SoundWidgetInfo();
    virtual ~SoundWidgetInfo(){;}
    virtual void setSound(int){;}
    QString getId() const {return id;}
    void setId(QString id){this->id = id;}
private:
    QString id;
};

#endif // ABSTRACTWIDGET_H

#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QString>
class IWidgetInfo
{
public:
    IWidgetInfo();
    virtual ~IWidgetInfo(){;}
    virtual void setSound(int volume)=0;
    QString getId() const {return id;}
    void setId(QString id){this->id = id;}
private:
    QString id;
};

#endif // ABSTRACTWIDGET_H

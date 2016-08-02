#ifndef STATICTEXT_H
#define STATICTEXT_H

#include <QHash>
#include <QObject>

#define StaticTextService Singleton<StaticText>::instance()

class StaticText : public QObject
{
    Q_OBJECT
public:
    StaticText();
    StaticText(QObject * parent);
    ~StaticText(){}

    void init();
    void setLang(QString lang);
    QString getLang(){return lang;}

    QString getEula();
    QString getEula(QString lang);

    QString getPrivacyPolicy();
    QString getPrivacyPolicy(QString lang);

    QString getOpenSource();
    QString getOpenSource(QString lang);

    QString getLegal();
    QString getLegal(QString lang);
private:
    QString lang;
    QHash<QString, QString> eula;
    QHash<QString, QString> privacyPolicy;
    QHash<QString, QString> openSource;
    QHash<QString, QString> legal;
};

#endif // STATICTEXT_H

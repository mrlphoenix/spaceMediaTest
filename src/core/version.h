#ifndef VERSION_H
#define VERSION_H
#include <QString>

//this header is for automatic version builder
//number BUILD is incremented every time we build prject using versionbuild tool
//you can extend this as you want but string with build should be the same
namespace TeleDSVersion
{
    static const int MAJOR = 1;
    static const int MINOR = 0;
    static const int RELEASE = 20;
	static const int BUILD = 1229;
    QString getVersion()
    {
        return QString::number(MAJOR) + "." + QString::number(MINOR) + "." + QString::number(RELEASE) + "/" + QString::number(BUILD);
        //return "TeleDS v" + QString::number(MAJOR) + "." + QString::number(MINOR) + "/" + QString::number(BUILD);
    }
}

#endif // VERSION_H

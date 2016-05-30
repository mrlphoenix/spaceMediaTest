#ifndef VERSION_H
#define VERSION_H
#include <QString>

namespace TeleDSVersion
{
    static const int MAJOR = 0;
    static const int MINOR = 1;
	static const int BUILD = 19;
    QString getVersion()
    {
        return "TeleDS v" + QString::number(MAJOR) + "." + QString::number(MINOR) + "." + QString::number(BUILD);
    }
}

#endif // VERSION_H





























#ifndef VERSION_H
#define VERSION_H
#include <QString>

//this header is for automatic version builder
//number BUILD is incremented every time we build prject using versionbuild tool
//you can extend this as you want but string with build should be the same
class TeleDSVersion
{
public:
    static const int MAJOR = 1;
    static const int MINOR = 0;
    static const int RELEASE = 20;
    static const int BUILD = 1518;
    static QString getVersion()
    {
        return QString::number(MAJOR) + "." + QString::number(MINOR) + "." + QString::number(RELEASE) + "/" + QString::number(BUILD);
        //return "TeleDS v" + QString::number(MAJOR) + "." + QString::number(MINOR) + "/" + QString::number(BUILD);
    }

    static int compareVersion(int srcMajor, int srcMinor, int srcRelease, int srcBuild)
    {
        if (srcMajor > MAJOR)
            return 1;

        if (srcMajor == MAJOR)
        {
            if (srcMinor > MINOR)
                return 1;

            if (srcMinor == MINOR)
            {
                if (srcRelease > RELEASE)
                    return 1;

                if (srcRelease == RELEASE)
                {
                    if (srcBuild > BUILD)
                        return 1;
                    if (srcBuild == BUILD)
                        return 0;
                }
            }
        }
        return -1;
    }
};

#endif // VERSION_H

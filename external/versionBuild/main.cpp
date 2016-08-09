#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDebug>


void processBuildNumber(QString prefix)
{
    QString versionFileLocation = prefix + "src/core/version.h";
    QFile versionFile(versionFileLocation);
    if (versionFile.open(QFile::ReadOnly))
    {
        QString data = versionFile.readAll();
        QStringList strings = data.split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            qDebug() << strings[i];
            QString currentString = strings[i];
            if (currentString.indexOf("static const int BUILD") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int buildNumber = tokens[5].toInt();
                    buildNumber++;
                    strings[i] = "\tstatic const int BUILD = " + QString::number(buildNumber) + ";";
                    qDebug() << "Build string =" + strings[i];
                    break;
                }
            }
        }
        versionFile.close();
        QFile outFile(versionFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
}

void processReleaseNumber(QString prefix)
{
    QString versionFileLocation = prefix + "src/core/version.h";
    QFile versionFile(versionFileLocation);
    if (versionFile.open(QFile::ReadOnly))
    {
        QString data = versionFile.readAll();
        QStringList strings = data.split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            qDebug() << strings[i];
            QString currentString = strings[i];
            if (currentString.indexOf("static const int RELEASE") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int releaseNumber = tokens[5].toInt();
                    releaseNumber++;
                    strings[i] = "\tstatic const int RELEASE = " + QString::number(releaseNumber) + ";";
                    qDebug() << "Release string =" + strings[i];
                }
            }
            else if (currentString.indexOf("static const int BUILD") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int buildNumber = tokens[5].toInt();
                    buildNumber++;
                    strings[i] = "\tstatic const int BUILD = " + QString::number(buildNumber) + ";";
                    qDebug() << "Build string =" + strings[i];
                    break;
                }
            }
        }
        versionFile.close();
        QFile outFile(versionFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
}


void processMinorNumber(QString prefix)
{
    QString versionFileLocation = prefix + "src/core/version.h";
    QFile versionFile(versionFileLocation);
    if (versionFile.open(QFile::ReadOnly))
    {
        QString data = versionFile.readAll();
        QStringList strings = data.split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            qDebug() << strings[i];
            QString currentString = strings[i];

            if (currentString.indexOf("static const int MINOR") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int minorNumber = tokens[5].toInt();
                    minorNumber++;
                    strings[i] = "\tstatic const int MINOR = " + QString::number(minorNumber) + ";";
                    qDebug() << "Release string =" + strings[i];
                }
            }
            else if (currentString.indexOf("static const int RELEASE") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int releaseNumber = tokens[5].toInt();
                    releaseNumber++;
                    strings[i] = "\tstatic const int RELEASE = " + QString::number(releaseNumber) + ";";
                    qDebug() << "Release string =" + strings[i];
                }
            }
            else if (currentString.indexOf("static const int BUILD") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int buildNumber = tokens[5].toInt();
                    buildNumber++;
                    strings[i] = "\tstatic const int BUILD = " + QString::number(buildNumber) + ";";
                    qDebug() << "Build string =" + strings[i];
                    break;
                }
            }
        }
        versionFile.close();
        QFile outFile(versionFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
}

void processMajorNumber(QString prefix)
{
    QString versionFileLocation = prefix + "src/core/version.h";
    QFile versionFile(versionFileLocation);
    if (versionFile.open(QFile::ReadOnly))
    {
        QString data = versionFile.readAll();
        QStringList strings = data.split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            qDebug() << strings[i];
            QString currentString = strings[i];
            if (currentString.indexOf("static const int MAJOR") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int majorNumber = tokens[5].toInt();
                    majorNumber++;
                    strings[i] = "\tstatic const int MAJOR = " + QString::number(majorNumber) + ";";
                    qDebug() << "Major string =" + strings[i];
                }
            }
            if (currentString.indexOf("static const int MINOR") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int minorNumber = tokens[5].toInt();
                    minorNumber++;
                    strings[i] = "\tstatic const int MINOR = " + QString::number(minorNumber) + ";";
                    qDebug() << "Minor string =" + strings[i];
                }
            }
            else if (currentString.indexOf("static const int RELEASE") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int releaseNumber = tokens[5].toInt();
                    releaseNumber++;
                    strings[i] = "\tstatic const int RELEASE = " + QString::number(releaseNumber) + ";";
                    qDebug() << "Release string =" + strings[i];
                }
            }
            else if (currentString.indexOf("static const int BUILD") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    int buildNumber = tokens[5].toInt();
                    buildNumber++;
                    strings[i] = "\tstatic const int BUILD = " + QString::number(buildNumber) + ";";
                    qDebug() << "Build string =" + strings[i];
                    break;
                }
            }
        }
        versionFile.close();
        QFile outFile(versionFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
}

void processPlatform(QString prefix, QString platform)
{
    QString projectFileLocation = prefix + "app/TeleDS.pro";
    QString platformDefinesLocation = prefix + "/src/core/platformdefines.h";
    QString browserFileLocation = prefix + "app/AndroidBrowser.qml";
    QString sideBrowserLocation = prefix + "app/SideBrowser.qml";

    QFile projectFile(projectFileLocation);
    if (projectFile.open(QFile::ReadOnly))
    {
        QStringList strings = QString(projectFile.readAll()).split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            if (strings[i].indexOf("KIT = ") == 0)
            {
                if (platform == "rpi")
                    strings[i] = "KIT = Raspberry";
                else if (platform == "android")
                    strings[i] = "KIT = Android";
                else if (platform == "linux")
                    strings[i] = "KIT = Linux";
                break;
            }
        }
        projectFile.close();
        QFile outFile(projectFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
    QFile platformDefinesFile(platformDefinesLocation);
    if (platformDefinesFile.open(QFile::ReadOnly))
    {
        QStringList strings = QString(platformDefinesFile.readAll()).split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            if (strings[i].indexOf("#define PLATFORM_DEFINE_") == 0)
            {
                if (platform == "rpi")
                    strings[i] = "#define PLATFORM_DEFINE_RPI";
                else if (platform == "linux")
                    strings[i] = "#define PLATFORM_DEFINE_LINUX";
                else if (platform == "android")
                    strings[i] = "#define PLATFORM_DEFINE_ANDROID";
                else if (platform == "windows")
                    strings[i] = "#define PLATFORM_DEFINE_WINDOWS";
                break;
            }
        }
        platformDefinesFile.close();
        QFile outFile(platformDefinesLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
    QFile browserFile(browserFileLocation);
    if (browserFile.open(QFile::ReadOnly))
    {
        QStringList strings = QString(browserFile.readAll()).split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            if (platform == "rpi" || platform == "windows" || platform == "linux")
            {
                if (strings[i].indexOf("import QtWebView 1.1") == 0)
                    strings[i] = "//import QtWebView 1.1";
                else if (strings[i].indexOf("//import QtWebKit 3.0") == 0)
                {
                    strings[i] = "import QtWebKit 3.0";
                    break;
                }
            }
            else if (platform == "android")
            {
                if (strings[i].indexOf("//import QtWebView 1.1") == 0)
                    strings[i] = "import QtWebView 1.1";
                else if (strings[i].indexOf("import QtWebKit 3.0") == 0)
                {
                    strings[i] = "//import QtWebKit 3.0";
                    break;
                }
            }
        }
        browserFile.close();
        QFile outFile(browserFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
    QFile sideBrowserFile(sideBrowserLocation);
    if (sideBrowserFile.open(QFile::ReadOnly))
    {
        QStringList strings = QString(sideBrowserFile.readAll()).split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            if (platform == "rpi" || platform == "windows" || platform == "linux")
            {
                if (strings[i].indexOf("import QtWebView 1.1") == 0)
                    strings[i] = "//import QtWebView 1.1";
                else if (strings[i].indexOf("//import QtWebKit 3.0") == 0)
                {
                    strings[i] = "import QtWebKit 3.0";
                    break;
                }
            }
            else if (platform == "android")
            {
                if (strings[i].indexOf("//import QtWebView 1.1") == 0)
                    strings[i] = "import QtWebView 1.1";
                else if (strings[i].indexOf("import QtWebKit 3.0") == 0)
                {
                    strings[i] = "//import QtWebKit 3.0";
                    break;
                }
            }
        }
        sideBrowserFile.close();
        QFile outFile(sideBrowserLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
}

void processAndroidManifest(QString prefix)
{
    QString versionFileLocation = prefix + "src/core/version.h";
    QString manifestFileLocation = prefix + "app/android/AndroidManifest.xml";
    QFile versionFile(versionFileLocation);
    int versionMajor = 1, versionMinor = 0, versionRelease = 0, versionBuild = 1;

    if (versionFile.open(QFile::ReadOnly))
    {
        QString data = versionFile.readAll();
        QStringList strings = data.split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            qDebug() << strings[i];
            QString currentString = strings[i];
            if (currentString.indexOf("static const int MAJOR") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    versionMajor = tokens[5].toInt();
                }
            }
            else if (currentString.indexOf("static const int MINOR") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    versionMinor = tokens[5].toInt();
                }
            }
            else if (currentString.indexOf("static const int RELEASE") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    versionRelease = tokens[5].toInt();
                }
            }
            else if (currentString.indexOf("static const int BUILD") != -1)
            {
                qDebug() << "FOUND";
                QString temp = currentString;
                temp.replace(";","");
                QStringList tokens = temp.simplified().split(" ");
                if (tokens.count() >= 5)
                {
                    qDebug() << tokens[5];
                    versionBuild = tokens[5].toInt();
                    break;
                }
            }
        }
    }
    versionFile.close();
    QString version = QString::number(versionMajor) + "." + QString::number(versionMinor) + "." + QString::number(versionRelease);
    QString buildNumber = QString::number(versionBuild);

    QFile manifestFile(manifestFileLocation);
    if (manifestFile.open(QFile::ReadOnly))
    {
        QStringList strings = QString(manifestFile.readAll()).split("\n");
        for (int i = 0; i < strings.count(); ++i)
        {
            if (strings[i].indexOf("versionName=\"") != -1)
            {
                int beginIndex = strings[i].indexOf("versionName=\"") + QString("versionName=\"").length();
                int endIndex = strings[i].indexOf("android:versionCode") - 2;
                int codeEndIndex = strings[i].indexOf("android:installLocation") - 2;

                QString head = strings[i].mid(0, beginIndex);
                QString middle = strings[i].mid(endIndex, QString("\" android:versionCode=\"").length());
                QString tail = strings[i].mid(codeEndIndex);
                strings[i] = head + version + middle + buildNumber + tail;
                break;
            }
        }
        manifestFile.close();
        QFile outFile(manifestFileLocation);
        if (outFile.open(QFile::WriteOnly))
        {
            for (int i = 0; i < strings.count(); ++i)
            {
                if (i == strings.count() - 1)
                    outFile.write(strings[i].toUtf8());
                else
                    outFile.write(QString(strings[i]+ "\n").toUtf8());
            }
            outFile.flush();
            outFile.close();
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc < 4)
        return 0;
    QString platform = argv[1];
    QString build_type = argv[2];
    QString prefix = argv[3];
    qDebug() << platform << build_type << prefix;

    if (prefix != "")
    {
        if (platform != "none")
            processPlatform(prefix,platform);
        if (build_type == "build")
            processBuildNumber(prefix);
        else if (build_type == "release")
            processReleaseNumber(prefix);
        else if (build_type == "minor")
            processMinorNumber(prefix);
        else if (build_type == "major")
            processMajorNumber(prefix);
        if (platform == "android" && build_type != "build" && build_type != "none")
            processAndroidManifest(prefix);
    }
    return 0;
}


/*
vb [platform] [build_type] [path_to_project]

platform:= rpi | android | linux | windows | none
build_type:= build | release | minor | major | none

build - increase only build number
release - increase build number and release number
minor - increase build number and minor number, sets release to 0
major - increase build number and major number, sets release to 0, sets  minor to 0
*/

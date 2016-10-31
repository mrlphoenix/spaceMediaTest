#include <QCoreApplication>
#include <QString>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include "notherfilesystem.h"

int main(int argc, char *argv[])
{
    qDebug() << "Updater Started!!!Bitch";
    //IN: tdsu file
    if (argc < 2)
    {
        qDebug() << "wrong arguments!" << argc;
        return 0;
    }

    NotherFileSystem nfs;
    nfs.load(QString(argv[1]));
    foreach (const QString &d, nfs.dirs())
        QDir().mkpath(d);

   // QProcess stopProc;
   // stopProc.start("sudo service videoplayer stop");
   // stopProc.waitForFinished();
   // qDebug() << "stopss" << stopProc.readAllStandardError() << stopProc.readAllStandardOutput();

    if (nfs.fileExists("system::player"))
    {
        QByteArray data = nfs.getFile("system::player");
        QFile::remove("TeleDSPlayer");
        QFile f("TeleDSPlayer");
        if (f.open(QFile::WriteOnly))
        {
            f.write(data);
            f.flush();

            f.setPermissions(QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther | QFile::ExeUser |
                             QFile::ReadOwner| QFile::ReadUser | QFile::ReadOther | QFile::ReadGroup |
                             QFile::WriteGroup | QFile::WriteOwner | QFile::WriteOther | QFile::WriteUser);
            f.close();
        }
    }

    foreach (const QString &filename, nfs.files())
    {
        if (filename != "system::player" &&
            filename != "system::updater")
        {
            QByteArray data = nfs.getFile(filename);
            QFile f(filename);
            if (f.open(QFile::WriteOnly))
            {
                f.write(data);
                f.flush();

                f.setPermissions(QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther | QFile::ExeUser |
                                 QFile::ReadOwner| QFile::ReadUser | QFile::ReadOther | QFile::ReadGroup |
                                 QFile::WriteGroup | QFile::WriteOwner | QFile::WriteOther | QFile::WriteUser);
                f.close();
            }
        }
    }

    QProcess startProc;
    startProc.start("bash updater.sh srv");
    startProc.waitForFinished();
    return 0;
}


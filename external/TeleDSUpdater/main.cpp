#include <QCoreApplication>
#include <QString>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include "notherfilesystem.h"

int main(int argc, char *argv[])
{
    qDebug() << "Updater Started!!!" << QDateTime::currentDateTimeUtc();
    QDir::setCurrent ( "home/pi/teleds" );
    qDebug() << QDir::currentPath();
    //IN: tdsu file
    if (argc < 2)
    {
        qDebug() << "wrong arguments!" << argc;
        return 0;
    }
    qDebug() << "arg is " << argv[1];
    QString updateBatchFile = QString(argv[1]);
    if (QString(argv[1]) == "-service")
    {
        qDebug() << "running from service!";
        QFile f("update_info");
        qDebug() << "trying to open file update_info";
        if (f.open(QFile::ReadOnly))
        {
            QString data = f.readAll();
            data = data.replace("\r","").replace("\n","");
            qDebug() << "given data is: " << data;
            if (data.length() > 5)
                updateBatchFile = data;
            f.close();
            QFile::remove("update_info");
        }
        else
        {
            qDebug() << "ERROR! Cant open???" << f.errorString();
        }
    }

    qDebug() << "working with " << updateBatchFile;

    NotherFileSystem nfs;
    nfs.load(QString(updateBatchFile));
    foreach (const QString &d, nfs.dirs())
        QDir().mkpath(d);
    if (nfs.fileExists("system::player"))
    {
        QByteArray data = nfs.getFile("system::player");
        if (data.length() > 1000000)
        {
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
    }
    else
    {
        qDebug() << "WARNING: no player update in batch!";
    }

    foreach (const QString &filename, nfs.files())
    {
        if (filename != "system::player" &&
            filename != "system::updater")
        {
            qDebug () << "unpacking " << filename;
            QByteArray data = nfs.getFile(filename);
            if (data.length() > 0)
            {
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
                else
                {
                    qDebug() << "file " << filename << "cannot be opened";
                }
            }
        }
    }

    QFile::remove(updateBatchFile);

  //  QProcess startProc;
  //  startProc.start("bash updater.sh srv");
  //  startProc.waitForFinished();
    return 0;
}


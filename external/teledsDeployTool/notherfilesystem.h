#ifndef NOTHERFILESYSTEM_H
#define NOTHERFILESYSTEM_H

#include <QStringList>
#include <QList>
#include <QString>
#include <QDataStream>

#define NFS_MAGIC 0x54445355

struct FileDescriptor
{
    QString name;
    int stored_size;
    int pointer;

    static FileDescriptor load(QDataStream &s);
    void save(QDataStream &s) const;
};

struct NFSHeader
{
    int magic;
    QStringList dirs;
    QList<FileDescriptor> files;

    static NFSHeader load(QDataStream &s);
    void save(QDataStream &s);
    void buildDirsFromFiles();
};

class NotherFileSystem
{
public:
    NotherFileSystem();
    ~NotherFileSystem(){;}
    void load(QString file);
    void save(QString file);
    QByteArray build();
    void addFile(QString fileName, QString name);
    QByteArray getFile(QString name);
    bool fileExists(QString name);
    QStringList files();
private:
    QByteArray data;
    NFSHeader header;
};

#endif // NOTHERFILESYSTEM_H

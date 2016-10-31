#ifndef NOTHERFILESYSTEM_H
#define NOTHERFILESYSTEM_H

#include <QByteArray>
#include <QStringList>
#include <QList>
#include <QString>
#include <QDataStream>
#define NFS_MAGIC 0x54445355

namespace LFSR{

class Encoder{
public:
    static void encode(QByteArray &data, QString pass);
};

}

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
    QStringList dirs(){return header.dirs;}
private:
    QByteArray data;
    NFSHeader header;
};

#endif // NOTHERFILESYSTEM_H

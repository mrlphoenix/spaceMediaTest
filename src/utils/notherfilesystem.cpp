#include "notherfilesystem.h"


#include <QCryptographicHash>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>

void LFSR::Encoder::encode(QByteArray &data, QString pass)
{
    QByteArray hashArray = QCryptographicHash::hash(pass.toLocal8Bit(),QCryptographicHash::Md5);
    hashArray = hashArray.mid(0,4);
    QDataStream ds(hashArray);

    int seed;
    ds >> seed;

    for (int i = 0; i<data.count(); ++i)
    {
        char currentByte = 0;
        for (int bitIndex = 0; bitIndex < 8; ++bitIndex)
        {
            if (seed & 0x00000001)
            {
                seed = (seed ^ 0x80000057 >> 1) | 0x80000000;
                currentByte |= (1 << bitIndex);
            }
            else
                seed >>= 1;
        }
        data[i] = data[i] ^ currentByte;
    }
}

NotherFileSystem::NotherFileSystem()
{
    header.magic = NFS_MAGIC;
}

void NotherFileSystem::load(QString file)
{
    QFile f(file);
    if (f.open(QFile::ReadOnly))
    {
        QDataStream s(&f);
        NFSHeader h = NFSHeader::load(s);
        if (h.magic == NFS_MAGIC)
        {
            QByteArray newData;
            s >> newData;
            data = newData;
            header = h;
        }
        f.close();
    }
}

void NotherFileSystem::save(QString file)
{
    QFile f(file);
    if (f.open(QFile::WriteOnly))
    {
        f.write(build());
        f.flush();
        f.close();
    }
}

QByteArray NotherFileSystem::build()
{
    QByteArray out;
    QDataStream s(&out, QIODevice::WriteOnly);
    header.buildDirsFromFiles();
    header.save(s);
    s << data;
    return out;
}

void NotherFileSystem::addFile(QString fileName, QString name)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly))
    {
        QByteArray fileData = f.readAll();
        fileData = qCompress(fileData,9);
        LFSR::Encoder::encode(fileData,name);

        int storedSize = fileData.count();
        int filePointer = data.size();
        data.append(fileData);

        FileDescriptor desc;
        desc.name = name;
        desc.pointer = filePointer;
        desc.stored_size = storedSize;
        header.files.append(desc);
        f.close();
    }
}

QByteArray NotherFileSystem::getFile(QString name)
{
    foreach (const FileDescriptor &f, header.files)
        if (f.name == name)
        {
            QByteArray fileData = data.mid(f.pointer, f.stored_size);
            LFSR::Encoder::encode(fileData, name);
            fileData = qUncompress(fileData);
            return fileData;
        }
    return QByteArray();
}

bool NotherFileSystem::fileExists(QString name)
{
    return files().contains(name);
}

QStringList NotherFileSystem::files()
{
    QStringList result;
    foreach (const FileDescriptor &fd, header.files)
        result.append(fd.name);
    return result;
}


FileDescriptor FileDescriptor::load(QDataStream &s)
{
    FileDescriptor result;
    s >> result.name;
    s >> result.stored_size;
    s >> result.pointer;
    return result;
}

void FileDescriptor::save(QDataStream &s) const
{
    s << name;
    s << stored_size;
    s << pointer;
}

NFSHeader NFSHeader::load(QDataStream &s)
{
    NFSHeader result;
    int dirCount, fileCount;
    s >> result.magic;
    if (result.magic == NFS_MAGIC)
    {
        s >> dirCount;
        s >> fileCount;
        for (int i = 0; i < dirCount; i++)
        {
            QString currentDir;
            s >> currentDir;
            result.dirs.append(currentDir);
        }
        for (int i = 0; i < fileCount; i++)
        {
            FileDescriptor currentFile = FileDescriptor::load(s);
            result.files.append(currentFile);
        }
    }
    return result;
}

void NFSHeader::save(QDataStream &s)
{
    s << magic;
    s << dirs.count();
    s << files.count();
    foreach (const QString &dir, dirs)
        s << dir;
    foreach (const FileDescriptor &fd, files)
        fd.save(s);
}

void NFSHeader::buildDirsFromFiles()
{
    foreach (const FileDescriptor &f, files)
    {
        QFileInfo fInfo(f.name);
        QString path = fInfo.path();
        if (!dirs.contains(path) && path != "" && path != "/")
            dirs.append(path);
    }
}


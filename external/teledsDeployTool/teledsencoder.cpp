#include <QFile>
#include <QByteArray>
#include <QFileDialog>
#include <QDateTime>
#include <QVector>
#include <QCryptographicHash>
#include <QHash>

#include "teledsencoder.h"


QByteArray TeleDSPatcher::readFile(QString filename)
{
    QByteArray result;
    QFile f(filename);
    if (f.open(QFile::ReadOnly))
    {
        result = f.readAll();
        f.close();
    }
    else
        qDebug() << "Cant Open File " << filename;
    return result;
}

QVector<DeltaPart> TeleDSPatcher::createFilePatch(QByteArray srcData, QByteArray destData, int window, int minPartSize)
{
    qDebug() << QDateTime::currentDateTimeUtc();
    QVector<DeltaPart> patch;

    DeltaPart dataPart;
    dataPart.type = DeltaPart::DELTA_PART_DATA;
    dataPart.offset = 0;
    dataPart.size = 0;

    int destDataCount = destData.count();
    int srcDataCount = srcData.count();

    for (int destIndex = 0; destIndex < destDataCount; destIndex++)
    {
        if (destIndex %100 == 0)
            qDebug() << destIndex << "/" << destDataCount;
        //
        int bestMatchIndex = -1, bestMatchSize = -1;
        for (int srcIndex = std::max(0, destIndex - window); srcIndex < destDataCount && srcIndex < srcDataCount && srcIndex < destIndex + window; srcIndex++)
        {
            int currentMatchIndex = -1, currentMatchSize = -1;
            for (int i = 0; i < destDataCount - destIndex && i < srcDataCount - srcIndex; i++)
            {
                if (destData[destIndex + i] == srcData[srcIndex + i])
                {
                    currentMatchIndex = srcIndex;
                    currentMatchSize = i + 1;
                }
                else
                    break;
            }
            if (currentMatchSize > minPartSize && currentMatchSize > bestMatchSize)
            {
                bestMatchIndex = currentMatchIndex;
                bestMatchSize = currentMatchSize;
            }
        }
        //Adding deploy data

        if (bestMatchSize > minPartSize)
        {
            if (dataPart.data.count())
            {
                patch.append(dataPart);
                dataPart.data.clear();
            }
            DeltaPart matchPart;
            matchPart.type = DeltaPart::DELTA_PART_SOURCE;
            matchPart.offset = bestMatchIndex;
            matchPart.size = bestMatchSize;
            patch.append(matchPart);
            destIndex += bestMatchSize - 1;
        }
        else
            dataPart.data.append(destData[destIndex]);
    }

    if (dataPart.data.count())
        patch.append(dataPart);

    return patch;
}

QByteArray TeleDSPatcher::serializePatch(QVector<DeltaPart> &patch)
{
    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);
    ds << patch.count();
    foreach (const DeltaPart &p, patch)
        ds << p.type << p.offset << p.size << p.data;
    out = qCompress(out, 9);
    return out;
}

QByteArray TeleDSPatcher::applyPatch(QByteArray srcData, QByteArray patchData)
{
    qDebug() << QDateTime::currentDateTimeUtc();
    QByteArray result;
    QByteArray unpackedPatchData = qUncompress(patchData);
    QDataStream ds(&unpackedPatchData, QIODevice::ReadOnly);
    int deltaCount;
    ds >> deltaCount;

    for (int i = 0; i < deltaCount; i++)
    {
        DeltaPart p;
        ds >> p.type >> p.offset >> p.size >> p.data;
        if (p.type == DeltaPart::DELTA_PART_SOURCE)
        {
            for (int j = p.offset; j < p.offset + p.size; j++)
            {
                result.append(srcData[j]);
            }
        }
        else if (p.type == DeltaPart::DELTA_PART_DATA)
        {
            for (int i = 0; i < p.data.count(); i++)
                result.append(p.data[i]);
        }
    }
    qDebug() << QDateTime::currentDateTimeUtc();

    return result;
}

TeleDSPatch TeleDSPatcher::createPatch(QStringList allFileList, QStringList updateFileList, QStringList prevFileList,
                                       QString prevRootDir, QString nextRootDir, int productVersion, int updateVersion)
{
    QHash<QString, QString> sourceFileHashes, updateFileHashes;
    foreach (const QString &fileName, allFileList)
    {
        QFile f(fileName);
        if (f.open(QFile::ReadOnly))
        {
            QByteArray data = f.readAll();
            sourceFileHashes[fileName] = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
            f.close();
        }
    }

    foreach (const QString &fileName, updateFileList)
    {
        QFile f(fileName);
        if (f.open(QFile::ReadOnly))
        {
            QByteArray data = f.readAll();
            updateFileHashes[fileName] = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
            f.close();
        }
    }
    TeleDSPatch result;
    result.magic = TeleDSPatch::TELEDS_PATCH_MAGIC;
    result.playerVersion = productVersion;
    result.patchVersion = updateVersion;
    result.totalFileCount = allFileList.count();
    result.updateFileCount = updateFileList.count();

    for (int i = 0; i < updateFileList.count(); i++)
    {
        QString currentFileName = updateFileList[i];
        QString deviceFileName = updateFileList[i].replace(nextRootDir, "");
        QString prevFileName;
        for (int j = 0; j < prevFileList.count(); j++)
        {
            QString prevFile = prevFileList[j];
            prevFile = prevFile.replace(prevRootDir, "");
            if (deviceFileName == prevFile)
            {
                prevFileName = prevFileList[j];
                break;
            }
        }

        FilePatchInfo info;
        info.magic = FilePatchInfo::TELEDS_FILE_PATCH_MAGIC;
        info.filePath = deviceFileName;

        QString hash;
        QFile f(currentFileName);
        if (f.open(QFile::ReadOnly))
        {
            QByteArray data = f.readAll();
            hash = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
            f.close();
        }
        info.originalFileHash = hash;
        QVector<DeltaPart> patch = createFilePatch(prevFileName, currentFileName);
        info.patch = patch;
        QString patchHash = QString(QCryptographicHash::hash(serializePatch(patch), QCryptographicHash::Md5).toHex());
        info.patchHash = patchHash;
        result.updateFiles.append(info);
    }
    foreach (const QString &s, sourceFileHashes.keys())
    {
        QString deviceFileName = s;
        deviceFileName = deviceFileName.replace(prevRootDir, "");
        result.fileNames.append(deviceFileName);
        result.fileHashes.append(sourceFileHashes[s]);
    }
    return result;
}

QVector<DeltaPart> TeleDSPatcher::createFilePatch(QString srcFileName, QString destFileName, int window, int minPartSize)
{
    qDebug() << "Creating patch between " << srcFileName << " and " << destFileName;
    return createFilePatch(readFile(srcFileName), readFile(destFileName), window, minPartSize);
}

TeleDSImage TeleDSImage::deserialize(QByteArray &data)
{
    QDataStream ds(&data, QIODevice::ReadOnly);
    TeleDSImage result;
    ds >> result.magic >> result.playerVersion >> result.patchVersion >> result.fileCount;
    if (result.magic == TeleDSImage::TELEDS_IMAGE_MAGIC)
    {
        for (int i = 0; i < result.fileCount; i++)
        {
            FileDescriptor fd;
            ds >> fd.name >> fd.hash;
            QByteArray compressedData;
            ds >> compressedData;
            fd.data = qUncompress(compressedData);
            result.files[fd.name] = fd;
        }
    }
    return result;
}

TeleDSImage TeleDSImage::createImage(int playerVersion, int patchVersion,QStringList fileList)
{
    TeleDSImage result;
    result.magic = TeleDSImage::TELEDS_IMAGE_MAGIC;
    result.playerVersion = playerVersion;
    result.patchVersion = patchVersion;
    foreach (const QString &filename, fileList)
    {
        qDebug() << "CurrentFile " << filename;
        if (QFile::exists(filename))
        {
            FileDescriptor fd;
            QFile f(filename);
            if (f.open(QFile::ReadOnly))
            {
                QByteArray data = f.readAll();
                fd.hash = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
                fd.name = filename;
                fd.data = qCompress(data, 9);
                qDebug() << "Compressed Size = " << fd.data.count() << "bytes";
                f.close();
                result.files[fd.name] = fd;
            }
        }
    }
    result.fileCount = result.files.count();
    return result;
}

QByteArray TeleDSImage::serialize()
{
    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);
    ds << TELEDS_IMAGE_MAGIC << playerVersion << patchVersion << files.count();
    foreach (const QString &name, files.keys())
        ds << files[name].name << files[name].hash << qCompress(files[name].data, 9);
    return out;
}

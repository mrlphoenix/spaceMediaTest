#ifndef TELEDSENCODER_H
#define TELEDSENCODER_H


#include <QObject>
#include <QDebug>
#include <QVector>
#include <QStringList>
#include <QByteArray>

struct DeltaPart
{
    static const int DELTA_PART_SOURCE = 0x0;
    static const int DELTA_PART_DATA = 0x1;
    int type;
    int size, offset;
    QByteArray data;
};

struct FilePatchInfo
{
    static const int TELEDS_FILE_PATCH_MAGIC = 0x0202FFDD;
    int magic;
    QString patchHash, originalFileHash, filePath;
    QVector<DeltaPart> patch;
};

struct TeleDSPatch
{
    static const int TELEDS_PATCH_MAGIC = 0x0303CCCC;
    int magic;
    int playerVersion, patchVersion;
    int updateFileCount, totalFileCount;
    QVector<QString> fileNames;
    QVector<QString> fileHashes;
    QVector<FilePatchInfo> updateFiles;
};

struct TeleDSImage
{
    static const int TELEDS_IMAGE_MAGIC = 0x0404EEEE;
    int magic;
    int playerVersion, patchVersion;
    int fileCount;
    struct FileDescriptor
    {
        QString name, hash;
        QByteArray data;
    };
    QHash <QString, FileDescriptor> files;
    static TeleDSImage deserialize(QByteArray &data);
    static TeleDSImage createImage(int playerVersion, int patchVersion, QStringList fileList);
    QByteArray serialize();
};


struct TeleDSPatchFileHeader
{
    static const int TELEDS_PATCH_MAGIC = 0x0101FEFE;
    int magic, playerVersion, patchVersion;
    QString patchHash, originalFileHash;
    QString fileName;
    //
};


class TeleDSPatcher
{
public:
    static QByteArray readFile(QString filename);
    static QVector<DeltaPart> createFilePatch(QByteArray srcData, QByteArray destData, int window = 16000, int minPartSize = 8);
    static QVector<DeltaPart> createFilePatch(QString srcFileName, QString destFileName, int window = 16000, int minPartSize = 8);
    static QByteArray serializePatch(QVector<DeltaPart> &patch);
    static QByteArray applyPatch(QByteArray srcData, QByteArray patchData);
    static TeleDSPatch createPatch(QStringList allFileList, QStringList updateFileList, QStringList prevFileList, QString prevRootDir, QString nextRootDir, int productVersion, int updateVersion);
};


#endif // TELEDSENCODER_H

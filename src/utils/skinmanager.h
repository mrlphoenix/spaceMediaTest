#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
class QNetworkReply;
struct ThemeDesc
{
    ThemeDesc(){isDefault = true;}
    ThemeDesc(QString backgroundURL, QString logoURL, QString backgroundColor, QString foregroundColor);
    ~ThemeDesc(){;}

    bool isDefault;

    QString backgroundURL, logoURL, color1, color2;

    QUrl relocatedBackgroundURL, relocatedLogoURL;

    QString backImageHash, logoHash;
};
class SkinManager : public QObject
{
    Q_OBJECT
public:
    SkinManager(QObject * parent);
    ~SkinManager(){;}
    enum DownloadState {IDLE, BACKGROUND_STATE, LOGO_STATE};
signals:
    void skinReady(ThemeDesc desc);
public slots:

    void updateSkin(QString backgroundURL, QString logoURL, QString bgHash, QString logoHash, QString color1, QString color2);
    bool isSkinReady(QString backgroundURL, QString logoURL, QString bgHash, QString logoHash);
private slots:
    void replyFinished(QNetworkReply * reply);
private:
    void downloadFile(QString url, QString dest);
    QString getFileExt(QString url);
    QString getBgHash(QString ext);
    QString getLogoHash(QString ext);
    void writeToFileSync(QByteArray data, QString filename);


    ThemeDesc currentSkin;
    QNetworkAccessManager manager;
    DownloadState state;
    QString bgExt, logoExt;
    QString cachedBgHash, cachedLogoHash;
    bool needToDownloadLogo;
};

#endif // SKINMANAGER_H

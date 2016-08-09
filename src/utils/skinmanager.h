#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
class QNetworkReply;
struct ThemeDesc
{
    ThemeDesc(){isDefault = true;}
    ThemeDesc(QString backgroundURL, QString logoURL, QString backgroundColor, QString foregroundColor,
              QString menuBackgroundURL, QString menuLogoURL, QString menuColor1, QString menuColor2,
              bool tileMode, bool showTeleDSPlayer);
    ~ThemeDesc(){;}

    bool isDefault;

    QString backgroundURL, logoURL, color1, color2;
    QString menuBackgroundURL, menuLogoURL, menuColor1, menuColor2;

    QUrl relocatedBackgroundURL, relocatedLogoURL;
    QUrl relocatedMenuBackgroundURL, relocatedMenuLogoURL;

    bool tileMode, showTeleDSPlayer;
    bool menuTileMode, menuShowTeleDSPlayer;
};
class SkinManager : public QObject
{
    Q_OBJECT
public:
    SkinManager(QObject * parent);
    ~SkinManager(){;}
signals:
    void skinReady(ThemeDesc desc);
public slots:

    void updateSkin(QString backgroundURL, QString logoURL,
                    QString menuBackgroundURL, QString menuLogoURL,
                    QString bgHash, QString logoHash,
                    QString menuBgHash, QString menuLogoHash,
                    QString color1, QString color2,
                    QString menuColor1, QString menuColor2,
                    bool tileMode, bool showTeleDSPlayer, bool menuTileMode, bool menuShowTeleDSPlayer);
    bool isSkinReady(QString backgroundURL, QString logoURL,
                     QString menuBackgroundURL, QString menuLogoURL,
                     QString bgHash, QString logoHash,
                     QString menuBgHash, QString menuLogoHash);
private slots:
    void replyFinished(QNetworkReply * reply);
private:
    void downloadFile(QString url, QString dest);
    QString getFileExt(QString url);
    QString getFileHash(QString name, QString ext);
    void writeToFileSync(QByteArray data, QString filename);


    ThemeDesc currentSkin;
    QNetworkAccessManager manager;
    QHash<QString, QString> cachedHashes;

    struct SkinComponent
    {
        enum ComponentType{BACKGROUND, LOGO, MENU_BACKGROUND, MENU_LOGO};
        QString source, dest, ext;
        ComponentType type;
    };

    QList<SkinComponent> componentsToDownload;
};

#endif // SKINMANAGER_H

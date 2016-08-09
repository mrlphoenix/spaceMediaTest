#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include "platformspecific.h"
#include "skinmanager.h"


ThemeDesc::ThemeDesc(QString backgroundURL, QString logoURL, QString backgroundColor, QString foregroundColor,
                     QString menuBackgroundURL, QString menuLogoURL, QString menuColor1, QString menuColor2,
                     bool tileMode, bool showTeleDSPlayer)
{
    this->backgroundURL = backgroundURL;
    this->logoURL = logoURL;
    this->color1 = backgroundColor;
    this->color2 = foregroundColor;
    this->menuBackgroundURL = menuBackgroundURL;
    this->menuLogoURL = menuLogoURL;
    this->menuColor1 = menuColor1;
    this->menuColor2 = menuColor2;
    this->isDefault = true;
    this->tileMode = tileMode;
    this->showTeleDSPlayer = showTeleDSPlayer;
}

SkinManager::SkinManager(QObject *parent) : QObject(parent)
{
    connect(&manager,SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void SkinManager::updateSkin(QString backgroundURL, QString logoURL, QString menuBackgroundURL, QString menuLogoURL, QString bgHash, QString logoHash,
                             QString menuBgHash, QString menuLogoHash, QString color1, QString color2, QString menuColor1, QString menuColor2,
                             bool tileMode, bool showTeleDSPlayer, bool menuTileMode, bool menuShowTeleDSPlayer)
{
    currentSkin.color1 = color1;
    currentSkin.color2 = color2;
    currentSkin.backgroundURL = backgroundURL;
    currentSkin.logoURL = logoURL;
    currentSkin.menuBackgroundURL = menuBackgroundURL;
    currentSkin.menuLogoURL = menuLogoURL;
    currentSkin.menuColor1 = menuColor1;
    currentSkin.menuColor2 = menuColor2;
    currentSkin.tileMode = tileMode;
    currentSkin.showTeleDSPlayer = showTeleDSPlayer;
    currentSkin.menuTileMode = menuTileMode;
    currentSkin.menuShowTeleDSPlayer = menuShowTeleDSPlayer;

    currentSkin.isDefault = (
                                backgroundURL.isEmpty() && logoURL.isEmpty() && menuBackgroundURL.isEmpty() && menuLogoURL.isEmpty() &&
                                color1.isEmpty() && color2.isEmpty() && menuColor1.isEmpty() && menuColor2.isEmpty()
                            );

    componentsToDownload.clear();

    QString bgExt = getFileExt(backgroundURL),
            logoExt = getFileExt(logoURL),
            menuBgExt = getFileExt(menuBackgroundURL),
            menuLogoExt = getFileExt(menuLogoURL);

    if (getFileHash("bg", bgExt) != bgHash)
    {
        SkinComponent bgComponent;
        bgComponent.type = SkinComponent::BACKGROUND;
        bgComponent.source = backgroundURL;
        bgComponent.dest = CONFIG_FOLDER + "bg" + bgExt;
        componentsToDownload.append(bgComponent);
    }
    if (getFileHash("logo", logoExt) != logoHash)
    {
        SkinComponent logoComponent;
        logoComponent.type = SkinComponent::LOGO;
        logoComponent.source = logoURL;
        logoComponent.dest = CONFIG_FOLDER + "logo" + logoExt;
        componentsToDownload.append(logoComponent);
    }
    if (getFileHash("menu_bg", menuBgExt) != menuBgHash)
    {
        SkinComponent menuBgComponent;
        menuBgComponent.type = SkinComponent::MENU_BACKGROUND;
        menuBgComponent.source = menuBackgroundURL;
        menuBgComponent.dest = CONFIG_FOLDER + "menu_bg" + menuBgExt;
        componentsToDownload.append(menuBgComponent);
    }
    if (getFileHash("menu_logo", menuLogoHash) != menuLogoHash)
    {
        SkinComponent menuLogoComponent;
        menuLogoComponent.type = SkinComponent::MENU_LOGO;
        menuLogoComponent.source = menuLogoURL;
        menuLogoComponent.dest = CONFIG_FOLDER + "menu_logo" + menuBgExt;
        componentsToDownload.append(menuLogoComponent);
    }


    qDebug() << "TeleDSCore::settings|need to download " << componentsToDownload.count() << " items";

    if (componentsToDownload.isEmpty())
    {
        currentSkin.relocatedBackgroundURL = QUrl::fromLocalFile(CONFIG_FOLDER + "bg" + bgExt);
        currentSkin.relocatedLogoURL = QUrl::fromLocalFile(CONFIG_FOLDER + "logo" + logoExt);
        currentSkin.relocatedMenuBackgroundURL = QUrl::fromLocalFile(CONFIG_FOLDER + "menu_bg" + menuBgExt);
        currentSkin.relocatedMenuLogoURL = QUrl::fromLocalFile(CONFIG_FOLDER + "menu_logo" + menuLogoExt);
        emit skinReady(currentSkin);
    }
    else
    {
        manager.get(QNetworkRequest(QUrl(componentsToDownload.first().source)));
    }
}

bool SkinManager::isSkinReady(QString backgroundURL, QString logoURL,
                              QString menuBackgroundURL, QString menuLogoURL,
                              QString bgHash, QString logoHash,
                              QString menuBgHash, QString menuLogoHash)
{
    QString bgExt = getFileExt(backgroundURL),
            logoExt = getFileExt(logoURL),
            menuBgExt = getFileExt(menuBackgroundURL),
            menuLogoExt = getFileExt(menuLogoURL);
    return  (getFileHash("bg", bgExt) == bgHash) && (getFileHash("logo", logoExt) == logoHash) &&
            (getFileHash("menu_bg", menuBgExt) == menuBgHash) && (getFileHash("menu_logo",menuLogoExt) == menuLogoHash);
}

void SkinManager::replyFinished(QNetworkReply *reply)
{
    qDebug() << "TeleDSCore::settingsSkinManager::replyFinished";
    if (reply->error())
    {
        qDebug() << "TeleDSCore::settingsSkinManager::replyFinished -> network error: " + reply->errorString();
        componentsToDownload.clear();
        emit skinReady(currentSkin);
    }
    else
    {
        SkinComponent currentComponent = componentsToDownload.first();
        writeToFileSync(reply->readAll(),currentComponent.dest);
        switch (currentComponent.type)
        {
        case SkinComponent::BACKGROUND:
            currentSkin.relocatedBackgroundURL = currentComponent.dest;
            cachedHashes["bg"] = "";
            break;
        case SkinComponent::LOGO:
            currentSkin.relocatedLogoURL = currentComponent.dest;
            cachedHashes["logo"] = "";
            break;
        case SkinComponent::MENU_BACKGROUND:
            currentSkin.relocatedMenuBackgroundURL = currentComponent.dest;
            cachedHashes["menu_bg"] = "";
            break;
        case SkinComponent::MENU_LOGO:
            currentSkin.relocatedMenuLogoURL = currentComponent.dest;
            cachedHashes["menu_logo"] = "";
            break;
        default:
            break;
        }
        componentsToDownload.removeFirst();
        reply->deleteLater();
        if (!componentsToDownload.isEmpty())
            manager.get(QNetworkRequest(QUrl(componentsToDownload.first().source)));
        else
            emit skinReady(currentSkin);
    }
}

QString SkinManager::getFileExt(QString url)
{
    QStringList urlTokens = url.split(".");
    QString extension;
    if (urlTokens.count() < 2)
        extension = "";
    else
        extension = "." + urlTokens.last();
    return extension;
}

QString SkinManager::getFileHash(QString name, QString ext)
{
    if (!cachedHashes.contains(name) || cachedHashes[name].isEmpty())
        cachedHashes[name] = PlatformSpecificService.getFileHash(CONFIG_FOLDER + name + ext);
    return cachedHashes[name];
}


void SkinManager::writeToFileSync(QByteArray data, QString filename)
{
    QFile f(filename);
    f.open(QFile::WriteOnly);
    f.write(data);
    f.flush();
    f.close();
}

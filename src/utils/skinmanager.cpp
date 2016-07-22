#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include "platformspecific.h"
#include "skinmanager.h"


ThemeDesc::ThemeDesc(QString backgroundURL, QString logoURL, QString backgroundColor, QString foregroundColor)
{
    this->backgroundURL = backgroundURL;
    this->logoURL = logoURL;
    this->color1 = backgroundColor;
    this->color2 = foregroundColor;
    this->isDefault = true;
}

SkinManager::SkinManager(QObject *parent) : QObject(parent)
{
    connect(&manager,SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    state = IDLE;
}

void SkinManager::updateSkin(QString backgroundURL, QString logoURL, QString bgHash, QString logoHash, QString color1, QString color2)
{
    currentSkin.color1 = color1;
    currentSkin.color2 = color2;
    currentSkin.backgroundURL = backgroundURL;
    currentSkin.logoURL = logoURL;

    currentSkin.isDefault = (backgroundURL == "" && logoURL == "" && color1 == "" && color2 == "");

    bgExt = getFileExt(backgroundURL);
    logoExt = getFileExt(logoURL);

    needToDownloadLogo = (getLogoHash(logoExt) != logoHash);
    bool needToDownloadBg = getBgHash(bgExt) != bgHash;

    qDebug() << "TeleDSCore::settings " << needToDownloadBg << " " << needToDownloadLogo;

    if (needToDownloadBg)
    {
        qDebug() << "TeleDSCore::settings URL: " << backgroundURL;
        manager.get(QNetworkRequest(QUrl(backgroundURL)));
        state = BACKGROUND_STATE;
    }
    else
    {
        QString filepath = CONFIG_FOLDER + "bg" + bgExt;
        currentSkin.relocatedBackgroundURL = QUrl::fromLocalFile(filepath);
    }
    if (!needToDownloadLogo)
    {
        QString filepath = CONFIG_FOLDER + "logo" + logoExt;
        currentSkin.relocatedLogoURL = QUrl::fromLocalFile(filepath);
    }
    if (!needToDownloadBg &&  needToDownloadLogo)
    {
        qDebug() << "TeleDSCore::settings URL: " << logoURL;
        manager.get(QNetworkRequest(QUrl(logoURL)));
        state = LOGO_STATE;
    }
    if (!needToDownloadBg && !needToDownloadLogo)
    {
        emit skinReady(currentSkin);
    }

}

bool SkinManager::isSkinReady(QString backgroundURL, QString logoURL, QString bgHash, QString logoHash)
{
    QString bgExtension, logoExtension;
    bgExtension = getFileExt(backgroundURL);
    logoExtension = getFileExt(logoURL);
    bool shouldDownloadLogo = (getLogoHash(logoExtension) != logoHash), shouldDownloadBg = (getBgHash(bgExtension) != bgHash);
    return !shouldDownloadLogo && !shouldDownloadBg;
}

void SkinManager::replyFinished(QNetworkReply *reply)
{
    qDebug() << "TeleDSCore::settingsSkinManager::replyfinished";
    if (reply->error())
    {
        qDebug() << "TeleDSCore::settingsSkinManager::replyFinished -> network error: " + reply->errorString();
    }
    else
    {
        if (state == BACKGROUND_STATE)
        {
            qDebug() << "TeleDSCore::settings SkinManager::BGState -> " << CONFIG_FOLDER + "bg" + bgExt;
            QString filepath = CONFIG_FOLDER + "bg" + bgExt;
            writeToFileSync(reply->readAll(),filepath);
            cachedBgHash = "";
            reply->deleteLater();
            currentSkin.relocatedBackgroundURL = QUrl::fromLocalFile(filepath);
            if (needToDownloadLogo)
            {
                manager.get(QNetworkRequest(QUrl(currentSkin.logoURL)));
                state = LOGO_STATE;
            }
            else
            {
                currentSkin.relocatedLogoURL = QUrl::fromLocalFile(CONFIG_FOLDER + "logo" + logoExt);
                state = IDLE;
                emit skinReady(currentSkin);
            }
        }
        else if (state == LOGO_STATE)
        {
            qDebug() << "TeleDSCore::settings SkinManager::LogoState -> " << CONFIG_FOLDER + "logo" + logoExt;
            QString filepath = CONFIG_FOLDER + "logo" + logoExt;
            writeToFileSync(reply->readAll(),filepath);
            cachedLogoHash = "";
            reply->deleteLater();
            currentSkin.relocatedLogoURL = QUrl::fromLocalFile(filepath);
            state = IDLE;
            emit skinReady(currentSkin);
        }
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

QString SkinManager::getBgHash(QString ext)
{
    if (cachedBgHash.isEmpty())
        cachedBgHash = PlatformSpecificService.getFileHash(CONFIG_FOLDER + "bg" + ext);
    return cachedBgHash;
}

QString SkinManager::getLogoHash(QString ext)
{
    if (cachedLogoHash.isEmpty())
        cachedLogoHash = PlatformSpecificService.getFileHash(CONFIG_FOLDER + "logo" + ext);
    return cachedLogoHash;
}

void SkinManager::writeToFileSync(QByteArray data, QString filename)
{
    QFile f(filename);
    f.open(QFile::WriteOnly);
    f.write(data);
    f.flush();
    f.close();
}

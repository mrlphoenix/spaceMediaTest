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

    bgExt = getFileExt(backgroundURL);
    logoExt = getFileExt(logoURL);

    if (getBgHash(bgExt) != bgHash)
    {
        manager.get(QNetworkRequest(QUrl(backgroundURL)));
        state = BACKGROUND_STATE;
        needToDownloadLogo = (getLogoHash(logoExt) != logoHash);
    }
    else if (getLogoHash(logoExt) != logoHash)
    {
        manager.get(QNetworkRequest(QUrl(logoURL)));
        state = LOGO_STATE;
    }
    else
        state = IDLE;
}

void SkinManager::replyFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "SkinManager::replyFinished -> network error: " + reply->errorString();
    }
    else
    {
        if (state == BACKGROUND_STATE)
        {
            QString filepath = CONFIG_FOLDER + "bg" + bgExt;
            writeToFileSync(reply->readAll(),filepath);
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
            QString filepath = CONFIG_FOLDER + "logo" + logoExt;
            writeToFileSync(reply->readAll(),filepath);
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

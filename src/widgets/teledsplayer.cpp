#include <QFileInfo>
#include <QProcess>
#include "teledsplayer.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "sunposition.h"
#include "version.h"
#include "statictext.h"

#ifdef PLATFORM_DEFINE_RPI
#include "linux/input.h"
#endif

TeleDSPlayer::TeleDSPlayer(QObject *parent) : QObject(parent)
{
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);

    delay = 0000;
    status.isPlaying = false;
    status.item = "";
    isActive = true;

    view.setSource(QUrl(QStringLiteral("qrc:/main_player.qml")));
    viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QTimer::singleShot(500,this,SLOT(bindObjects()));
    QTimer::singleShot(500,this,SLOT(invokeVersionText()));
    QTimer::singleShot(500,this,SLOT(invokeSetLicenseData()));

    QTimer::singleShot(1000, [this](){
        qDebug() << "SpaceMediaMenu Initialization";
        connect(&menu, SIGNAL(displayRotationChanged(int,QString)),this,SLOT(invokeMenuDisplayRotationChanged(int,QString)));
        connect(&menu, SIGNAL(displayRotationSelected()), this, SLOT(invokeMenuDisplayRotationSelected()));
        connect(&menu, SIGNAL(hdmiBoostChanged(int)), this, SLOT(invokeMenuHDMIBoostChanged(int)));
        connect(&menu, SIGNAL(hdmiBoostSelected()), this, SLOT(invokeMenuHDMIBoostSelected()));
        connect(&menu, SIGNAL(hdmiDriveChanged(int,QString)), this, SLOT(invokeMenuHDMIDriveChanged(int,QString)));
        connect(&menu, SIGNAL(hdmiDriveSelected()), this, SLOT(invokeMenuHDMIDriveSelected()));
        connect(&menu, SIGNAL(hdmiGroupChanged(int,QString)), this, SLOT(invokeMenuHDMIGroupChanged(int,QString)));
        connect(&menu, SIGNAL(hdmiGroupSelected()), this, SLOT(invokeMenuHDMIGroupSelected()));
        connect(&menu, SIGNAL(hdmiModeChanged(int,QString)), this, SLOT(invokeMenuHDMIModeChanged(int,QString)));
        connect(&menu, SIGNAL(hdmiModeSelected()), this, SLOT(invokeMenuHDMIModeSelected()));
        connect(&menu, SIGNAL(wifiNetworkSelected()), this, SLOT(invokeMenuWifiNetworkSelected()));
        connect(&menu, SIGNAL(wifiNetworkChanged(QString)), this, SLOT(invokeMenuWifiNetworkChanged(QString)));
        connect(&menu, SIGNAL(wifiNameSelected()), this, SLOT(invokeMenuWifiNameSelected()));
        connect(&menu, SIGNAL(wifiNameChanged(QString)), this, SLOT(invokeMenuWifiNameChanged(QString)));
        connect(&menu, SIGNAL(wifiPassSelected()), this, SLOT(invokeMenuWifiPassSelected()));
        connect(&menu, SIGNAL(wifiPassChanged(QString)), this, SLOT(invokeMenuWifiPassChanged(QString)));
        connect(&menu, SIGNAL(saveSelected()), this, SLOT(invokeMenuSaveSelected()));
        connect(&menu, SIGNAL(savePressed()), this, SLOT(invokeMenuSavePressed()));
        connect(&menu, SIGNAL(cancelSelected()), this, SLOT(invokeMenuCancelSelected()));
        connect(&menu, SIGNAL(cancelPressed()), this, SLOT(invokeMenuCancelPressed()));
        connect(this, SIGNAL(keyDown(int)), &menu, SLOT(onKeyDown(int)));
        connect(this, SIGNAL(keyUp(int)), &menu, SLOT(onKeyUp(int)));
        //connect(this, SIGNAL(refreshNeeded()), &menu, SLOT(setActive(bool)));
        menu.load();
    });

   // show();
    isSplitScreen = false;
    nextCampaignTimer = new QTimer(this);
    connect(nextCampaignTimer,SIGNAL(timeout()),this, SLOT(nextCampaignEvent()));
    checkNextVideoAfterStopTimer = new QTimer(this);
    connect(checkNextVideoAfterStopTimer, SIGNAL(timeout()), this, SLOT(nextItemEvent()));
    checkNextVideoAfterStopTimer->setProperty("activated", false);
    checkNextVideoAfterStopTimer->start(5000);

    //view.installEventFilter(this);
    shouldStop = false;
}

TeleDSPlayer::~TeleDSPlayer()
{

}

QString TeleDSPlayer::getFullPath(QString fileName, AbstractPlaylist * playlist)
{
    QString nextFile =  VIDEO_FOLDER + fileName +
                        playlist->findItemById(fileName).file_hash +
                        playlist->findItemById(fileName).file_extension;
    QFileInfo fileInfo(nextFile);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

QString TeleDSPlayer::getFullPathZip(QString path)
{
    QFileInfo fileInfo(path);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

void TeleDSPlayer::show()
{
    qDebug() << "TeleDSPlayer::show";
#ifdef PLAYER_MODE_WINDOWED
    view.setMinimumHeight(520);
    view.setMinimumWidth(920);
    view.setWidth(920);
    view.setHeight(520);
    //view.show();
    view.showFullScreen();
#else
    view.showFullScreen();
#endif
}

void TeleDSPlayer::updateConfig(PlayerConfigAPI &playerConfig)
{
    config = playerConfig;
    foreach (const QString &key, playlists.keys())
        playlists[key]->deleteLater();
    playlists.clear();
    foreach (const PlayerConfigAPI::Campaign &campaign, playerConfig.campaigns)
    {
        foreach (const PlayerConfigAPI::Campaign::Area area, campaign.areas)
        {
            AbstractPlaylist* playlist = new SuperPlaylist(this);
            playlist->updatePlaylist(area);
            playlists[area.area_id] = playlist;
        }
    }
    if (config.currentCampaignId < 0)
        config.currentCampaignId = 0;
}

void TeleDSPlayer::play()
{
    qDebug() << "TeleDSPlayer::play!";
    static int wasEverPlayed = false;
    //
    qDebug() << "status.isPlaying = true;";
    status.isPlaying = true;
    isActive = true;
    int currentCampaignId = config.currentCampaignId;
    qDebug() << "currentCampaignId = " << currentCampaignId << config.campaigns.count();
    if (currentCampaignId < 0)
        currentCampaignId = 0;

    qDebug() << "currentCampaignId = " << currentCampaignId << config.campaigns.count();
    int duration = config.nextCampaign();
    duration -= config.campaigns[currentCampaignId].areas.first().content.count() * 50;
    if (config.campaigns.count() > 1)
        GlobalStatsInstance.setCampaignEndDate(QDateTime::currentDateTimeUtc().addMSecs(duration));
    else
    {
        QDateTime d;
        GlobalStatsInstance.setCampaignEndDate(d);
    }

    invokeSetDelay(config.campaigns[currentCampaignId].delay);
    int index = 0;
    foreach (const PlayerConfigAPI::Campaign::Area &a,config.campaigns[currentCampaignId].areas)
    {
        invokeInitArea(a.area_id,
                       config.campaigns[currentCampaignId].screen_width,
                       config.campaigns[currentCampaignId].screen_height,
                       a.x, a.y, a.width, a.height,
                       config.campaigns[currentCampaignId].rotation,
                       a.opacity, index);
        invokeSetAreaMuted(index, !a.sound_enabled);
        QTimer::singleShot(700, [this, a]() {
            next(a.area_id);
        });
        index++;
    }

    foreach (const PlayerConfigAPI::Campaign::Area &a,config.campaigns[config.currentCampaignId].areas)
    {
        qDebug() << "TeleDSPlayer::reset current index";
        ((SuperPlaylist*)playlists[a.area_id])->resetCurrentItemIndex();
        if (!wasEverPlayed)
        {
            qDebug() << "Trying to fill force items";
            foreach (const QString &i, a.priority_content)
            {
                qDebug() << "adding force item to playlist: " << a.area_id << i;
                playlists[a.area_id]->forceItems.append(i);
            }
        }
    }

    if (config.campaigns.count() > 1) {
        nextCampaignTimer->setInterval(duration);
        nextCampaignTimer->start();
    }
    wasEverPlayed = true;
}

PlayerConfigAPI::Campaign::Area TeleDSPlayer::getAreaById(QString id)
{
    PlayerConfigAPI::Campaign::Area result;
    foreach (const PlayerConfigAPI::Campaign &cmp, config.campaigns)
        foreach (const PlayerConfigAPI::Campaign::Area &area, cmp.areas)
            if (area.area_id == id)
                result = area;
    return result;
}

bool TeleDSPlayer::isPlaying()
{
    qDebug() << "TeleDSPlayer" << status.item << status.isPlaying;
    return status.isPlaying;
}

bool TeleDSPlayer::isFileCurrentlyPlaying(QString name)
{
    return status.item == name;
}

QVector<QString> TeleDSPlayer::getAreas(QString areaString)
{
    QVector<QString> result;
    auto tokens = areaString.split(";");
    foreach (const QString &s, tokens)
        if (!s.simplified().isEmpty())
            result.append(s);
    return result;
}

QString TeleDSPlayer::packAreas(QString oldAreas, QString area)
{
    QString result;
    QVector<QString> areaVector = getAreas(oldAreas);
    if (!areaVector.contains(area))
        areaVector.append(area);
    for (int i = 0; i < areaVector.count(); i++)
    {
        result += areaVector[i];
        if (i != areaVector.count() - 1)
            result += ";";
    }
    return result;
}

bool TeleDSPlayer::eventFilter(QObject *target, QEvent *event)
{
    qDebug() << "TELEDSEVENT!" << event->type();
    if (event->type() == QEvent::KeyPress) {
          QKeyEvent *keyEvent = (QKeyEvent *)event;
          if (keyEvent->key() == Qt::Key_Q) {
            qApp->quit();
            return true;
          }
        }
    return QObject::eventFilter(target,event);
}

void TeleDSPlayer::prepareStop()
{
    shouldStop = true;
    invokePrepareStop();
}

void TeleDSPlayer::onKeyDown(int code)
{
    emit keyDown(code);
}

void TeleDSPlayer::onKeyUp(int code)
{
    emit keyUp(code);
}

void TeleDSPlayer::invokeNextVideoMethodAdvanced(QString name, QString area_id)
{
    if (name == "" || area_id == "" || !playlists.contains(area_id))
    {
        invokeStopMainPlayer();
        return;
    }
    PlayerConfigAPI::Campaign::Area::Content item = playlists[area_id]->findItemById(name);

    QVariant source;
    if (item.type == "video" || item.type == "audio" || item.type == "image")
    {
        source = QUrl(getFullPath(name,playlists[area_id]));
        invokeSetPlayerVolume(GlobalConfigInstance.getVolume());
    }
    else if (item.type == "html5_online")
        source = item.file_url;
    else if (item.type == "html5_zip")
        source = getFullPathZip(VIDEO_FOLDER + item.content_id + "/index.html");

    QVariant type;
    if (item.type == "html5_zip")
        type = "html5_online";
    else
        type = item.type;
    QVariant duration = item.duration;
    QVariant skip = item.play_start;
    QVariant fillMode = item.fill_mode;

    QMetaObject::invokeMethod(viewRootObject, "playNextItem",
                              Q_ARG(QVariant, area_id),
                              Q_ARG(QVariant, source),
                              Q_ARG(QVariant, type),
                              Q_ARG(QVariant, duration),
                              Q_ARG(QVariant, skip),
                              Q_ARG(QVariant, fillMode));
}

void TeleDSPlayer::invokeFileProgress(double p, QString name)
{
    qDebug() << "invoking file progress";
    QVariant percent(p);
    QVariant fileName(name);
    QMetaObject::invokeMethod(viewRootObject,"setTotalProgress",Q_ARG(QVariant, percent), Q_ARG(QVariant, fileName));
}

void TeleDSPlayer::invokeProgress(double p)
{
    qDebug() << "invoking download progress";
    QVariant percent(p);
    QMetaObject::invokeMethod(viewRootObject,"setProgress",Q_ARG(QVariant, percent));
}

void TeleDSPlayer::invokeSimpleProgress(double p, QString)
{
    QVariant percent(p);
    QMetaObject::invokeMethod(viewRootObject,"setDownloadProgressSimple",Q_ARG(QVariant, percent));
}

void TeleDSPlayer::invokeBackDownloadProgressBarVisible(bool isVisible)
{
    QMetaObject::invokeMethod(viewRootObject, "setBackDownloadProgressBarVisible",
                              Q_ARG(QVariant, QVariant(isVisible)));
}

void TeleDSPlayer::invokeDownloadDone()
{
    qDebug() << "invoking download completed";
    QMetaObject::invokeMethod(viewRootObject,"downloadComplete");
    invokeSetDeviceInfo();
}

void TeleDSPlayer::invokeVersionText()
{
    qDebug() << "invoking version text";
    QString pass;
    QVariant versionText(TeleDSVersion::getVersion());
    QVariant versionBuildText(QString::number(TeleDSVersion::BUILD) + " " + pass);
    QMetaObject::invokeMethod(viewRootObject,"setVersion",
                              Q_ARG(QVariant, versionText),
                              Q_ARG(QVariant, versionBuildText));
}

void TeleDSPlayer::invokeShowPassword()
{
    static int showPass = 0;
    showPass = 1 - showPass;
    QString pass;
    if (showPass){
        if (QFile::exists("/home/pi/password.txt"))
        {
            QFile f("/home/pi/password.txt");
            if (f.open(QFile::ReadOnly))
            {
                pass = QString(f.readAll()).replace("\n","");
                f.close();
            }
        }
    }
    QMetaObject::invokeMethod(viewRootObject, "showPassword",
                              Q_ARG(QVariant, QVariant(showPass)),
                              Q_ARG(QVariant, QVariant(pass)));
}

void TeleDSPlayer::invokeShowInternetConnectionInfo()
{
    static int show = 0;
    show = 1 - show;
    QString ifconfig;
    if (show)
    {
        QProcess p;
        p.start("ifconfig");
        p.waitForStarted();
        p.waitForFinished();
        ifconfig = QString(p.readAll()).replace("\n", "<br/>");

        QMetaObject::invokeMethod(viewRootObject, "showConnectionMenu",
                                  Q_ARG(QVariant, QVariant(ifconfig)));
    }
    else
        QMetaObject::invokeMethod(viewRootObject, "hideConnectionMenu");
}

void TeleDSPlayer::invokePlayerCRC()
{
    qDebug() << "invoking version text";
    QVariant crc(GlobalStatsInstance.getCRC32Hex());
    QMetaObject::invokeMethod(viewRootObject,"setCRC",Q_ARG(QVariant, crc));
}

void TeleDSPlayer::invokeToggleVisibility(int status)
{
    qDebug() << "invokeToggleVisibility";
    QMetaObject::invokeMethod(viewRootObject, "toggleVisibility", Q_ARG(QVariant, QVariant(status)));
}

void TeleDSPlayer::invokeSetDelay(int delay)
{
    qDebug() << "TeleDSPlayer::invokeSetDelay";
    QMetaObject::invokeMethod(viewRootObject, "setDelay", Q_ARG(QVariant, QVariant(delay)));
}

void TeleDSPlayer::invokePlayerActivationRequiredView(QString url, QString playerId, bool showCode)
{
    qDebug() << "invokePlayerActivationRequiredView";
    QVariant urlParam(url);
    QVariant playerIdParam("  " + playerId.toUpper() + "  ");
    QMetaObject::invokeMethod(viewRootObject,"setNeedActivationLogo",
                              Q_ARG(QVariant, urlParam),
                              Q_ARG(QVariant, playerIdParam),
                              Q_ARG(QVariant, QVariant(showCode)));

    invokeSetDeviceInfo();
}

void TeleDSPlayer::invokeNoItemsView(QString url)
{
    qDebug() << "invokeNoItemsView";
    QVariant urlParam(url);
    QMetaObject::invokeMethod(viewRootObject,"setNoItemsLogo", Q_ARG(QVariant, urlParam));
    invokeSetDeviceInfo();
}

void TeleDSPlayer::invokeDownloadingView()
{
    qDebug() << "invokeDownloading View";
    QMetaObject::invokeMethod(viewRootObject,"setDownloadLogo");
}

void TeleDSPlayer::invokeStop()
{
    qDebug() << "TeleDSPlayer::invokeStop";
    nextCampaignTimer->stop();
    QMetaObject::invokeMethod(viewRootObject, "stopPlayer");
}

void TeleDSPlayer::invokePrepareStop()
{
    qDebug() << "TeleDSPlayer::invokePrepareStop";
    QMetaObject::invokeMethod(viewRootObject, "prepareStop");
}

void TeleDSPlayer::invokeStopMainPlayer()
{
    qDebug() << "TeleDSPlayer::invokeStopMainPlayer";
    QMetaObject::invokeMethod(viewRootObject, "stopMainPlayer");
    QTimer::singleShot(10000,this,SLOT(runAfterStop()));
}

void TeleDSPlayer::invokeSetTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, QString color3, bool tileMode, bool showTeleDSLogo)
{
    qDebug() << "TeleDSPlayer::invokeSetTheme";
    QVariant backgroundURLParam = QUrl(backgroundURL);
    QVariant logoURLParam = QUrl(logoURL);
    QVariant color1Param = QColor(color1);
    QVariant color2Param = QColor(color2);
    QVariant color3Param = QColor(color3);
    QVariant tileModeParam = tileMode;
    QVariant showTeleDSLogoParam = showTeleDSLogo;
    QMetaObject::invokeMethod(viewRootObject, "setTheme",
                              Q_ARG(QVariant, backgroundURLParam),
                              Q_ARG(QVariant, logoURLParam),
                              Q_ARG(QVariant, color1Param),
                              Q_ARG(QVariant, color2Param),
                              Q_ARG(QVariant, color3Param),
                              Q_ARG(QVariant, tileModeParam),
                              Q_ARG(QVariant, showTeleDSLogoParam));

    this->show();
}

void TeleDSPlayer::invokeSetMenuTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, bool tileMode, bool showTeleDSLogo)
{
    //setMenuTheme(brandBGLogo, brandLogo, brandBGColor, brandFGColor, tileMode, showTeleDSLogo)
    qDebug() << "TeleDSPlayer::invokeSetMenuTheme";
    QVariant backgroundURLParam = QUrl(backgroundURL);
    QVariant logoUrlParam = QUrl(logoURL);
    QVariant color1Param = QColor(color1);
    QVariant color2Param = QColor(color2);
    QVariant tileModeParam = tileMode;
    QVariant showTeleDSLogoParam = showTeleDSLogo;

    QMetaObject::invokeMethod(viewRootObject, "setMenuTheme",
                              Q_ARG(QVariant, backgroundURLParam),
                              Q_ARG(QVariant, logoUrlParam),
                              Q_ARG(QVariant, color1Param),
                              Q_ARG(QVariant, color2Param),
                              Q_ARG(QVariant, tileModeParam),
                              Q_ARG(QVariant, showTeleDSLogoParam));
}

void TeleDSPlayer::invokeRestoreDefaultTheme()
{
    qDebug() << "TeleDSPlayer::invokeRestoreDefaultTheme";
    QMetaObject::invokeMethod(viewRootObject, "restoreDefaultTheme");
    this->show();
}

void TeleDSPlayer::invokeToggleMenu()
{
    static int i = 0;
    i = 1 - i;
    menu.setActive(i);
    qDebug() << "TeleDSPlayer::invokeToggleMenu";
    QMetaObject::invokeMethod(viewRootObject, "toggleMenu");
}

void TeleDSPlayer::invokeUpdateState()
{
    qDebug() << "TeleDSPlayer::invokeUpdateState";
    QMetaObject::invokeMethod(viewRootObject, "setUpdateState", Q_ARG(QVariant, QVariant(TeleDSVersion::getVersion())));
}

void TeleDSPlayer::invokeSetAreaCount(int areaCount)
{
    qDebug() << "TeleDSPlayer::invokeSetAreaCount";
    QMetaObject::invokeMethod(viewRootObject, "setAreaCount", Q_ARG(QVariant, QVariant(areaCount)));
}

void TeleDSPlayer::invokePlayCampaign(int campaignIndex)
{
    //for each of area in campaign - ask next Item and play it via next(areaID);
    config.currentCampaignId = campaignIndex;
    PlayerConfigAPI::Campaign currentCampaign = config.campaigns[campaignIndex];
    foreach (const PlayerConfigAPI::Campaign::Area &area, currentCampaign.areas)
        next(area.area_id);
}

void TeleDSPlayer::invokeInitArea(QString name, double campaignWidth, double campaignHeight, double x, double y, double w, double h, int rotation, double opacity, int index)
{
    qDebug() << "TeleDSPlayer::invokeInitArea";
    QMetaObject::invokeMethod(viewRootObject, "prepareArea",
                              Q_ARG(QVariant, QVariant(name)),
                              Q_ARG(QVariant, QVariant(campaignWidth)),
                              Q_ARG(QVariant, QVariant(campaignHeight)),
                              Q_ARG(QVariant, QVariant(x)),
                              Q_ARG(QVariant, QVariant(y)),
                              Q_ARG(QVariant, QVariant(w)),
                              Q_ARG(QVariant, QVariant(h)),
                              Q_ARG(QVariant, QVariant(rotation)),
                              Q_ARG(QVariant, QVariant(opacity)),
                              Q_ARG(QVariant, QVariant(index)));
}

void TeleDSPlayer::invokeSetPlayerVolume(int value)
{
    QMetaObject::invokeMethod(viewRootObject, "setPlayerVolume", Q_ARG(QVariant,QVariant(value/100.0)));
}

void TeleDSPlayer::invokeSetLicenseData()
{
    qDebug() << "TeleDSPlayer::invokeSetLicenseData";
    QVariant eulaParam = StaticTextService.getEula();
    QString eula = StaticTextService.getEula();
    qDebug() << "TeleDSPlayer::invoke eula(" + QString::number(eula.length()) + ")";
    QVariant privacyPolicyParam = StaticTextService.getPrivacyPolicy();
    QVariant opensourceParam = StaticTextService.getOpenSource();
    QVariant legalParam = StaticTextService.getLegal();
    QMetaObject::invokeMethod(viewRootObject, "setLicenseText",
                              Q_ARG(QVariant, eulaParam),
                              Q_ARG(QVariant, privacyPolicyParam),
                              Q_ARG(QVariant, opensourceParam),
                              Q_ARG(QVariant, legalParam));
}

void TeleDSPlayer::invokeSetDeviceInfo()
{

    qDebug() << "TeleDSPlayer::invokeSetDeviceInfo";
    SettingsRequestResult settings = SettingsRequestResult::fromJson(GlobalConfigInstance.getSettings());
    QVariant nameParam = settings.name;
    QVariant connectionName = PlatformSpecificService.getConnectionName();
    QMetaObject::invokeMethod(viewRootObject, "setDeviceInfo",
                              Q_ARG(QVariant, nameParam),
                              Q_ARG(QVariant, connectionName));
}

void TeleDSPlayer::invokeSetDisplayMode(QString mode)
{
    qDebug() << "TeleDSPlayer::invokeSetDisplayMode -> " << mode;
    QVariant modeParam = mode;
    if (mode == "fullscreen")
        invokeSetContentPosition();

    QMetaObject::invokeMethod(viewRootObject, "setDisplayMode",
                              Q_ARG(QVariant, modeParam));
}

void TeleDSPlayer::invokeSetContentPosition(float contentLeft, float contentTop, float contentWidth, float contentHeight,
                                            float widgetLeft, float widgetTop, float widgetWidth, float widgetHeight)
{
    qDebug() << "TeleDSPlayer::invokeSetContentPosition";
    QMetaObject::invokeMethod(viewRootObject, "setContentPosition",
                              Q_ARG(QVariant, QVariant(contentLeft)),
                              Q_ARG(QVariant, QVariant(contentTop)),
                              Q_ARG(QVariant, QVariant(contentWidth)),
                              Q_ARG(QVariant, QVariant(contentHeight)),
                              Q_ARG(QVariant, QVariant(widgetLeft)),
                              Q_ARG(QVariant, QVariant(widgetTop)),
                              Q_ARG(QVariant, QVariant(widgetWidth)),
                              Q_ARG(QVariant, QVariant(widgetHeight)));
}

void TeleDSPlayer::invokeSkipCurrentItem()
{
    qDebug() << "TeleDSPlayer::invokeSkipCurrentItem";
    QMetaObject::invokeMethod(viewRootObject, "skipCurrentItem");
}

void TeleDSPlayer::invokeSetAreaMuted(int index, bool isMuted)
{
    qDebug() << "TeleDSPlayer::invokeSetAreaMuted";
    QMetaObject::invokeMethod(viewRootObject, "setAreaMuted",
                              Q_ARG(QVariant, QVariant(index)),
                              Q_ARG(QVariant, QVariant(isMuted)));
}

void TeleDSPlayer::invokeMenuDisplayRotationSelected()
{
    qDebug() << "invokeMenuDisplayRotationSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuDisplayRotationSelected");
}

void TeleDSPlayer::invokeMenuDisplayRotationChanged(int value, QString text)
{
    qDebug() << "invokeMenuDisplayRotationChanged" << value << text;
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuDisplayRotationChanged",
                              Q_ARG(QVariant, QVariant(value)),
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuHDMIGroupSelected()
{
    qDebug() << "invokeMenuHDMIGroupSelected";

    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIGroupSelected");
}

void TeleDSPlayer::invokeMenuHDMIGroupChanged(int value, QString text)
{
    qDebug() << "invokeMenuHDMIGroupChanged" << value << text;
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIGroupChanged",
                              Q_ARG(QVariant, QVariant(value)),
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuHDMIModeSelected()
{
    qDebug() << "invokeMenuHDMIModeSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIModeSelected");
}

void TeleDSPlayer::invokeMenuHDMIModeChanged(int value, QString text)
{
    qDebug() << "invokeMenuHDMIModeChanged" << value << text;
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIModeChanged",
                              Q_ARG(QVariant, QVariant(value)),
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuHDMIDriveSelected()
{
    qDebug() << "invokeMenuHDMIDriveSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIDriveSelected");
}

void TeleDSPlayer::invokeMenuHDMIDriveChanged(int value, QString text)
{
    qDebug() << "invokeMenuHDMIDriveChanged" << value << text;
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIDriveChanged",
                              Q_ARG(QVariant, QVariant(value)),
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuHDMIBoostSelected()
{
    qDebug() << "invokeMenuHDMIBoostSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIBoostSelected");
}

void TeleDSPlayer::invokeMenuHDMIBoostChanged(int value)
{
    qDebug() << "invokeMenuHDMIBoostChanged" << value;
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuHDMIBoostChanged",
                              Q_ARG(QVariant, QVariant(value)));
}

void TeleDSPlayer::invokeMenuWifiNetworkSelected()
{
    qDebug() << "invokeMenuWifiNetworkSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuWifiNetworkSelected");
}

void TeleDSPlayer::invokeMenuWifiNetworkChanged(QString text)
{
    qDebug() << "invokeMenuWifiNetworkChanged";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuWifiNetworkChanged",
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuWifiNameSelected()
{
    qDebug() << "invokeMenuWifiNameSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuWifiNameSelected");
}

void TeleDSPlayer::invokeMenuWifiNameChanged(QString text)
{
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuWifiNameChanged",
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuWifiPassSelected()
{
    qDebug() << "invokeMenuWifiPassSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuWifiPassSelected");
}

void TeleDSPlayer::invokeMenuWifiPassChanged(QString text)
{
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuWifiPassChanged",
                              Q_ARG(QVariant, QVariant(text)));
}

void TeleDSPlayer::invokeMenuSaveSelected()
{
    qDebug() << "invokeMenuSaveSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuSaveSelected");
}

void TeleDSPlayer::invokeMenuSavePressed()
{
    qDebug() << "invokeMenuSavePressed";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuSavePressed");
}

void TeleDSPlayer::invokeMenuCancelSelected()
{
    qDebug() << "invokeMenuCancelSelected";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuCancelSelected");
}

void TeleDSPlayer::invokeMenuCancelPressed()
{
    qDebug() << "invokeMenuCancelPressed";
    QMetaObject::invokeMethod(viewRootObject, "invokeMenuCancelPressed");
    invokeToggleMenu();
}

void TeleDSPlayer::invokeTogglePlayerIDVisible()
{
    qDebug() << "invokeTogglePlayerIDVisible";
    static int isVisible = 0;
    isVisible = 1 - isVisible;
    QMetaObject::invokeMethod(viewRootObject, "setPlayerIdHiddenMode",
                              Q_ARG(QVariant, QVariant(bool(isVisible))));
}

void TeleDSPlayer::runAfterStop()
{
    /*
    qDebug() << "TeleDSPlayer::runAfterStop";
    bool haveNext = playlist->haveNext();
    next();
    if (haveNext)
    {
        qDebug() << "TeleDSPlayer::runAfterStop -> we have next item";
        QTimer::singleShot(1000,this,SLOT(next()));
    }*/
}

void TeleDSPlayer::next(QString area_id)
{
    qDebug() << "TeleDSPlayer::next " << area_id;

    if (shouldStop)
    {
        shouldStop = false;
        emit readyToUpdate();
        return;
    }
    if (isActive)
    {
        qDebug() << "next method is called";
        playNextGeneric(area_id);
    }
    else
    {
        qDebug() << "Player is not active, so no next item";
    }
}

void TeleDSPlayer::playNextGeneric(QString area_id)
{
    qDebug() << QDateTime::currentDateTime().time();
    qDebug() << "playNextGeneric!" << area_id;
    if (!playlists.contains(area_id))
    {
        qDebug() << "playlist " << area_id << " not found";
        return;
    }
    //call haveNext instead
    //if haveNext has none then dont play anything
    if (playlists[area_id]->haveNext())
    {
        QString nextItem = playlists[area_id]->getStoredItem();
        invokeNextVideoMethodAdvanced(nextItem,area_id);
        if (GlobalConfigInstance.isAutoBrightnessActive())
        {
            SunsetSystem sunSystem;

            double minBrightness = std::min(GlobalConfigInstance.getMinBrightness(), GlobalConfigInstance.getMaxBrightness());
            double maxBrightness = std::max(GlobalConfigInstance.getMinBrightness(), GlobalConfigInstance.getMaxBrightness());
            double brightnessSinValue = std::min(sunSystem.getSinPercent()*4.0,1.0);
            double brightnessValue = brightnessSinValue * (maxBrightness - minBrightness) + minBrightness;
            brightnessValue = std::max(std::min(brightnessValue, maxBrightness), minBrightness);
            qDebug() << "BRIGHTNESS = " << brightnessValue;
            if (brightnessValue/100. < 0)
                setBrightness(1.0);
            else
                setBrightness(brightnessValue/100.);
        }
        else
            setBrightness(1.0);
        playedIds.enqueue(nextItem);
        PlatformSpecificService.generateSystemInfo();

        qDebug() << "set next playnextgeneric::status.isPlaying=true;";
        status.isPlaying = true;
        status.item = nextItem;
        GlobalStatsInstance.setCurrentItem(nextItem);
        showVideo();
    }
    else
    {
        qDebug() << "Playlist doesnt have next item: stopping!";
        invokePrepareStop();

        //this->playNextGeneric(area_id);

        checkNextVideoAfterStopTimer->setProperty("area_id", packAreas(checkNextVideoAfterStopTimer->property("area_id").toString(), area_id));
         //checkNextVideoAfterStopTimer->setProperty("area_id", area_id);
        checkNextVideoAfterStopTimer->setProperty("activated", true);
        //we need to stop playback after last item end
        //invoke prepare stop
    }
    qDebug() << QDateTime::currentDateTime().time();
}

void TeleDSPlayer::bindObjects()
{
    qDebug() << "binding QML and C++";
    connect(&PlatformSpecificService,SIGNAL(systemInfoReady(Platform::SystemInfo)),this,SLOT(systemInfoReady(Platform::SystemInfo)));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
    QObject::connect(viewRootObject,SIGNAL(refreshId()), this, SIGNAL(refreshNeeded()));
    QObject::connect(viewRootObject,SIGNAL(gpsChanged(double,double)),this,SLOT(gpsUpdate(double,double)));
    QObject::connect(viewRootObject,SIGNAL(setRestoreModeTrue()),this,SLOT(setRestoreModeTrue()));
    QObject::connect(viewRootObject,SIGNAL(setRestoreModeFalse()),this, SLOT(setRestoreModeFalse()));
    QObject::connect(viewRootObject,SIGNAL(nextItem(QString)), this, SLOT(next(QString)));
}


void TeleDSPlayer::stopPlaying()
{
    isActive = false;
    status.isPlaying = false;
    invokeStop();
    invokeNoItemsView("http://teleds.com");
}

void TeleDSPlayer::setRestoreModeTrue()
{
    Platform::PlatformSpecific::setResetWindow(true);
}

void TeleDSPlayer::setRestoreModeFalse()
{
    Platform::PlatformSpecific::setResetWindow(false);
}

void TeleDSPlayer::showVideo()
{
    invokeShowVideo(true);
}

void TeleDSPlayer::hideVideo()
{
    invokeShowVideo(false);
}

void TeleDSPlayer::setBrightness(double value)
{
    QVariant brightness(value);
    QMetaObject::invokeMethod(viewRootObject,"setBrightness",Q_ARG(QVariant, brightness));
}

void TeleDSPlayer::gpsUpdate(double lat, double lgt)
{
    GlobalStatsInstance.setGps(lat, lgt);
}

void TeleDSPlayer::systemInfoReady(Platform::SystemInfo info)
{
    qDebug() << "TeleDSPlayer::systemInfoReady()";
    foreach (const QString &areaId, playlists.keys())
    {
        qDebug() << areaId;
        auto playlist = playlists[areaId];
        if (playlist){
            while (!playedIds.isEmpty())
            {
                auto item = playlists[areaId]->findItemById(playedIds.head());
                qDebug() << item.content_id;
                if (item.content_id != "")
                {
                    DatabaseInstance.createPlayEvent(item, info);
                    playedIds.dequeue();
                    //break;
                }
                else
                {
                    qDebug() << "This area doesnt contain such item";
                    break;
                }
            }
        }
    }

    qDebug() << "~~~TeleDSPlayer::systemInfoReady()";
}

void TeleDSPlayer::nextCampaignEvent()
{
    qDebug() << "nextCampaignEvent";
    this->invokeStop();
    checkNextVideoAfterStopTimer->stop();
    this->play();
}

void TeleDSPlayer::nextItemEvent()
{
    if (checkNextVideoAfterStopTimer->property("activated").toBool())
    {
        checkNextVideoAfterStopTimer->setProperty("activated", false);

        QVector<QString> areas = getAreas(checkNextVideoAfterStopTimer->property("area_id").toString());
        foreach (const QString &area, areas)
            this->playNextGeneric(area);
        //checkNextVideoAfterStopTimer->setProperty("area_id","");
        //this->playNextGeneric(checkNextVideoAfterStopTimer->property("area_id").toString());
    }

}

void TeleDSPlayer::invokeShowVideo(bool isVisible)
{
    //qDebug() << "invoking video visibility change -> " + (isVisible ? QString("true") : QString("false"));
    QVariant isVisibleArg(isVisible);
    QMetaObject::invokeMethod(viewRootObject,"showVideo",Q_ARG(QVariant, isVisibleArg));
}

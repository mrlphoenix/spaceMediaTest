import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.2
//import QtWebKit 3.0
import QtWebView 1.1


Item {
    id: item
    property bool invokeNext:true
    property double videoOutBrightness: 1.0
    property double hValue: Math.min(height, width)
    property double aspect: Math.min(hValue / 1080.0, 1.0)
    property int heightP: height
    property int widthP: width
    property int decreasingTextValue: 0


    //properties for split-view
    property double contentWidthPercent: 100.0
    property double contentHeightPercent: 100.0
    property double contentLeftPercent: 0.0
    property double contentTopPercent: 0.0
    property double widgetWidthPercent: 0.0
    property double widgetHeightPercent: 0.0
    property double widgetLeftPercent: 0.0
    property double widgetTopPercent: 0.0

    property double contentWidth: contentWidthPercent * item.width / 100
    property double contentHeight: contentHeightPercent * item.height / 100
    property double contentLeft: contentLeftPercent * item.width / 100
    property double contentTop: contentTopPercent * item.height / 100
    property double widgetWidth: widgetWidthPercent * item.width / 100
    property double widgetHeight: widgetHeightPercent * item.height / 100
    property double widgetLeft: widgetLeftPercent * item.width / 100
    property double widgetTop: widgetTopPercent * item.height / 100

    //properties for branding
    //
    property color brand_blackColor:                "#000000"
    property color brand_whiteColor:                "#FFFFFF"
    property color brand_backgroundColor:           "#333e47"
    property color brand_foregroundColor:           "#00cdc1"
    property color brand_borderGrayColor:           "#d7d7d7"
    //properties for text
    property string brand_nothingToPlayText:        "Nothing to play"
    property string brand_nothingToPlayLinkText:    "teleds.com"
    property string brand_downloadingText:          "Downloading..."
    property string brand_pleaseWaitText:           "Please wait..."
    property string brand_setupText:                "To setup this player use the following code at"
    property string brand_waitingForActivationText: "Waiting for activation"
    property string brand_initialization:           "Initialization"
    //properties for images
    property url brand_backgroundLogoHorizonal:     "logo_bg.svg"
    property url brand_backgroundLogoVertical:      "logo_bg_vertical.svg"
    property url brand_logoImage:                   "logo.svg"
    property url brand_logoLabel:                   "logo_teleds.png"
    property url brand_audioIcon:                   "audio.svg"
    property url brand_menu_bg_image:               "menu_background_horizontal.png"
    //properties for menu branding
    property color brand_menu_backgroundColor:      "white"
    property color brand_menu_foregroundColor:      "#00cdc1"
    property url brand_menu_background:             "menu_background_horizontal.png"
    property url brand_menu_logo:                   "logo.svg"

    //properties for other branding
    property bool brand_show_teleds_logo:           true
    property bool brand_menu_show_teleds_logo:      true
    property bool brand_tile_background:            false
    property bool brand_menu_tile_background:       false


    //properties for default brand theme
    property color brand_default_blackColor:                "#000000"
    property color brand_default_whiteColor:                "#FFFFFF"
    property color brand_default_backgroundColor:           "#333e47"
    property color brand_default_foregroundColor:           "#00cdc1"
    property color brand_default_borderGrayColor:           "#d7d7d7"
    property url brand_default_backgroundLogoHorizonal:     "logo_bg.svg"
    property url brand_default_backgroundLogoVertical:      "logo_bg_vertical.svg"
    property url brand_default_logoImage:                   "logo.svg"
    property url brand_default_logoLabel:                   "logo_teleds.png"
    property url brand_default_audioIcon:                   "audio.svg"
    property url brand_default_menu_bg_image:               "menu_background_horizontal.png"

    property color brand_default_menu_backgroundColor:      "white"
    property color brand_default_menu_foregroundColor:      "#00cdc1"
    property url brand_default_menu_background:             "menu_background_horizontal.png"
    property url brand_default_menu_logo:                   "logo.svg"

    //properties for seamless video player
    property bool preloader: false
    property bool useSecondPlayer: false
    property bool askNextItemOnStop: true

    //properties for cross content managing
    property string currentType: "null"

    property string systemDeviceName: ""
    property string systemConnectionName: ""
    property string systemEULAText: ""
    property string systemPrivacyPolicyText: ""
    property string systemOpensourceText: ""
    property string systemLegalText: ""
    property string systemVersion: ""
    property string systemVersionBuild: ""
    property string displayMode: "fullscreen"

    property bool isPlayerCodeVisibleState: false
    property bool isPlayerCodeVisibleMode: false

    property int playlistDelay: 0


    signal nextItem(string areaID)
    signal nextWidget()
    signal refreshId()
    signal gpsChanged(double lat, double lgt)
    signal setRestoreModeTrue()
    signal setRestoreModeFalse()

    focus: true

    onHeightChanged: {
        heightP = height
        widthP = width
    }

    function setDelay(delay)
    {
        playlistDelay = delay
    }

    function setRotation(angle)
    {
        if (angle % 180 != 0)
        {
            item.height = parent.width
            item.width = parent.height
            item.x = (parent.width - parent.height) / 2
            item.y = (parent.height - parent.width) / 2
        }
        else
        {
            item.width = parent.width
            item.height = parent.height
            item.x = 0
            item.y = 0
        }
        item.rotation = angle
    }

    function playerisReady()
    {
        prepareLoadRect.visible = false
    }

    function setSpaceMediaTheme()
    {
        brand_backgroundColor = "#F2F2F2"
        brand_foregroundColor = "#C9353D"
        brand_nothingToPlayText = "Нет роликов для воспроизведения"
        brand_nothingToPlayLinkText = ""
        brand_downloadingText = "Загрузка роликов"
        brand_setupText = ""
        brand_waitingForActivationText = ""
        brand_initialization = "Инициализация плеера..."
        //
        brand_backgroundLogoHorizonal = ""
        brand_backgroundLogoVertical = ""
        brand_logoImage = "sm_bg.png"

        teledsLogoImg.y = getHeightPercentSize(0.125925926)
        teledsLogoImg.width = getWidthPercentSize(0.515625)
        teledsLogoImg.height = getHeightPercentSize(0.333333)

        teledsLogoImg.sourceSize.width =  getWidthPercentSize(0.515625*4)
        teledsLogoImg.sourceSize.height = getHeightPercentSize(0.333333*4)

        teledsLogo.height = 0
        brRect.height = 0

        progressText.visible = false


        playerIDRect.x = getWidthPercentSize(0.393229167)
        playerIDRect.y = getHeightPercentSize(0.599074074)
        playerIDRect.width = getWidthPercentSize(0.211458333)
        playerIDRect.height = getHeightPercentSize(0.109259259)
        playerIDText.font.family = ptSansFont.name
        playerIDText.font.pixelSize = getHeightPercentSize(0.064)

        //playerIDText.x = getWidthPercentSize(0.019797917 / 6.0)
        playerIDText.y = getHeightPercentSize(0.027777778 / 2.5)

        titleText.color = brand_blackColor
        spacemediaContactsBlock.visible = true

        logoDownloadProgressBar.y = getHeightPercentSize(0.673148148)
        logoDownloadProgressBar.height = getHeightPercentSize(0.055555556)
        titleText.y = getHeightPercentSize(0.58)


        crcText.color = brand_blackColor
        versionText.color = brand_blackColor

        fullscreenView.browserVisible = true
        fullscreenView2.browserVisible = true
        fullscreenView3.browserVisible = true
        fullscreenView4.browserVisible = true
    }

    function setSpaceMediaThemeBlack()
    {
        brand_backgroundColor = "#2F2F3A"
        brand_foregroundColor = "#C9353D"
        brand_nothingToPlayText = "Нет роликов для воспроизведения"
        brand_nothingToPlayLinkText = ""
        brand_downloadingText = "Загрузка роликов"
        brand_setupText = ""
        brand_waitingForActivationText = ""
        brand_initialization = "Инициализация плеера..."
        //
        brand_backgroundLogoHorizonal = ""
        brand_backgroundLogoVertical = ""
        brand_logoImage = "sm_bg_black.png"

        teledsLogoImg.y = getHeightPercentSize(0.125925926)
        teledsLogoImg.width = getWidthPercentSize(0.515625)
        teledsLogoImg.height = getHeightPercentSize(0.333333)

        teledsLogoImg.sourceSize.width =  getWidthPercentSize(0.515625*4)
        teledsLogoImg.sourceSize.height = getHeightPercentSize(0.333333*4)

        teledsLogo.height = 0
        brRect.height = 0

        progressText.visible = false


        playerIDRect.x = getWidthPercentSize(0.393229167)
        playerIDRect.y = getHeightPercentSize(0.599074074)
        playerIDRect.width = getWidthPercentSize(0.211458333)
        playerIDRect.height = getHeightPercentSize(0.109259259)
        playerIDText.font.family = ptSansFont.name
        playerIDText.font.pixelSize = getHeightPercentSize(0.064)
        playerIDText.y = getHeightPercentSize(0.027777778 / 2.5)
        playerIDText.color = brand_whiteColor

        titleText.color = brand_whiteColor
        spacemediaContactsBlock.visible = true

        logoDownloadProgressBar.y = getHeightPercentSize(0.673148148)
        logoDownloadProgressBar.height = getHeightPercentSize(0.055555556)
        titleText.y = getHeightPercentSize(0.58)


        crcText.color = brand_whiteColor
        versionText.color = brand_whiteColor

        spacemediaContactsPhoneText.color = brand_whiteColor
        spacemediaContactsEmailText.color = brand_whiteColor

        fullscreenView.browserVisible = true
        fullscreenView2.browserVisible = true
        fullscreenView3.browserVisible = true
        fullscreenView4.browserVisible = true
    }

    //spacemedia menu functions
    function invokeMenuDisplayRotationSelected()
    {
        menu.invokeMenuDisplayRotationSelected()
    }

    function invokeMenuDisplayRotationChanged(value, text)
    {
        menu.invokeMenuDisplayRotationChanged(value, text)
    }

    function invokeMenuHDMIGroupSelected()
    {
        menu.invokeMenuHDMIGroupSelected()
    }

    function invokeMenuHDMIGroupChanged(value, text)
    {
        menu.invokeMenuHDMIGroupChanged(value, text)
    }

    function invokeMenuHDMIModeSelected()
    {
        menu.invokeMenuHDMIModeSelected()
    }

    function invokeMenuHDMIModeChanged(value, text)
    {
        menu.invokeMenuHDMIModeChanged(value, text)
    }

    function invokeMenuHDMIDriveSelected()
    {
        menu.invokeMenuHDMIDriveSelected()
    }

    function invokeMenuHDMIDriveChanged(value, text)
    {
        menu.invokeMenuHDMIDriveChanged(value, text)
    }

    function invokeMenuHDMIBoostSelected()
    {
        menu.invokeMenuHDMIBoostSelected()
    }

    function invokeMenuHDMIBoostChanged(value)
    {
        menu.invokeMenuHDMIBoostChanged(value)
    }

    function invokeMenuWifiNetworkSelected()
    {
        menu.invokeMenuWifiNetworkSelected()
    }

    function invokeMenuWifiNetworkChanged(text)
    {
        menu.invokeMenuWifiNetworkChanged(text)
    }

    function invokeMenuWifiNameSelected()
    {
        menu.invokeMenuWifiNameSelected()
    }

    function invokeMenuWifiNameChanged(text)
    {
        menu.invokeMenuWifiNameChanged(text)
    }

    function invokeMenuWifiPassSelected()
    {
        menu.invokeMenuWifiPassSelected()
    }

    function invokeMenuWifiPassChanged(text)
    {
        menu.invokeMenuWifiPassChanged(text)
    }

    function invokeMenuSaveSelected()
    {
        menu.invokeMenuSaveSelected()
    }

    function invokeMenuSavePressed()
    {
        menu.invokeMenuSavePressed()
    }

    function invokeMenuCancelSelected()
    {
        menu.invokeMenuCancelSelected()
    }

    function invokeMenuCancelPressed()
    {
        menu.invokeMenuCancelPressed()
    }

    function prepareArea(name, campaignWidth, campaignHeight, x, y, w, h, _rotation, opacity, index)
    {
        console.log("OPACITY = " + opacity)
        switch (index)
        {
        case 0:
            fullscreenView.prepare(name, campaignWidth, campaignHeight, x, y, w, h , 0, width, height, opacity)
            break;
        case 1:
            fullscreenView2.prepare(name, campaignWidth, campaignHeight, x, y, w, h , 0, width, height, opacity)
            break;

        case 2:
            fullscreenView3.prepare(name, campaignWidth, campaignHeight, x, y, w, h , 0, width, height, opacity)
            break;

        case 3:
            fullscreenView4.prepare(name, campaignWidth, campaignHeight, x, y, w, h , 0, width, height, opacity)
            break;
        default: break
        }
        item.setRotation(_rotation)
    }

    function setAreaMuted(index, isMuted)
    {
        switch (index)
        {
        case 0:
            fullscreenView.setMuted(isMuted)
            break
        case 1:
            fullscreenView2.setMuted(isMuted)
            break
        case 2:
            fullscreenView3.setMuted(isMuted)
            break
        case 3:
            fullscreenView4.setMuted(isMuted)
            break
        default:
            break
        }
    }

   /* function prepareArea(name, campaignWidth, campaignHeight, x, y, w, h, _rotation)
    {
        console.log("prepareArea " + campaignWidth + " " + campaignHeight + " " + w + " " + h)
        fullscreenView.areaID = name
        fullscreenView.x = width * x / campaignWidth;
        fullscreenView.y = height * y / campaignHeight;
        fullscreenView.width = width * w / campaignWidth;
        fullscreenView.height = height * h / campaignHeight;
        fullscreenView.setRotation(_rotation)
        item.focus = true
*/
      /*  fullscreenView.areaID = name
        fullscreenView.x = 0
        fullscreenView.y = 0
        fullscreenView.height = item.height
        fullscreenView.width = item.width*/
       // moveTimer.start()
   // }

    function prepareStop()
    {
        fullscreenView.prepareStop()
        fullscreenView2.prepareStop()
        fullscreenView3.prepareStop()
        fullscreenView4.prepareStop()
    }

    Timer {
        id: moveTimer
        repeat: true
        interval: 50
        onTriggered: {
            fullscreenView.x = fullscreenView.x + 1
        }
    }

    function setCRC(crc)
    {
        crcText.text = crc
    }

    function toggleVisibility(status)
    {
        if (status === 0)
        {
            hideRect.visible = true
            fullscreenView.setMuted(true)
        }
        else
        {
            hideRect.visible = false
            fullscreenView.setMuted(false)
        }
        item.focus = true
    }

    function playNextItem(areaId, source, type, duration, skipTime, fillMode)
    {
        if (fullscreenView.areaID === areaId)
        {
            if (fullscreenView.visible == false)
                fullscreenView.visible = true
            fullscreenView.playStandartMode(source, duration, type, skipTime, fillMode)
        }
        else if (fullscreenView2.areaID === areaId)
        {
            if (fullscreenView2.visible == false)
                fullscreenView2.visible = true
            fullscreenView2.playStandartMode(source, duration, type, skipTime, fillMode)
        }
        else if (fullscreenView3.areaID === areaId)
        {
            if (fullscreenView3.visible == false)
                fullscreenView3.visible = true
            fullscreenView3.playStandartMode(source, duration, type, skipTime, fillMode)
        }
        else if (fullscreenView4.areaID === areaId)
        {
            if (fullscreenView4.visible == false)
                fullscreenView4.visible = true
            fullscreenView4.playStandartMode(source, duration, type, skipTime, fillMode)
        }
        else
            console.log("playNextItem::ERROR! Area " + areaId + " not found!")
    }

    function toggleMenu()
    {
        menu.visible = !menu.visible
    }

    function showConnectionMenu(text)
    {
        oldMenu.visible = true
        oldMenu.showInternetInfoDialog(text)
        //menu.restoreTextDialog(text, "")
    }

    function hideConnectionMenu()
    {
        oldMenu.visible = false
        oldMenu.hideTextDialog()
    }

    function setPlayerVolume(value)
    {
        console.log("player volume " + value)
        //fullscreenView.volumeValue = value
        changeVolumeTimer.destValue = value
        changeVolumeTimer.restart()
    }
    Timer {
        id: changeVolumeTimer
        repeat: true
        interval: 100
        //property bool forward: true
        property double destValue: 1.0
        onTriggered: {

            if (fullscreenView.volumeValue < destValue)
                fullscreenView.volumeValue = fullscreenView.volumeValue + 0.01
            else if (destValue == 0.0)
                fullscreenView.volumeValue = fullscreenView.volumeValue - fullscreenView.volumeValue;
            else if (fullscreenView.volumeValue > destValue) {
                fullscreenView.volumeValue = fullscreenView.volumeValue - Math.min(0.01, fullscreenView.volumeValue)
            }

            if (fullscreenView2.volumeValue < destValue)
                fullscreenView2.volumeValue = fullscreenView2.volumeValue + 0.01
            else if (destValue == 0.0)
                fullscreenView2.volumeValue = fullscreenView2.volumeValue - fullscreenView2.volumeValue;
            else if (fullscreenView2.volumeValue > destValue) {
                fullscreenView2.volumeValue = fullscreenView2.volumeValue - Math.min(0.01, fullscreenView2.volumeValue)
            }

            if (fullscreenView3.volumeValue < destValue)
                fullscreenView3.volumeValue = fullscreenView3.volumeValue + 0.01
            else if (destValue == 0.0)
                fullscreenView3.volumeValue = fullscreenView3.volumeValue - fullscreenView3.volumeValue;
            else if (fullscreenView3.volumeValue > destValue) {
                fullscreenView3.volumeValue = fullscreenView3.volumeValue - Math.min(0.01, fullscreenView3.volumeValue)
            }

            if (fullscreenView4.volumeValue < destValue)
                fullscreenView4.volumeValue = fullscreenView4.volumeValue + 0.01
            else if (destValue == 0.0)
                fullscreenView4.volumeValue = fullscreenView4.volumeValue - fullscreenView4.volumeValue;
            else if (fullscreenView4.volumeValue > destValue) {
                fullscreenView4.volumeValue = fullscreenView4.volumeValue - Math.min(0.01, fullscreenView4.volumeValue)
            }
        }
    }

    function setContentPosition(cLeft, cTop, cWidth, cHeight, wLeft, wTop, wWidth, wHeight)
    {
        contentWidthPercent = cWidth
        contentHeightPercent = cHeight
        contentLeftPercent = cLeft
        contentTopPercent = cTop

        widgetWidthPercent = wWidth
        widgetHeightPercent = wHeight
        widgetLeftPercent = wLeft
        widgetTopPercent = wTop
    }

    function setLicenseText(eula, privacyPolicy, openSource, legal)
    {
        console.log("E.l=" + eula.length)
        systemEULAText = eula
        systemPrivacyPolicyText = privacyPolicy
        systemOpensourceText = openSource
        systemLegalText = legal
    }

    function setDeviceInfo(name, conn)
    {
        systemDeviceName = name
        systemConnectionName = conn
    }

    function setTheme(brandBGLogo, brandLogo, brandBGColor, brandFGColor, brandGRColor, tileMode, showTeleDSLogo)
    {
        brand_backgroundColor = brandBGColor
        brand_foregroundColor = brandFGColor
        brand_borderGrayColor = brandGRColor

        brand_backgroundLogoHorizonal = brandBGLogo
        brand_backgroundLogoVertical = brandBGLogo
        brand_logoImage = brandLogo

        if (tileMode)
            bgLogoBlock.setTileMode()
        else
            bgLogoBlock.setDefaultMode()

        if (showTeleDSLogo)
            teledsLogo.opacity = 1.0
        else
            teledsLogo.opacity = 0.0
    }
    function setMenuTheme(brandBGLogo, brandLogo, brandBGColor, brandFGColor, tileMode, showTeleDSLogo)
    {
        brand_menu_backgroundColor = brandBGColor
        brand_menu_foregroundColor = brandFGColor

        brand_menu_background = brandBGLogo
        brand_menu_logo = brandLogo

        if (tileMode)
            menu.setTileMode()
        else
            menu.setDefaultMode()
        menu.showTeleDSLogo = showTeleDSLogo
    }

    function restoreDefaultTheme()
    {
     /*   brand_backgroundColor = brand_default_backgroundColor
        brand_foregroundColor = brand_default_foregroundColor
        brand_borderGrayColor = brand_default_borderGrayColor

        brand_backgroundLogoHorizonal = brand_default_backgroundLogoHorizonal
        brand_backgroundLogoVertical = brand_default_backgroundLogoVertical
        brand_logoImage = brand_default_logoImage

        brand_menu_backgroundColor = brand_default_menu_backgroundColor
        brand_menu_foregroundColor = brand_default_menu_foregroundColor
        brand_menu_background = brand_default_menu_background
        brand_menu_logo = brand_default_menu_logo

        teledsLogo.opacity = 1.0
        bgLogoBlock.setDefaultMode()
        menu.setDefaultMode()
        menu.showTeleDSLogo = true*/
    }

    function stopPlayer()
    {
        fullscreenView.stop()
        fullscreenView2.stop()
        fullscreenView3.stop()
        fullscreenView4.stop()
        currentType = "null"
    }

    function downloadComplete()
    {
        console.debug("download complete. Hiding Progress Bars, Showing Video Player");

        filesProgressBar.visible = false
        downloadProgressBar.visible = false
        playerName.visible = false
        bgLogoBlock.visible = false
        logoColumn.visible = false
    }

    function setDownloadProgressSimple(value)
    {
        logoDownloadProgressBar.value = value
        backDownloadingProgressBar.value = value
    }

    function setBackDownloadProgressBarVisible(value)
    {
        backDownloadingProgressBar.visible = value
    }

    function setPlayerName(playerId){
        playerName.text = playerId
    }

    function setTotalProgress(p, name){
        filesProgressBar.value = p
        playerName.text = name
    }

    function setProgress(p){
        downloadProgressBar.value = p
    }

    function showVideo(isVisible){
        logoColumn.visible = false
    }

    function setBrightness(value){
        videoOutBrightness = value
        fullscreenView.brightness = value
    }

    function setVersion(version, build){
        systemVersion = version
        systemVersionBuild = build
    }

    function showPassword(is_visible, pass)
    {
        if (is_visible === 1)
            passText.visible = true
        else
            passText.visible = false
        passTextItem.text = pass
    }

    function setNoItemsLogo(link){

        logoColumn.visible = true
        titleText.text = brand_nothingToPlayText
        progressText.text = "Go to <a href=\"" + link + "\">" + brand_nothingToPlayLinkText +"</a></html>"

        logoDownloadProgressBar.visible = false
        isPlayerCodeVisibleState = false
        waitingBlock.visible = false

        bgLogoBlock.visible = true

        stopEverything()
    }

    function stopEverything()
    {
        fullscreenView.visible = false
        fullscreenView.stop()
        fullscreenView2.visible = false
        fullscreenView2.stop()
        fullscreenView3.visible = false
        fullscreenView3.stop()
        fullscreenView4.visible = false
        fullscreenView4.stop()
    }

    function setUpdateState(tx)
    {
        logoColumn.visible = true
        progressText.text = ""
        titleText.text = ""
        logoDownloadProgressBar.visible = false
        isPlayerCodeVisibleState = false
        waitingBlock.visible = false
        bgLogoBlock.visible = true
        fullscreenView.visible = false
        brRect.visible = false
    }

    function setDownloadLogo(){
        logoColumn.visible = true
        titleText.text = brand_downloadingText
        progressText.text = brand_pleaseWaitText
        logoDownloadProgressBar.visible = true
        isPlayerCodeVisibleState = false
        waitingBlock.visible = false
    }

    function setNeedActivationLogo(link, playerID, showCode){
        logoColumn.visible = true
        titleText.text = brand_setupText
        progressText.text = "<a href=\"" + link + "\">" + link + "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDText.text = playerID

        isPlayerCodeVisibleState = true
        //playerIDItem.visible = true
        //playerIDText.color = brand_backgroundColor

        bgLogoBlock.visible = true
        if (showCode === true)
        {
            isPlayerCodeVisibleMode = true
        }
    }

    function setPlayerIdHiddenMode(isVisible)
    {
        isPlayerCodeVisibleMode = isVisible
    }

    function getWidthPercentSize(value)
    {
        return value * item.width
    }

    function getHeightPercentSize(value)
    {
        return value * item.heightP
    }

    function getPointSize(text){
        return 28
        /*
                if (text.length > 40)
            decreasingTextValue = text.length-40
        else
            decreasingTextValue = 0
        if (item.widthP > item.heightP)
            return 32 - decreasingTextValue
        else
        {
            if (decreasingTextValue)
                decreasingTextValue += 2
            testText.text = text
            if (testText.width < item.widthP)
                decreasingTextValue -= 4
            return 32 - decreasingTextValue
        }*/
    }

    function getTopValue(h)
    {
        var result = h * 0.248148148 //- Math.max(waitingRefreshInText.y + h * 0.248148148 + waitingRefreshInText.height - h,0)*1.5
        if (result < 0)
            return h * 0.248148148 * aspect
        else
            return result
    }

    function skipCurrentItem(){
        fullscreenView.skipCurrentItem()
    }

    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            console.log("Coordinate:", coord.longitude, coord.latitude);
            gpsChanged(coord.latitude, coord.longitude)
        }
    }
    Timer {
        id: dialogCloseTimer
        repeat: false
        interval: 10000
        onTriggered: {
            dialogAndroid.close()
            setRestoreModeTrue();
            item.focus = true
        }
    }

    FontLoader {
        id: ubuntuFont
        source: "ubuntu-m.ttf"
    }

    FontLoader {
        id: ptSansFont
        source: "pt_sans.ttf"
    }

    Rectangle {
        id: overlayBgRect
        color: brand_backgroundColor
        width: parent.width
        height: parent.height
    }

    Item {
        id: bgLogoBlock
        width: item.width
        height: item.height
        Item{
            id: defaultLogoBlock
            width: parent.width
            height: parent.height
            Image {
                id: bgLogoImage
                source: brand_backgroundLogoHorizonal
                sourceSize.width: parent.width
                sourceSize.height: parent.height
                width: parent.width
                height: parent.height
            }

            Image {
                id: bgLogoImageV
                visible: item.width < item.height
                source: brand_backgroundLogoVertical
                sourceSize.width: item.width
                sourceSize.height: item.height
                width: parent.width
                height: parent.height
            }
        }

        Image {
            id: bgLogoImageTiling
            source: brand_backgroundLogoHorizonal
            sourceSize.width: parent.width
            sourceSize.height: parent.height
            width: parent.width
            height: parent.height
            fillMode: Image.Tile
            horizontalAlignment: Image.AlignLeft
            verticalAlignment: Image.AlignTop
            visible: false
        }
        function setDefaultMode()
        {
            defaultLogoBlock.visible = true
            bgLogoImageTiling.visible = false
        }
        function setTileMode()
        {
            defaultLogoBlock.visible = false
            bgLogoImageTiling.visible = true
        }
    }

    Item
    {
        id: logoColumn
        width: parent.width
        y: getTopValue(parent.height)
        Text{
            id: testText
            font.family: ubuntuFont.name
            font.pointSize: 26
            visible: false
            text: "kek"
        }

        Image {
            id: teledsLogoImg
            source: brand_logoImage
            sourceSize.width: 195.0 * aspect
            sourceSize.height: 156.0 * aspect
            x: parent.width/2 - width/2
        }

        Image {
            id: teledsLogo
            y: teledsLogoImg.y + teledsLogoImg.height + (22.0 * aspect)
            x: parent.width/2 - width/2
            source: brand_logoLabel

            scale: aspect
        }

        Rectangle {
            id: brRect
            width: 124 * aspect
            height: 8 * aspect
            y: teledsLogo.y + teledsLogo.height + (75.0 * aspect)
            x: parent.width/2 - width/2
            color: brand_foregroundColor
        }

        Text {
            id: titleText
            font.family: ubuntuFont.name
            font.pointSize: getPointSize(text)
            text: brand_initialization
            y: brRect.y + brRect.height + (75.0 * aspect)
            color: "white"
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        Text {
            id: progressText
            font.family: ubuntuFont.name
            font.pointSize: getPointSize(text)
            text: "..."
            x: parent.width/2 - width/2
            y: titleText.y + titleText.height + 18 * aspect
            color: brand_foregroundColor
            linkColor: brand_foregroundColor
            onLinkActivated: Qt.openUrlExternally(link)
        }

        ProgressBar{
            id: logoDownloadProgressBar
            visible: false
            value: 0.0
            y: progressText.y + progressText.height + 110 * aspect
            width: parent.width
            style: ProgressBarStyle {
                    background: Rectangle {
                        color: "transparent"
                        border.color: "black"
                        border.width: 1
                        implicitHeight: 68 * aspect
                    }
                    progress: Rectangle {
                        color: brand_foregroundColor
                        border.color: brand_foregroundColor
                    }
                }
        }

        Item{
            id: playerIDItem
            width: parent.width
            height: parent.height
            visible: isPlayerCodeVisibleMode && isPlayerCodeVisibleState
            Rectangle {
                id: playerIDRect
                color: brand_foregroundColor
                Text {
                    id: playerIDText
                    text: ""
                    color: brand_backgroundColor
                    font.family: ubuntuFont.name
                    font.pointSize: 38
                }
                width: childrenRect.width
                height: childrenRect.height
                y: progressText.y + progressText.height + 55 * aspect
                x: parent.width/2 - width/2
            }
        }

        Item{
            id: waitingBlock
            width: parent.width
            visible: false
            Text {
                y: playerIDRect.y + playerIDRect.height + 55 * aspect
                x: item.width/2 - width/2
                id: waitingText

                font.family: ubuntuFont.name
                font.pointSize: 20
                text: brand_waitingForActivationText
                color: brand_foregroundColor
            }
            Text {
                y: waitingText.y + waitingText.height
                x: item.width/2 - width/2
                id: waitingRefreshInText
                font.family: ubuntuFont.name
                font.pointSize: 20
                text: ""
                color: brand_foregroundColor
            }
        }

        Item {
            id: spacemediaContactsBlock
            visible: false

            Image {
                id: spacemediaContactsPhoneImage
                source: "sm_phone.png"
                width: getWidthPercentSize(0.010416667)
                height: getHeightPercentSize(0.031481481)
                x: getWidthPercentSize(0.416666667)
                y: getHeightPercentSize(0.848148148)
            }
            Text {
                id: spacemediaContactsPhoneText
                text: "8 800 555 32 06"
                font.family: ptSansFont.name
                x: spacemediaContactsPhoneImage.x + spacemediaContactsPhoneImage.width + spacemediaContactsPhoneImage.width/2
                y: spacemediaContactsPhoneImage.y - spacemediaContactsPhoneText.height / 4
                font.pixelSize: getHeightPercentSize(0.031481481 * 1.25)
            }

            Image {
                id: spacemediaContactsEmailImage
                source: "sm_mail.png"
                width: getWidthPercentSize(0.016666667)
                height: getHeightPercentSize(0.02037037)
                x: getWidthPercentSize(0.396354167)
                y: getHeightPercentSize(0.912962963)
            }

            Text {
                id: spacemediaContactsEmailText
                text: "info@spacemedia.ru"
                font.family: ptSansFont.name
                x: spacemediaContactsEmailImage.x + spacemediaContactsEmailImage.width + spacemediaContactsEmailImage.width/2
                y: spacemediaContactsEmailImage.y - spacemediaContactsEmailText.height * 0.38
                font.pixelSize: getHeightPercentSize(0.031481481 * 1.25)
            }

        }
    }

    Column
    {
        visible: false
        width: parent.width
        Label{
            id: playerName
            width: parent.width
            font.pointSize: 48
        }

        ProgressBar{
            id: filesProgressBar
            value: 0.0
            width: parent.width
            style: ProgressBarStyle {
                    background: Rectangle {
                        radius: 2
                        color: "lightgray"
                        border.color: "gray"
                        border.width: 1
                        implicitHeight: 24
                    }
                    progress: Rectangle {
                        color: "lightsteelblue"
                        border.color: "steelblue"
                    }
                }
        }

        ProgressBar{
            id: downloadProgressBar
            value: 0.0
            width: parent.width
            style: ProgressBarStyle {
                       background: Rectangle {
                           radius: 2
                           color: "lightgray"
                           border.color: "gray"
                           border.width: 1
                           implicitHeight: 24
                       }
                       progress: Rectangle {
                           color: "lightsteelblue"
                           border.color: "steelblue"
                       }
                   }
        }
    }

    PlayerView {
        id: fullscreenView
        visible: true
        delay: playlistDelay
        onAskNext: {
            console.log("askNext?")
            nextItem(areaId)
        }
        Keys.onReleased:{
            console.log("KEy pressed")
            if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                if (menu.visible == false)
                {
                    setRestoreModeFalse()
                    menu.visible = true
                }
                else
                {
                    menu.visible = false
                    setRestoreModeTrue()
                }
                event.accepted = true
            }
        }
        onOnStop: {
            console.log("FULLSCREENVIEW::onstop")
            nextItem("")
        }
        z: 0
    }

    PlayerView {
        id: fullscreenView2
        visible: false
        delay: playlistDelay
        onAskNext: {
            console.log("askNext?")
            nextItem(areaId)
        }
        Keys.onReleased:{
            console.log("KEy pressed")
            if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                if (menu.visible == false)
                {
                    setRestoreModeFalse()
                    menu.visible = true
                }
                else
                {
                    menu.visible = false
                    setRestoreModeTrue()
                }
                event.accepted = true
            }
        }
        onOnStop: {
            console.log("FULLSCREENVIEW::onstop")
            nextItem("")
        }
        z: 1
    }

    PlayerView {
        id: fullscreenView3
        visible: false
        delay: playlistDelay
        onAskNext: {
            console.log("askNext?")
            nextItem(areaId)
        }
        Keys.onReleased:{
            console.log("KEy pressed")
            if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                if (menu.visible == false)
                {
                    setRestoreModeFalse()
                    menu.visible = true
                }
                else
                {
                    menu.visible = false
                    setRestoreModeTrue()
                }
                event.accepted = true
            }
        }
        onOnStop: {
            console.log("FULLSCREENVIEW::onstop")
            nextItem("")
        }
        z: 2
    }
    PlayerView {
        id: fullscreenView4
        visible: false
        delay: playlistDelay
        onAskNext: {
            console.log("askNext?")
            nextItem(areaId)
        }
        Keys.onReleased:{
            console.log("KEy pressed")
            if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                if (menu.visible == false)
                {
                    setRestoreModeFalse()
                    menu.visible = true
                }
                else
                {
                    menu.visible = false
                    setRestoreModeTrue()
                }
                event.accepted = true
            }
        }

        onOnStop: {
            console.log("FULLSCREENVIEW::onstop")
            nextItem("")
        }
        z: 3
    }


    //android:excludeFromRecents="true"



    SpaceMediaMenu{
        id: menu
        width: item.width
        height: item.height
        visible: false
        z: 10
    }


    TeleDSMenu {
        id: oldMenu
        color1: brand_menu_backgroundColor
        color2: brand_menu_foregroundColor
        logo: brand_menu_logo
        screenWidth: item.width
        screenHeight: item.height
        visible: false

        eula: systemEULAText
        privacyPolicy: systemPrivacyPolicyText
        openSource: systemOpensourceText
        legal: systemLegalText

        deviceName: systemDeviceName
        deviceConnection: systemConnectionName
        deviceVersion: systemVersion

        onPlayerClicked: {

            hideTeleDSMenu()
        }

        onExitClicked: {
            dialogCloseTimer.start()
            dialogAndroid.open()
            dialogButtonOk.focus = true
            setRestoreModeFalse()
            if (currentType == "browser")
            {
                bgLogoBlock.visible = true
                androidBrowser.visible = false
                videoPlayer.visible = false
            }
            if (displayMode == "split")
                sideBrowser.visible = false
            console.log("back key pressed: main")
        }
        z: 10
    }

    Item {
        id: passText
        width: parent.width
        height: parent.height
        visible: false
        Rectangle{
            id: passTextRect
            x: parent.width/2 - width/2
            y: parent.height - height
            height: passTextItem.height + 5
            width: passTextItem.width + 10
            color: "#333e47"
            opacity: 0.5
        }

        Text {
            id: passTextItem
            anchors.centerIn: passTextRect
            text: ""
            color: "#FFFFFF"
            font.pointSize: 12
        }
        z: 10
    }

    Rectangle{
        id: prepareLoadRect
        color: brand_blackColor
        width: parent.width
        height: parent.height
    }

    Item {
        id: crcTextBlock
        width: parent.width
        height: parent.height
        Rectangle{
            id: bottomCRCRect
            x: parent.width - width
            y: parent.height - height
            height: crcText.height + 5
            width: crcText.width + 10
            color: "#00333e47"
        }
        Text {
            id: crcText
            anchors.centerIn: bottomCRCRect
            text: "________"
            color: "#FFFFFF"
            font.pointSize: 7
            opacity: 0.75
        }
        z: 10
    }

    Text {
        id: versionText
        x: 0
        y: parent.height - height
        text: "build: " + systemVersionBuild
        color: "#FFFFFF"
        font.pointSize: 7
        opacity: 0.5
        z: 10
    }

    ProgressBar{
        id: backDownloadingProgressBar
        visible: false
        value: 0.0
        x: versionText.x + versionText.width + 4
        y: parent.height - height
        width: 100
        opacity: 0.4
        style: ProgressBarStyle {

                background: Rectangle {
                    color: "transparent"
                    border.color: "black"
                    border.width: 1
                    implicitHeight: versionText.height
                }

                progress: Rectangle {
                    color: brand_foregroundColor
                    border.color: brand_foregroundColor
                }
            }
        z: 10
    }

    Dialog {
        id: dialogAndroid

        contentItem: Rectangle {
            width: 600
            height: 500
            color: "#333e47"

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: dividerHorizontal.top
                color: "#333e47"

                Label {
                    id: textLabel
                    text: qsTr("Are you sure?")
                    color: brand_foregroundColor
                    anchors.centerIn: parent
                }
            }

            Rectangle {
                id: dividerHorizontal
                color: brand_borderGrayColor
                height: 2
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: row.top
            }

            Row {
                id: row
                height: 100
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right

                Button {
                    id: dialogButtonCancel
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.width / 2 - 1

                    style: ButtonStyle {
                        background: Rectangle {
                            color: control.pressed ? brand_borderGrayColor : "#333e47"
                            border.width: 0
                        }

                        label: Text {
                            text: qsTr("Cancel")
                            color: brand_foregroundColor
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }

                    onClicked: {
                        dialogAndroid.close()
                       // if (displayMode == "split")
                       //     sideBrowser.visible = true
                        if (currentType == "browser")
                        {
                            androidBrowser.visible = true
                            videoPlayer.visible = true
                            bgLogoBlock.visible = false
                        }
                        else if (currentType == "videoPlayer")
                        {
                            bgLogoBlock.visible = false
                        }
                        item.focus = true
                        menu.reinit()
                    }
                }

                Rectangle {
                    id: dividerVertical
                    width: 2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    color: brand_borderGrayColor
                }

                Button {
                    id: dialogButtonOk
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.width / 2 - 1

                    style: ButtonStyle {
                        background: Rectangle {
                            color: control.pressed ? brand_borderGrayColor : "#333e47"
                            border.width: 0
                        }

                        label: Text {
                            text: qsTr("Ok")
                            color: brand_foregroundColor
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                    Keys.onReleased: {
                        console.log("DIALOG ON RELEASED!!!")
                        if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                            dialogAndroid.close()
                            setRestoreModeTrue();
                            item.focus = true
                            event.accepted = true
                        }
                    }
                    onClicked: Qt.quit()
                }
            }
        }
    }

    Rectangle {
        id: hideRect
        anchors.fill: parent
        color: "black"
        visible: false
    }


    Keys.onReturnPressed:{
        console.log("keypressed")
    }


    function showTeleDSMenu()
    {
        setRestoreModeFalse()
        fullscreenView.visible = false
        fullscreenView2.visible = false
        fullscreenView3.visible = false
        fullscreenView4.visible = false

        fullscreenView.hideWebViews = true
        fullscreenView2.hideWebViews = true
        fullscreenView3.hideWebViews = true
        fullscreenView4.hideWebViews = true

        oldMenu.restore()
    }

    function hideTeleDSMenu()
    {
        oldMenu.visible = false
        fullscreenView.visible = fullscreenView.isActive
        fullscreenView2.visible = fullscreenView2.isActive
        fullscreenView3.visible = fullscreenView3.isActive
        fullscreenView4.visible = fullscreenView4.isActive

        fullscreenView.hideWebViews = false
        fullscreenView2.hideWebViews = false
        fullscreenView3.hideWebViews = false
        fullscreenView4.hideWebViews = false

        setRestoreModeTrue()

        fullscreenView.restore()
        fullscreenView2.restore()
        fullscreenView3.restore()
        fullscreenView4.restore()
    }

    Keys.onReleased:{
        console.log("Key pressed")
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {

            console.log("KEY BACK!!!!")
            //event.accepted = true
            if (oldMenu.visible == false)
            {
                showTeleDSMenu()
            }
            else
            {
                hideTeleDSMenu()
            }
            event.accepted = true
        }
    }
}

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.2
import QtWebKit 3.0


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
    property string displayMode: "fullscreen"

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

    function prepareArea(name, campaignWidth, campaignHeight, x, y, w, h, _rotation)
    {
        console.log("prepareArea " + campaignWidth + " " + campaignHeight + " " + w + " " + h)
        fullscreenView.areaID = name
        fullscreenView.x = width * x / campaignWidth;
        fullscreenView.y = height * y / campaignHeight;
        fullscreenView.width = width * w / campaignWidth;
        fullscreenView.height = height * h / campaignHeight;
        fullscreenView.setRotation(_rotation)

      /*  fullscreenView.areaID = name
        fullscreenView.x = 0
        fullscreenView.y = 0
        fullscreenView.height = item.height
        fullscreenView.width = item.width*/
       // moveTimer.start()
    }
    Timer {
        id: moveTimer
        repeat: true
        interval: 50
        onTriggered: {
            fullscreenView.x = fullscreenView.x + 1
        }
    }

    function playNextItem(areaId, source, type, duration, skipTime, fillMode)
    {
        if (fullscreenView.visible == false)
            fullscreenView.visible = true
        fullscreenView.play(source, duration, type, skipTime, fillMode)
    }

    function setPlayerVolume(value)
    {
        //fullscreenView.volumeValue = value
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
        brand_backgroundColor = brand_default_backgroundColor
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
        menu.showTeleDSLogo = true
    }

    function stopPlayer()
    {
        fullscreenView.stop()
        currentType = "null"
    }

    function downloadComplete(){
        console.debug("download complete. Hiding Progress Bars, Showing Video Player");
        filesProgressBar.visible = false
        downloadProgressBar.visible = false
        playerName.visible = false
        bgLogoBlock.visible = false
        logoColumn.visible = false
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
    function setVersion(version){
        systemVersion = version
    }


    function setNoItemsLogo(link){
        logoColumn.visible = true
        titleText.text = brand_nothingToPlayText
        progressText.text = "Go to <a href=\"" + link + "\">" + brand_nothingToPlayLinkText +"</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDItem.visible = false
        waitingBlock.visible = false

        bgLogoBlock.visible = true
        fullscreenView.visible = false
        fullscreenView.stop()
    }

    function setUpdateState(tx)
    {
        logoColumn.visible = true
        titleText.text = "Updating..."
        progressText.text = "v. " + tx
        logoDownloadProgressBar.visible = false
        playerIDItem.visible = false
        waitingBlock.visible = false
        bgLogoBlock.visible = true
        fullscreenView.visible = false
    }

    function setDownloadLogo(){
        logoColumn.visible = true
        titleText.text = brand_downloadingText
        progressText.text = brand_pleaseWaitText
        logoDownloadProgressBar.visible = true
        playerIDItem.visible = false
        waitingBlock.visible = false
    }


    function setNeedActivationLogo(link, playerID, updateDelay){
        logoColumn.visible = true
        titleText.text = brand_setupText
        progressText.text = "<a href=\"" + link + "\">" + link + "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDText.text = playerID

        playerIDItem.visible = true
       // waitingBlock.visible = true
        playerIDText.color = brand_backgroundColor

        bgLogoBlock.visible = true
    }
    function getPointSize(text){
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
        }
    }
    function getPointSizeSecond(text){
        if (item.widthP > item.heightP)
            return 20
        else
            return 18
    }
    function getRefreshButtonPositionY(w, h)
    {
        if (w > h)
        {
            return playerIDRect.y
        }
        else
        {
            return playerIDRect.y + playerIDRect.height + 8
        }
    }
    function setDownloadProgressSimple(value)
    {
        logoDownloadProgressBar.value = value
    }

    function getTopValue(h)
    {
        var result = h * 0.248148148 - Math.max(waitingRefreshInText.y + h * 0.248148148 + waitingRefreshInText.height - h,0)*1.5
        if (result < 0)
            return h * 0.248148148 * aspect
        else
            return result
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
            visible: false
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
    }

    //

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
    Text {
        id: versionText
        x: parent.width - width
        y: parent.height - height
        text: systemVersion
    }

    PlayerView {
        id: fullscreenView
        visible: true
        onAskNext: {
            nextItem(areaId)
        }
    }


    //code for testing rotations!
   /* Rectangle {
        id: normalRect
        x: 100
        y: 100
        width: item.width/2
        height: item.height/2
        color: "#ff0000"
        opacity: 0.7
    }*/
/*
   Rectangle {
        id: rotatedRect
        x: 100 + (item.width/2 - item.height/2)/2          //parent.x + (item.width/2 - item.height/2)/2
        y: 100 - (item.width/2 - item.height/2)/2
        width: item.height/2
        height: item.width/2
        color: "#00FFFF"
        opacity: 0.3
        rotation: 90
        WebView {
            url: "https://youtube.com"
            width: parent.width
            height: parent.height
        }
    }
    WebView{
        id: ttw
        url: "https://yandex.ru"
        x: 400 + (item.width/2 - item.height/2)/2
        y: 300 - (item.width/2 - item.height/2)/2
        width: item.height/2
        height: item.width/2
        opacity: 0.5
        rotation: -60
    }*/


    //android:excludeFromRecents="true"

    TeleDSMenu {
        id: menu
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
            menu.visible = false
            setRestoreModeTrue()
            if (displayMode == "split")
                sideBrowser.visible = true
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
                            if (displayMode == "split")
                                sideBrowser.visible = true
                            if (currentType === "browser")
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
                            event.accepted = true
                        }
                    }
                    onClicked: Qt.quit()
                }
            }
        }
    }

    function setMenuView(secondScreen){
        if (secondScreen === true)
        {
            if (androidBrowser.visible === true)
                androidBrowser.visible = false
            if (sideBrowser.visible === true)
                sideBrowser.visible = false
        }
        else {
            if (sideBrowser.visible === false && displayMode == "split")
                sideBrowser.visible = true
        }
    }

    Keys.onReleased:{
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
            if (menu.visible == false)
            {
                setMenuView(true)
                setRestoreModeFalse()
                menu.restore()
            }
            else
            {
                setMenuView(false)
                menu.visible = false
                setRestoreModeTrue()
            }
            event.accepted = true
        }
    }
}

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.2


Item {
    id: item
    property bool invokeNext:true
    property double videoOutBrightness: 1.0
    property double hValue: Math.min(height, width)
    property double aspect: Math.min(hValue / 1080.0, 1.0)
    property int heightP: height
    property int widthP: width
    property int decreasingTextValue: 0


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
    property color brand_menu_backgroundColor:      "#333e47"
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

    property color brand_default_menu_backgroundColor:      "#333e47"
    property color brand_default_menu_foregroundColor:      "#00cdc1"
    property url brand_default_menu_background:             "menu_background_horizontal.png"
    property url brand_default_menu_logo:                   "logo.svg"

    //properties for seamless video player
    property bool preloader: false
    property bool useSecondPlayer: false

    //properties for cross content managing
    property string currentType: "null"

    property string systemDeviceName: ""
    property string systemConnectionName: ""
    property string systemEULAText: ""
    property string systemPrivacyPolicyText: ""
    property string systemOpensourceText: ""
    property string systemLegalText: ""
    property string systemVersion: ""

    signal nextItem()
    signal refreshId()
    signal gpsChanged(double lat, double lgt)
    signal setRestoreModeTrue()
    signal setRestoreModeFalse()

    focus: true

    onHeightChanged: {
        heightP = height
        widthP = width
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
        videoPlayer.stopPlayer();
        videoPlayer.visible = false;
        if (androidBrowser.visible == true){
            androidBrowser.visible = false
            androidBrowser.stopBrowser()
        }
        currentType = null
    }

    function enablePreloading(filename)
    {
        console.log("Preloading: MP2->: " +filename)
        videoPlayer.playItem(filename)
    }

    function playFile(filename){
        console.debug("playfile is called " + filename)
        videoPlayer.playItem(filename)
    }

    function playFileAdvanced(filename, type, build, length, skip)
    {
        console.debug("playfile is called " + filename)
        console.log("Platform: " + Qt.platform.os)
        if (type === "video")
        {
            //browser just started to play, nextItem returned video - we need to preload video
            //on browser show end -> we should play this video
            //
            if (currentType === "browser")
            {
                console.log("playFileAdv::video/browser -> preloading video")

                videoPlayer.preloadItem(filename, type, length, skip)
                androidBrowser.setNextItem(filename, length, "video", skip)
            }
            else
            {
                console.log("playFileAdv::video/video -> loading next video | assuming its preloaded")
                videoPlayer.playItemAdv(filename, length, skip)
                currentType = "videoPlayer"
            }
            return
        }
        else if (type === "audio")
        {
            if (currentType === "browser")
            {
                console.log("playFileAdv::audio/browser -> preloading audio")
                videoPlayer.preloadItem(filename, "audio", length, skip)
                androidBrowser.setNextItem(filename, length, "audio", skip)
            }
            else
            {
                console.log("playFileAdv::audio/multi -> loading next audio | assuming its preloaded")
                videoPlayer.playAudioItemAdv(filename, length, skip)
                currentType = "videoPlayer"
            }
        }
        else if (type === "html5_online")
        {
            if (currentType === "null")
            {
                console.log("playFileAdv::browser/null -> loading and showing browser")
                androidBrowser.load(filename)
                androidBrowser.setShowTime(length)
                showBrowserTimer.browserVisible = true
                showBrowserTimer.start()
                //androidBrowser.visible = true
                androidBrowser.startShow()
                currentType = "browser"
                nextItem()
            }
            else if (currentType === "videoPlayer")
            {
                //prepare for Browser
                console.log("playfileAdvanced::html5")
                videoPlayer.prepareStop = true
                currentType = "browser"
                androidBrowser.load(filename)
                androidBrowser.setShowTime(length)
            }
            else if (currentType === "browser"){
                console.log("playFileAdv::browser/browser -> filling nextItem with new browser item")
                androidBrowser.setNextItem(filename, length, "browser", 0)
            }
        }
    }

    function downloadComplete(){
        console.debug("download complete. Hiding Progress Bars, Showing Video Player");
        filesProgressBar.visible = false
        downloadProgressBar.visible = false
        videoPlayer.visible = true
        playerName.visible = false
        bgLogoBlock.visible = false
        logoColumn.visible = false
        videoPlayer.opacity = videoOutBrightness
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
        if (videoPlayer.isVideoPlaying){
            videoPlayer.visible = isVisible
            logoColumn.visible = false
            if (!isVisible)
            {
                overlayBgRect.color = brand_backgroundColor
            }
            else if (videoOutBrightness > 1.0)
            {
                overlayBgRect.color = brand_whiteColor
            }
            else if (videoOutBrightness < 0.99){
                overlayBgRect.color = brand_blackColor
            }
            else{
                overlayBgRect.color = brand_blackColor
            }
        }
    }

    function setBrightness(value){
        videoOutBrightness = value
        if (value > 1.0) {
            overlayBgRect.color = brand_whiteColor
            videoPlayer.opacity = 2.0 - value
        }
        else {
            overlayBgRect.color = brand_blackColor
            videoPlayer.opacity = value
        }
    }
    function setVersion(version){
        systemVersion = version
    }

    function setNoItemsLogo(link){
        logoColumn.visible = true
        videoPlayer.opacity = 0
        titleText.text = brand_nothingToPlayText
        progressText.text = "Go to <a href=\"" + link + "\">" + brand_nothingToPlayLinkText +"</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDItem.visible = false
        waitingBlock.visible = false

        bgLogoBlock.visible = true
    }
    function setDownloadLogo(){
        logoColumn.visible = true
        videoPlayer.opacity = 0
        titleText.text = brand_downloadingText
        progressText.text = brand_pleaseWaitText
        logoDownloadProgressBar.visible = true
        playerIDItem.visible = false
        waitingBlock.visible = false
    }
    function setNeedActivationLogo(link, playerID, updateDelay){
        logoColumn.visible = true
        videoPlayer.opacity = 0
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
    Timer{
        property var object: ({})
        id: hideVideoDelayTimer
        interval: 50
        repeat: false
        onTriggered: {
            object.visible = false
        }
    }
    Timer {
        property int updateDelay: 10
        id: refreshTimeTimer
        interval: 1000
        repeat: true
        running: true
        onTriggered:{
            updateDelay = updateDelay-1;
            if (updateDelay == -1)
                updateDelay = 10;
            if (updateDelay != 0)
                waitingRefreshInText.text = "Refresh in " + updateDelay + "s"
            else
                waitingRefreshInText.text = "Refreshing..."
        }
    }
    Timer {
        id: dialogCloseTimer
        repeat: false
        interval: 10000
        onTriggered: {
            dialogAndroid.close()
            setRestoreModeTrue();
            if (currentType == "browser")
            {
                androidBrowser.visible = true
                videoPlayer.visible = true
            }
            else if (currentType == "videoPlayer")
            {
                videoPlayer.visible = true
            }
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
    /*
    Image {
        id: audioFG
        width: item.width; height: item.height
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop
        source: "audio.svg"
    }
*/

    Timer {
        id: showBrowserTimer
        interval: 200
        repeat: false
        property bool browserVisible: true
        onTriggered: {
            androidBrowser.visible = browserVisible
        }
    }

    //content types
    //video player
    VideoPlayer {
        id: videoPlayer
        onNext: {
            console.log("video player:invoking next item")
            nextItem()
        }
        onVideoPlayed: {
            console.log("video player:video played")
            videoPlayer.visible = true
            audioIcon.visible = false
        }
        onAudioPlayed: {
            console.log("videoplayer:audio played")
            videoPlayer.visible = false
            audioIcon.visible = true
        }
        onVideoStopped: {
            console.log("video player:video stopped")


            videoPlayer.stopPlayer()

            showBrowserTimer.browserVisible = true
            showBrowserTimer.start()
            androidBrowser.startShow()
            audioIcon.visible = false

            nextItem()
        }
    }
    //image for audio playback
    Image {
        id: audioIcon
        visible: false
        source: brand_audioIcon
        sourceSize.width: hValue/2
        sourceSize.height: hValue/2
        width: hValue/2
        height: hValue/2
        x: parent.width/2 - width/2
        y: parent.height/2 - height/2
    }


    AndroidBrowser{
        id: androidBrowser
        visible: false
        property string nextItemUrl: ""
        property int nextItemTime: 0
        property string nextItemType: ""
        property int nextItemSkipTime: 0

        function setNextItem(item, length, type, skip)
        {
            nextItemUrl = item
            nextItemTime = length
            nextItemType = type
            nextItemSkipTime = skip
        }

        onLoadFinished: {
        }
        onEndShow: {
            if (nextItemUrl == ""){
            console.log("browser end show, no next Item!")
            }
            else
            {
                if (nextItemType === "browser")
                {
                    console.log("browser end show but there is next BROWSER Item ahead")
                    androidBrowser.load(nextItemUrl)
                    androidBrowser.setShowTime(nextItemTime)
                    androidBrowser.startShow()
                }
                else if (nextItemType === "video" || nextItemType === "audio")
                {
                    console.log("browser end show but there is next MULTIMEDIA item ahead")
                    showBrowserTimer.browserVisible = false
                    showBrowserTimer.start()
                    currentType = "videoPlayer"
                    videoPlayer.playPreloaded()
                }
                nextItemUrl = ""
            }
            nextItem()
        }
    }

   /* SideBrowser{
        id: sideBrowser
        visible: false
        x: 0//1.6 * Math.min(item.height, item.width)
        y:
        width: Math.max(item.height, item.width) - 1.6*Math.min(item.height, item.width)
        height: item.height
        onNext: {
            //sideBrowser::next
        }
    }*/
//android:excludeFromRecents="true"

    TeleDSMenu{
        id: menu
        color1: brand_menu_backgroundColor
        color2: brand_menu_foregroundColor
        logo: brand_logoImage
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
            console.log("back key pressed: main")
        }
    }

    Dialog {
        id: dialogAndroid

        // Создаём содержимое диалогового окна
        contentItem: Rectangle {
            width: 600
            height: 500
            color: brand_backgroundColor

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: dividerHorizontal.top
                color: brand_backgroundColor

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
                            color: control.pressed ? brand_borderGrayColor : brand_backgroundColor
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
                            color: control.pressed ? brand_borderGrayColor : brand_backgroundColor
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

    Keys.onReleased:{
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
            /*dialogCloseTimer.start()
            dialogAndroid.open()
            dialogButtonOk.focus = true
            setRestoreModeFalse()
            if (currentType == "browser")
            {
                bgLogoBlock.visible = true
                androidBrowser.visible = false
                videoPlayer.visible = false
            }
            console.log("item width: " + item.width.toString() + " item height: " + item.height.toString()+ "VIS: " + bgLogoImage.visible.toString() +"/" + bgLogoImageV.visible.toString() )

            event.accepted = true
            console.log("back key pressed: main")*/
            if (menu.visible == false)
            {
                setRestoreModeFalse()
                menu.restore()
            }
            else
            {
                menu.visible = false
                setRestoreModeTrue()
            }
            event.accepted = true
        }
    }
}

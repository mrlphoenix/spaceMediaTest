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

    signal nextItem()
    signal refreshId()
    focus: true

    function playFile(filename){
        console.debug("playfile is called" + filename)
        invokeNext = false
        mediaplayer.stop();
        mediaplayer.source = filename;
        mediaplayer.play();
        invokeNext = true
        mediaplayer.volume = 1.0
    }

    function downloadComplete(){
        console.debug("download complete. Hiding Progress Bars, Showing Video Player");
        filesProgressBar.visible = false
        downloadProgressBar.visible = false
        videoOut.visible = true
        playerName.visible = false
        bgLogoImage.visible = false
        logoColumn.visible = false
        videoOut.opacity = videoOutBrightness
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
        videoOut.visible = true
        logoColumn.visible = false
        if (!isVisible)
        {
            overlayBgRect.color = "#333e47"
        }
        else if (videoOutBrightness > 1.0)
        {
            overlayBgRect.color = "#FFFFFF"
        }
        else{
            overlayBgRect.color = "#333e47"
        }
    }

    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            console.log("Coordinate:", coord.longitude, coord.latitude);
        }
    }

    Dialog {
                id: dialogAndroid
              //  width: 600  // for desktop ::TODO
               // height: 500 // for desktop ::TODO

                // Создаём содержимое диалогового окна
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
                            color: "#00cdc1"
                            anchors.centerIn: parent
                        }
                    }

                    // Создаём горизонтальный разделитель с помощью Rectangle
                    Rectangle {
                        id: dividerHorizontal
                        color: "#d7d7d7"
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
                                    color: control.pressed ? "#d7d7d7" : "#333e47"
                                    border.width: 0
                                }

                                label: Text {
                                    text: qsTr("Cancel")
                                    color: "#00cdc1"
                                    // Устанавливаем текст в центр кнопки
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                            onClicked: {
                                dialogAndroid.close()
                                item.focus = true
                            }
                        }

                        Rectangle {
                            id: dividerVertical
                            width: 2
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            color: "#d7d7d7"
                        }

                        Button {
                            id: dialogButtonOk
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: parent.width / 2 - 1

                            style: ButtonStyle {
                                background: Rectangle {
                                    color: control.pressed ? "#d7d7d7" : "#333e47"
                                    border.width: 0
                                }

                                label: Text {
                                    text: qsTr("Ok")
                                    color: "#00cdc1"
                                    // Устанавливаем текст в центр кнопки
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                            onClicked: Qt.quit()
                        }
                    }
                }
            }

    FontLoader {
        id: ubuntuFont
        source: "ubuntu-m.ttf"
    }

    Rectangle {
        id: overlayBgRect
        color: "#333e47"
        width: parent.width
        height: parent.height
    }
    function setBrightness(value){
        videoOutBrightness = value
        if (value > 1.0) {
            overlayBgRect.color = "#FFFFFF"
            videoOut.opacity = 2.0 - value
        }
        else {
            overlayBgRect.color = "#333e47"
            videoOut.opacity = value
        }
    }

    function setNoItemsLogo(link){
        logoColumn.visible = true
        videoOut.opacity = 0
        titleText.text = "No campaign available"
        progressText.text = "Go to <a href=\"" + link + "\">" + link +  "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDItem.visible = false
    }
    function setDownloadLogo(){
        logoColumn.visible = true
        videoOut.opacity = 0
        titleText.text = "Downloading campains"
        progressText.text = "Please wait..."
        logoDownloadProgressBar.visible = true
       playerIDItem.visible = false
    }
    function setNeedActivationLogo(link, playerID){
        logoColumn.visible = true
        videoOut.opacity = 0
        titleText.text = "To setup this player use the following code at"
        progressText.text = "<a href=\"" + link + "\">" + link + "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDText.text = playerID

        playerIDItem.visible = true
        refreshPlayerID.sourceSize.width = playerIDRect.height
        refreshPlayerID.sourceSize.height = playerIDRect.height
        refreshPlayerID.visible = true
        refreshplayerIDAnimation.stop()
        playerIDText.color = "#333e47"
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
            return 24 - decreasingTextValue
        }
    }

    function getRefreshButtonPositionX(w, h){
        if (w > h)
        {
            return playerIDRect.x + playerIDRect.width + playerIDRect.height/2
        }
        else
        {
            return w/2 - refreshPlayerID.width/2
        }
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
    Image {
        id: bgLogoImage
        source: "logo_bg.svg"
        sourceSize.width: Math.max(parent.width, parent.height)
        sourceSize.height: Math.max(parent.height,parent.width)
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop
    }
    Image {
        id: bgLogoImageV
        visible: parent.width < parent.height
        source: "logo_bg_vertical.svg"
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop
    }
    Item
    {
        id: logoColumn
        width: parent.width
        y: parent.height * 0.248148148 * aspect

        Image {
            id: teledsLogoImg
            source: "logo.svg"
            sourceSize.width: 195.0 * aspect
            sourceSize.height: 156.0 * aspect
            x: parent.width/2 - width/2
        }
        Image {
            id: teledsLogo
            y: teledsLogoImg.y + teledsLogoImg.height + (22.0 * aspect)
            x: parent.width/2 - width/2
            source: "logo_teleds.png"

            scale: aspect
        }

        Rectangle {
            id: brRect
            width: 124 * aspect
            height: 8 * aspect
            y: teledsLogo.y + teledsLogo.height + (75.0 * aspect)
            x: parent.width/2 - width/2
            color: "#00cdc1"
        }
        Text {
            id: titleText
            font.family: ubuntuFont.name
            font.pointSize: getPointSize(text)
            text: "Initialization"
            x: parent.width/2 - width/2
            y: brRect.y + brRect.height + (75.0 * aspect)
            color: "white"
        }
        Text {
            id: progressText
            font.family: ubuntuFont.name
            font.pointSize: getPointSize(text)
            text: "..."
            x: parent.width/2 - width/2
            y: titleText.y + titleText.height + 18 * aspect
            color: "#00cdc1"
            linkColor: "#00cdc1"
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
                        color: "#00cdc1"
                        border.color: "#00cdc1"
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
                color: "#00cdc1"
                Text {
                    id: playerIDText
                    text: ""
                    color: "#333e47"
                    font.family: ubuntuFont.name
                    font.pointSize: 38
                }
                width: childrenRect.width
                height: childrenRect.height
                y: progressText.y + progressText.height + 55 * aspect
                x: parent.width/2 - width/2
            }
            MouseArea {
                x: getRefreshButtonPositionX(item.width, item.height)
                y: getRefreshButtonPositionY(item.width, item.height)
                Image {
                    id: refreshPlayerID
                    source: "refresh_2.svg"
                    smooth: true

                    RotationAnimator {
                        id: refreshplayerIDAnimation
                        target: refreshPlayerID
                        from: 0; to: 360
                        duration: 1000
                        loops: 3600
                    }
                }
                width: refreshPlayerID.width
                height: refreshPlayerID.height
                onClicked:{
                    if (refreshplayerIDAnimation.running == false)
                    {
                        playerIDText.color = "transparent"
                       // refreshPlayerID.visible = false
                      //  playerIDText.text = " Please wait... "
                        item.refreshId()
                        refreshplayerIDAnimation.start()
                    }
                }
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



    MediaPlayer {
        id: mediaplayer
        autoLoad: true
        source: "file:///sdcard/download/teleds/1.mp4"
        onStopped:{
            console.debug("on stopped")
            if (invokeNext){
                console.debug("calling nextItem")
                item.nextItem()
            }
        }
    }

    VideoOutput {
        visible: false
        id: videoOut
        anchors.fill: parent
        source: mediaplayer
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back) {
           dialogAndroid.open()
            // quitDialog.open()
            event.accepted = true
            console.log("back key pressed")
        }
    }
}

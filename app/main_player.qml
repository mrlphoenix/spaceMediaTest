import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0
import QtQuick.Controls.Styles 1.4


Item {
    id: item
    property bool invokeNext:true
    property double videoOutBrightness: 1.0
    property double hValue: Math.min(height, width)
    property double aspect: Math.min(hValue / 1080.0, 1.0)
    property int decreasingTextValue: 0
    signal nextItem()

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
        playerIDRect.visible = false
    }
    function setDownloadLogo(){
        logoColumn.visible = true
        videoOut.opacity = 0
        titleText.text = "Downloading campains"
        progressText.text = "Please wait..."
        logoDownloadProgressBar.visible = true
        playerIDRect.visible = false
    }
    function setNeedActivationLogo(link, playerID){
        logoColumn.visible = true
        videoOut.opacity = 0
        titleText.text = "To setup this player use the following code at"
        progressText.text = "<a href=\"" + link + "\">" + link + "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDText.text = playerID
        playerIDRect.visible = true
    }
    function getPointSize(text){
        if (text.length > 40)
            decreasingTextValue = text.length-40
        else
            decreasingTextValue = 0
        if (parent.width > parent.height)
            return 32 - decreasingTextValue
        else
        {
            if (decreasingTextValue)
                decreasingTextValue += 2
            return 24 - decreasingTextValue
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
            text: "Please wait..."
            x: parent.width/2 - width/2
            y: titleText.y + titleText.height + 18 * aspect
            color: "#00cdc1"
            linkColor: "#00cdc1"
            onLinkActivated: Qt.openUrlExternally(link)
        }
        ProgressBar{
            id: logoDownloadProgressBar
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
        Rectangle {
            id: playerIDRect
            color: "#00cdc1"
            visible: false
            Text {
                id: playerIDText
                text: ""
                color: "#333e47"
                font.family: ubuntuFont.name
                font.pointSize: 40
            }
            width: childrenRect.width
            height: childrenRect.height
            y: progressText.y + progressText.height + 55 * aspect
            x: parent.width/2 - width/2
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
}

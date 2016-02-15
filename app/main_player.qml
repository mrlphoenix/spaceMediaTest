import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0
import QtQuick.Controls.Styles 1.4


Item {
    id: item
    property bool invokeNext:true
    property double videoOutBrightness: 1.0
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
        videoOut.visible = isVisible
        if (!isVisible)
        {
            overlayBgRect.color = "black"
        }
        else if (videoOutBrightness > 1.0)
        {
            overlayBgRect.color = "#FFFFFF"
        }
        else{
            overlayBgRect.color = "black"
        }
    }

    Rectangle {
        id: overlayBgRect
        color: "black"
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
            overlayBgRect.color = "black"
            videoOut.opacity = value
        }
    }

    Column
    {
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
        //autoPlay: true
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

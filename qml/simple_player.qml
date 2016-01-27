import QtQuick 2.0
import QtMultimedia 5.0

Item {
    id: item
    property bool invokeNext:true
    signal nextItem()

    function playFile(filename){
        invokeNext = false
        mediaplayer.stop();
        mediaplayer.source = filename;
        mediaplayer.play();
        invokeNext = true
    }

    MediaPlayer {
        id: mediaplayer
        autoLoad: true
        autoPlay: true
        source: "file:///home/pi/data/video/2.mp4"
        onStopped:{
            console.debug("on stopped")
            if (invokeNext){
                console.debug("calling nextItem")
                item.nextItem()
            }
        }
    }

    VideoOutput {
        anchors.fill: parent
        source: mediaplayer
    }
}

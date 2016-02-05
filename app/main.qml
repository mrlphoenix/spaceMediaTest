import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0


Item {
    id: item
    property bool invokeNext:true
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
        anchors.fill: parent
        source: mediaplayer
    }
}

import QtQuick 2.0
import QtMultimedia 5.0

Item {
    signal next()
    property bool firstPlayer: true
    anchors.fill: parent

    function getDuration(duration){
        if (duration < 1000){
            console.log("duration too little - return 10000")
            return 10000
        }
        else{
            console.log("duration = " + duration)
            return duration - 1200
        }
    }

    Timer{
        id: nextVideoTimer
        repeat: false
        onTriggered: {
            console.log("NEXT VIDEO!")
            if (firstPlayer){
                console.log("MP2 Play called")
                firstPlayer = false
            //    videoOut2.visible = true
                mp2.play()
            } else {
                console.log("MP1 Play called")
                firstPlayer = true
             //   videoOut1.visible = true
                mp1.play()
            }
            next()
        }
    }

    Timer{
        id: showVideo1
        repeat: false
        interval: 1000
        onTriggered:{
           // videoOut1.visible = true
            videoOut2.visible = false
        }
    }

    Timer{
        id: showVideo2
        repeat: false
        interval: 1000
        onTriggered:{
            videoOut2.visible = true
            videoOut1.visible = false
        }
    }

    VideoOutput {
        visible: true
        id: videoOut1
        anchors.fill: parent
        source: mp1
    }
    VideoOutput {
        visible: false
        id: videoOut2
        anchors.fill: parent
        source: mp2
    }

    MediaPlayer{
        id: mp1
        autoLoad: true
        source: ""
        property bool fakePlay: false

        onPlaying:{
            console.log("MP1:onPlay")
            if (fakePlay == false)
            {
                videoOut1.visible = true
                showVideo1.start()
                nextVideoTimer.interval = getDuration(mp1.duration)
                nextVideoTimer.start()
            }
            else{
                console.log("MP1:Fake play called")
                fakePlay = false
              //  pause()
            }
        }
    }

    MediaPlayer{
        id: mp2
        autoLoad: true
        source: ""
        property bool fakePlay: false

        onPlaying:{
            console.log("MP2:onPlay")
            if (fakePlay == false)
            {
                videoOut2.visible = true
                videoOut1.visible = false
                //showVideo2.start()
                nextVideoTimer.interval = getDuration(mp2.duration)
                nextVideoTimer.start()
            }
            else{
                console.log("MP2:Fake play called")
                fakePlay = false
               // pause()
            }
        }
    }

    function playItem(filename){
        if (mp1.source == ""){
            console.log("Filling src < MP1")
            mp1.source = filename
            mp1.play()
        }
        else if (mp2.source == "")
        {
            console.log("Filling src < MP2")
            mp2.source = filename
         //   mp2.fakePlay = true
         //   mp2.play()
        }
        else{
            if (firstPlayer){
                console.log("Load to MP2")
                mp2.source = filename
              //  mp2.fakePlay = true
               // mp2.stop()
             //   mp2.play()
             //   mp2.pause()
            }
            else{
                console.log("Load to MP1")
                mp1.source = filename
            //    mp1.fakePlay = true
                //mp1.stop()
              //  mp1.play()
              //  mp1.pause()
            }
        }
    }
}


import QtQuick 2.0
import QtMultimedia 5.0

Item {
    signal next()
    signal videoPlayed()
    signal audioPlayed()
    property bool firstPlayer: true
    property bool isVideoPlaying: true
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
                mp2.play()
            } else {
                console.log("MP1 Play called")
                firstPlayer = true
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
            videoOut2.opacity = 0.0
        }
    }

    Timer{
        id: showVideo2
        repeat: false
        interval: 1000
        onTriggered:{
            videoOut2.opacity = 1.0
            videoOut1.opacity = 0.0
        }
    }

    VideoOutput {
        visible: true
        id: videoOut1
        anchors.fill: parent
        source: mp1
    }
    VideoOutput {
        visible: true
        opacity: 0
        id: videoOut2
        anchors.fill: parent
        source: mp2
    }

    MediaPlayer{
        id: mp1
        autoLoad: true
        source: ""
        property bool isVideo: true
        property bool fakePlay: false

        onPlaying:{
            console.log("MP1:onPlay")
            if (isVideo){
                if (!isVideoPlaying){
                    isVideoPlaying = true
                    videoPlayed()
                }
            }
            else{
                if (isVideoPlaying){
                    isVideoPlaying = false
                    audioPlayed()
                }
            }
            if (fakePlay == false)
            {
                videoOut1.opacity = 1
                showVideo1.start()
                nextVideoTimer.interval = getDuration(mp1.duration)
                nextVideoTimer.start()
            }
            else{
                console.log("MP1:Fake play called")
                fakePlay = false
            }
        }
    }

    MediaPlayer{
        id: mp2
        autoLoad: true
        source: ""
        property bool isVideo: true
        property bool fakePlay: false

        onPlaying:{
            console.log("MP2:onPlay")
            if (isVideo){
                if (!isVideoPlaying){
                    isVideoPlaying = true
                    videoPlayed()
                }
            }
            else{
                if (isVideoPlaying){
                    isVideoPlaying = false
                    audioPlayed()
                }
            }

            if (fakePlay == false)
            {
                videoOut2.opacity = 1.0
                videoOut1.opacity = 0.0
                nextVideoTimer.interval = getDuration(mp2.duration)
                nextVideoTimer.start()
            }
            else{
                console.log("MP2:Fake play called")
                fakePlay = false
            }
        }
    }

    function playItem(filename){
        if (mp1.source == ""){
            console.log("Filling src < MP1")
            mp1.source = filename
            mp1.play()
            mp1.isVideo = true
        }
        else if (mp2.source == "")
        {
            console.log("Filling src < MP2")
            mp2.source = filename
            mp2.isVideo = true
        }
        else{
            if (firstPlayer){
                console.log("Load to MP2")
                mp2.source = filename
                mp2.isVideo = true
            }
            else{
                console.log("Load to MP1")
                mp1.source = filename
                mp1.isVideo = true
            }
        }
    }
    function playAudioItem(filename){
        if (mp1.source == ""){
            console.log("Filling src < MP1")
            mp1.source = filename
            mp1.play()
            mp1.isVideo = false
        }
        else if (mp2.source == "")
        {
            console.log("Filling src < MP2")
            mp2.source = filename
            mp2.isVideo = false
        }
        else{
            if (firstPlayer){
                console.log("Load to MP2")
                mp2.source = filename
                mp2.isVideo = false
            }
            else{
                console.log("Load to MP1")
                mp1.source = filename
                mp1.isVideo = false
            }
        }
    }
}


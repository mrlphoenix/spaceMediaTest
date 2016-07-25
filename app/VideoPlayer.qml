import QtQuick 2.0
import QtMultimedia 5.0

Item {
    signal next()
    signal videoPlayed()
    signal audioPlayed()
    signal videoStopped()
    property bool firstPlayer: true
    property bool isVideoPlaying: true
    property bool prepareStop: false
    anchors.fill: parent

    function getDuration(duration){
        if (duration < 1000){
            console.log("duration too little - return 10000")
            return 10000
        }
        else{
            console.log("duration = " + duration)
            return duration - 300
        }
    }

    function stopPlayer(){
        nextVideoTimer.stop()
        mp1.stop()
        mp1.source = ""
        mp2.stop()
        mp2.source = ""
        isVideoPlaying = true
        firstPlayer = true
    }

    Timer{
        id: nextVideoTimer
        repeat: false
        onTriggered: {
            if (prepareStop == false){
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
            else{
                prepareStop = false
                videoStopped()
            }
        }
    }

    Timer{
        id: showVideo1
        repeat: false
        interval: 340
        onTriggered:{
            videoOut2.opacity = 0.0
        }
    }

    Timer{
        id: showVideo2
        repeat: false
        interval: 340
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
        property int durationMsecs: 0
        property int seekMsecs: 0

        onPlaying:{
            console.log("MP1:onPlay::" + isVideo + " source=" + source)
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
                if (durationMsecs == 0)
                    nextVideoTimer.interval = getDuration(mp1.duration)
                else
                    nextVideoTimer.interval = durationMsecs -500
                nextVideoTimer.start()
                if (seekMsecs > 0)
                {
                    console.log("MP1:seek " + seekMsecs)
                    seek(seekMsecs)
                }
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
        property int durationMsecs: 0
        property int seekMsecs: 0

        onPlaying:{
            console.log("MP2:onPlay " + isVideo + " source=" + source)
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
                //showVideo2.start()
                videoOut2.opacity = 1.0
                videoOut1.opacity = 0.0
                console.log("MP2:OnPlay Duration = " + durationMsecs)
                if (durationMsecs == 0)
                    nextVideoTimer.interval = getDuration(mp2.duration)
                else
                    nextVideoTimer.interval = durationMsecs -500
                nextVideoTimer.start()
                if (seekMsecs > 0)
                {
                    console.log("MP2:seek " + seekMsecs)
                    seek(seekMsecs)
                }
            }
            else{
                console.log("MP2:Fake play called")
                fakePlay = false
            }
        }
    }

    //function called when we ned to preload first item after browser
    function preloadItem(filename, type, duration, seek)
    {
        console.log("Filling src < MP1")
        mp1.durationMsecs = duration
        mp1.seekMsecs = seek
        mp1.source = filename
        if (type === "video")
            mp1.isVideo = true
        else
            mp1.isVideo = false
    }
    function playPreloaded()
    {
        mp1.play()
    }

    function playItemGeneric(filename, type, duration, seek)
    {
        if (mp1.source == ""){
            console.log("Filling src < MP1")
            mp1.durationMsecs = duration
            mp1.seekMsecs = seek
            mp1.source = filename
            if (type === "video")
                mp1.isVideo = true
            else
                mp1.isVideo = false
            mp1.play()
        }
        else if (mp2.source == "")
        {
            console.log("Filling src < MP2")
            mp2.source = filename
            if (type === "video")
                mp2.isVideo = true
            else
                mp2.isVideo = false
            mp2.durationMsecs = duration
            mp2.seekMsecs = seek
        }
        else{
            if (firstPlayer){
                console.log("Load to MP2")
                mp2.source = filename
                if (type === "video")
                    mp2.isVideo = true
                else
                    mp2.isVideo = false
                mp2.durationMsecs = duration
                mp2.seekMsecs = seek
            }
            else{
                console.log("Load to MP1")
                mp1.source = filename
                if (type === "video")
                    mp1.isVideo = true
                else
                    mp1.isVideo = false
                mp1.durationMsecs = duration
                mp1.seekMsecs = seek
            }
        }
    }

    function playItem(filename){
        playItemGeneric(filename,"video",0,0)
    }
    function playAudioItem(filename){
        playItemGeneric(filename,"audio",0,0)
    }
    function playAudioItemAdv(filename, length, seek){
        playItemGeneric(filename,"audio",length,seek)
    }
    function playItemAdv(filename, length, seek){
        playItemGeneric(filename,"video",length,seek)
    }
}




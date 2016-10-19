import QtQuick 2.2
import QtQuick.Controls 1.1
//import QtWebView 1.1
import QtWebKit 3.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtMultimedia 5.0

Item {

    property string areaID: ""
    property double brightness: 1.0
    property double volume: 1.0

    signal askNext(string areaId)
    signal onStop()


    property bool isPlaying: false
    property string currentType: "null"
    property string nextItemType: "--"


    function play(contentId, length, type, skip)
    {
        console.log("PlayerView::play" + contentId + " " + length + " " + type +" " + skip)
        if (type === "video")
        {
            if (currentType === "null")
            {
                console.log("play::t:null")
                //in this case we need to load video into videoplayer
                videoPlayer.play(contentId, length, skip)
                currentType = "video"
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                console.log("play::t:video")
                nextItemType = "video"
                videoPlayer.play(contentId, length, skip)
            }
            else if (currentType === "browser")
            {
                nextItemType = "video"
                videoPlayer.preloadItem(contentId, length, skip)
            }
        }
        else if (type === "html5_online")
        {
            if (currentType === "null")
            {
                browser.load(contentId, length)
                currentType = "browser"
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                nextItemType = "browser"
                browser.preload(contentId, length)
                videoPlayer.prepareReset()
            }
            else if (currentType === "browser")
            {
                nextItemType = "browser"
                browser.load(contentId, length)
            }
        }
    }
    function stop()
    {
        //stop and hide everything
    }

    function getCurrentType()
    {

    }

    Rectangle
    {
        id: bgRect
        color: "#000000"
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
    }

    Image {
        id: contentImage1
        x: parent.x
        y: parent.y
        width: parent.width
        height: parent.height
        opacity: brightness
        visible: false
        source: ""
    }

    Image {
        id: contentImage2
        x: parent.x
        y: parent.y
        width: parent.width
        height: parent.height
        opacity: brightness
        visible: false
        source: ""
    }

    Item{
        id: videoPlayer
        visible: true
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
        //opacity: brightness
        signal onStop()

        property bool isVideo: true
        property int durationMsecs: 0
        property int seekMsecs: 0
        property bool prepareResetP: false
        property bool firstPlayer: true

        Timer {
            id: videoPlayerTimer
            repeat: false
            onTriggered: {
                console.log("VPT::onTriggered")
                if (videoPlayer.prepareResetP)
                {
                    videoPlayer.reset();
                    videoPlayer.prepareResetP = false;
                }
                console.log(nextItemType)
                if (nextItemType === "video"){
                    if (videoPlayer.firstPlayer){
                        videoPlayer.firstPlayer = false
                        mp2.play()
                    }
                    else {
                        videoPlayer.firstPlayer = true
                        mp1.play()
                    }
                    askNext(areaID)
                }
                else if (nextItemType === "browser"){
                    browser.showPreloaded()
                    videoPlayer.reset()
                }
            }
        }

        function play(_url, _length, _skip)
        {
            if (mp1.source == ""){
                console.log("Filling src < MP1")
                mp1.durationMsecs = _length
                mp1.seekMsecs = _skip
                mp1.source = _url
                mp1.play()
            }
            else if (mp2.source == "")
            {
                console.log("Filling src < MP2")
                mp2.source = _url
                mp2.durationMsecs = _length
                mp2.seekMsecs = _skip
            }
            else{
                if (firstPlayer){
                    console.log("Load to MP2")
                    mp2.source = _url
                    mp2.durationMsecs = _length
                    mp2.seekMsecs = _skip
                }
                else{
                    console.log("Load to MP1")
                    mp1.source = _url
                    mp1.durationMsecs = _length
                    mp1.seekMsecs = _skip
                }
            }
        }
        function prepareReset()
        {
            prepareResetP = true
        }
        function reset()
        {
            prepareResetP = false
            videoPlayerTimer.stop()
            mp1.stop()
            mp1.source = ""
            mp2.stop()
            mp2.source = ""
            firstPlayer = true
        }
        function preloadItem(_url, length, skip)
        {
            mp1.source = _url
            mp1.durationMsecs = length
            mp1.seekMsecs = skip
        }
        function playPreloaded()
        {
            mp1.play()
        }

        MediaPlayer{
            id: mp1
            autoLoad: true
            autoPlay: false
            source: ""
            property int durationMsecs: 0
            property int seekMsecs: 0
            onPlaying:{
                console.log("MP1::onPlay " + mp1.source)
                videoOutput2.opacity = 0.0
                videoOutput1.opacity = 1.0
                videoPlayerTimer.interval = Math.max(mp1.durationMsecs, 5000) - 400

                console.log("MP1::VPT " + videoPlayerTimer.interval)
                videoPlayerTimer.start()
                if (mp1.seekMsecs > 0)
                    mp1.seek(mp1.seekMsecs)
                //ask for new item or smth
            }
        }
        VideoOutput{
            id: videoOutput1
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            source: mp1
            opacity: 0.0
            visible: true
        }

        MediaPlayer{
            id: mp2
            autoLoad: true
            autoPlay: false
            source: ""
            property int durationMsecs: 0
            property int seekMsecs: 0
            onPlaying: {
                console.log("MP2::onPlay " + mp2.source)
                videoOutput1.opacity = 0.0
                videoOutput2.opacity = 1.0
                videoPlayerTimer.interval = Math.max(mp2.durationMsecs, 5000) - 400
                videoPlayerTimer.start()
                if (mp2.seekMsecs > 0)
                    mp2.seek(mp2.seekMsecs)
            }
        }

        VideoOutput{
            id: videoOutput2
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            source: mp2
            opacity: 0.0
        }

        Image{
            id: audioIcon
            visible: false
            source: "audio.svg"
            sourceSize.width: parent.width/2
            sourceSize.height: parent.height/2
            width: parent.width/2
            height: parent.height/2
            x: parent.width/2 - width/2
            y: parent.height/2 - height/2
        }
    }



    Item{
        id: browser
        visible: true
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
       // opacity: brightness
        property bool prepareStop: false

        function preload(url, showtime)
        {
            console.log("browser.preload")
            sideBrowser1.preload(url, showtime)
            isFirstBrowser = true
            return
        }

        function showPreloaded()
        {
            sideBrowser1.showPreloaded()
        }

        function load(url, showtime){
            //load first time
            console.log("WebBrowser::load -> reloading page | " +
                        sideBrowser1.x + " " + sideBrowser1.y + " " +
                        sideBrowser1.width + " " + sideBrowser1.height);
            if (isFirstBrowser == false && sideBrowser1.prevUrl === "#none"){
                sideBrowser1.load(url, showtime)
                isFirstBrowser = true
                return
            }
            if (isFirstBrowser && sideBrowser2.prevUrl === "#none"){
                sideBrowser2.preload(url,showtime)
                return
            }
            if (isFirstBrowser){
                sideBrowser2.showPreloaded()
                sideBrowser1.preload(url, showtime)
                isFirstBrowser = false
                return
            }
            else
            {
                sideBrowser1.showPreloaded()
                sideBrowser2.preload(url, showtime)
                isFirstBrowser = true
                return
            }
        }

        function stopBrowser(){
            sideBrowser1.visible = false
            sideBrowser2.visible = false
            sideBrowser1.stop()
            sideBrowser2.stop()
            isFirstBrowser = false
            turnOffTimer.stop()
        }

        property bool isFirstBrowser: false

        Timer {
            id: turnOffTimer
            repeat: false
            onTriggered: {
                if (nextItemType === "browser"){
                    if (sideBrowser1.visible){
                        sideBrowser1.visible = false
                        sideBrowser2.showPreloaded();
                    }
                    else {
                        sideBrowser2.visible = false
                        sideBrowser1.showPreloaded()
                    }
                }
                else if (nextItemType === "video"){
                    browser.stopBrowser()
                    videoPlayer.playPreloaded()
                }
                askNext(areaID)
            }
        }

        WebView {
            id: sideBrowser1
            visible: false
            anchors.fill: parent
            property string prevUrl:"#none"
            property int showtime: 0
            url: ""

            function preload(_url, _showtime){
                if (prevUrl === _url)
                    reload()
                else
                    url = _url
                showtime = _showtime
                prevUrl = _url
            }
            function showPreloaded(){
                turnOffTimer.interval = showtime
                turnOffTimer.restart()
                visible = true
                sideBrowser2.visible = false
            }
            function load(_url, _showtime){
                preload(_url, _showtime)
                showPreloaded()
            }
            function stop(){
                prevUrl = "#none"
                url = ""
            }
        }

        WebView {
            id: sideBrowser2
            visible: false
            anchors.fill: parent
            property string prevUrl:"#none"
            property int showtime: 0
            url: ""
            function preload(_url, _showtime){
                if (prevUrl === _url)
                    reload()
                else
                    url = _url
                showtime = _showtime
                prevUrl = _url
            }
            function showPreloaded(){
                turnOffTimer.interval = showtime
                turnOffTimer.restart()
                visible = true
                sideBrowser1.visible = false
            }
            function load(_url, _showtime){
                preload(_url, _showtime)
                showPreloaded()
            }
            function stop(){
                prevUrl = "#none"
                url = ""
            }
        }
    }


}


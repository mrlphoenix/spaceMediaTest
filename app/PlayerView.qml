import QtQuick 2.2
import QtQuick.Controls 1.1
//import QtWebView 1.1
import QtWebKit 3.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtMultimedia 5.5

Item {

    property string areaID: ""
    property double brightness: 1.0
    property double volume: 1.0

    signal askNext(string areaId)
    signal onStop()


    function play(contentId, length, type)
    {
        //play item
    }
    function stop()
    {
        //stop and hide everything
    }
    function prepareStop()
    {
        //play last item and stop
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

    Item{
        id: browser
        visible: false
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
        opacity: brightness
        property bool prepareStop: false

        onNext: {
            //this method is called when browser stops show
            //do what is done right now in main_player.qml
        }

        function load(url, showtime){
            //load first time
            console.log("WebBrowser::load -> reloading page")
            if (isFirstBrowser == false && sideBrowser1.prevUrl === "#none"){
                sideBrowser1.load(url, showtime)
                isFirstBrowser = true
                next()
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
        signal next()

        Timer {
            id: turnOffTimer
            repeat: false
            onTriggered: {
                next()
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

    Item{
        id: videoPlayer
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
        opacity: brightness
        signal onStop()

        property bool isVideo: true
        property int durationMsecs: 0
        property int seekMsecs: 0

        function play(_url, _type)
        {
            //if video - load/preload and show proper video output with delay
            //if audio - load to next mp and show audio Image
        }
        function prepareReset()
        {
            //this method should be called when next item will be browser or no item at all
            //when video/audio will stop playing - hide everything instead of asking new player
        }
        function reset()
        {
            //this function actually resets videoplayer
            //we should reset every field about current video player and hide pics and videooutputs
        }

        MediaPlayer{
            id: mp1
            autoLoad: false
            autoPlay: false
            source: ""
            onPlaying:{
                videoOutput1.opacity = 1.0
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
        }

        MediaPlayer{
            id: mp2
            autoLoad: false
            autoPlay: false
            source: ""
            onPlaying: {

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
}


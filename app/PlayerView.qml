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
    property double volumeValue: 1.0

    signal askNext(string areaId)
    signal onStop()


    property bool isPlaying: false
    property string currentType: "null"
    property string nextItemType: "--"


    function play(contentId, length, type, skip, fillMode)
    {
        var isAudio = false
        if (type === "audio"){
            isAudio = true
            type = "video"
        }

        console.log("PlayerView::play" + contentId + " " + length + " " + type +" " + skip)
        if (type === "video")
        {
            if (currentType === "null")
            {
                console.log("play::t:null")
                //in this case we need to load video into videoplayer
                videoPlayer.play(contentId, length, skip, isAudio, fillMode)
                currentType = "video"
                console.log("askNext play::video::null")
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                console.log("play::t:video")
                nextItemType = "video"
                videoPlayer.play(contentId, length, skip, isAudio, fillMode)
            }
            else if (currentType === "browser")
            {
                console.log("play::t:browser")
                nextItemType = "video"
                videoPlayer.preloadItem(contentId, length, skip, isAudio, fillMode)
            }
            else if (currentType === "image")
            {
                console.log("play::t:image")
                nextItemType = "video"
                videoPlayer.preloadItem(contentId, length, skip, isAudio, fillMode)
            }
        }
        else if (type === "html5_online")
        {
            if (currentType === "null")
            {
                console.log("play::t:null")
                browser.load(contentId, length)
                currentType = "browser"

                console.log("askNext play::html5::null")
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                console.log("play::t:video")
                nextItemType = "browser"
                browser.preload(contentId, length)
            }
            else if (currentType === "image")
            {
                console.log("play::t:video")
                nextItemType = "browser"
                browser.preload(contentId, length)
            }
            else if (currentType === "browser")
            {
                console.log("play::t:browser")
                nextItemType = "browser"
                browser.load(contentId, length)
            }
        }
        else if (type === "image")
        {
            if (currentType === "null")
            {
                console.log("play::t:null")
                currentType = "image"
                imageContent.load(contentId, length, fillMode)
                console.log("askNext play::image::null")
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                console.log("play::t:video")
                nextItemType = "image"
                imageContent.preload(contentId, length, fillMode)
            }
            else if (currentType === "browser")
            {
                console.log("play::t:browser")
                nextItemType = "image"
                imageContent.preload(contentId, length, fillMode)
            }
            else if (currentType === "image")
            {
                console.log("play::t:image")
                nextItemType = "image"
                imageContent.load(contentId, length, fillMode)
            }
        }
    }
    function stop()
    {
        //stop and hide everything
        videoPlayer.reset()
        browser.stopBrowser()
        imageContent.stop()
        currentType = "null"
    }

    Rectangle {
        id: bgRect
        color: "#000000"
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
    }

    Item {
        id: videoPlayer
        visible: currentType != "browser"
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
        opacity: brightness
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
                if (nextItemType === "video"){
                    console.log("VPT:video")
                    if (videoPlayer.firstPlayer){
                        videoPlayer.firstPlayer = false
                        mp2.play()
                    }
                    else {
                        videoPlayer.firstPlayer = true
                        mp1.play()
                    }
                }
                else if (nextItemType === "browser"){
                    console.log("VPT:browser")
                    browser.showPreloaded()
                    currentType = "browser"
                    videoPlayer.reset()
                }
                else if (nextItemType === "image"){
                    console.log("VPT:image")
                    imageContent.showPreloaded()
                    currentType = "image"
                    videoPlayer.reset()
                }

                console.log("askNext play::VPT")
                askNext(areaID)
            }
        }

        function play(_url, _length, _skip, _isAudio, _fillMode)
        {
            if (mp1.source == ""){
                console.log("Filling src < MP1")
                mp1.durationMsecs = _length
                mp1.seekMsecs = _skip
                mp1.source = _url
                mp1.isAudio = _isAudio
                mp1.play()
                if (_isAudio === false)
                    videoOutput1.setFillMode(_fillMode)
            }
            else if (mp2.source == "")
            {
                console.log("Filling src < MP2")
                mp2.source = _url
                mp2.durationMsecs = _length
                mp2.seekMsecs = _skip
                mp2.isAudio = _isAudio
                if (_isAudio === false)
                    videoOutput2.setFillMode(_fillMode)
            }
            else{
                if (firstPlayer){
                    console.log("Load to MP2")
                    mp2.source = _url
                    mp2.durationMsecs = _length
                    mp2.seekMsecs = _skip
                    mp2.isAudio = _isAudio
                    if (_isAudio === false)
                        videoOutput2.setFillMode(_fillMode)
                }
                else{
                    console.log("Load to MP1")
                    mp1.source = _url
                    mp1.durationMsecs = _length
                    mp1.seekMsecs = _skip
                    mp1.isAudio = _isAudio
                    if (_isAudio === false)
                        videoOutput1.setFillMode(_fillMode)
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
            //currentType = "null"
        }

        function preloadItem(_url, length, skip, _isAudio, _fillMode)
        {
            console.log("preload item to mp1")
            mp1.source = _url
            mp1.durationMsecs = length
            mp1.seekMsecs = skip
            mp1.isAudio = _isAudio
            if (_isAudio === false)
                videoOutput1.setFillMode(_fillMode)
        }
        function playPreloaded()
        {
            mp1.play()
        }
        Timer {

            function activateTimer(isFirstVideo){
                showFirstVideo = isFirstVideo
                antiFlickTimerVideo.restart()
            }

            property bool showFirstVideo: true

            id: antiFlickTimerVideo
            repeat: false
            interval: 500
            onTriggered: {
                 if (showFirstVideo){
                     videoOutput2.opacity = 0.0
                     videoOutput1.opacity = 1.0
                 }
                 else{
                     videoOutput1.opacity = 0.0
                     videoOutput2.opacity = 1.0
                 }
            }
        }

        MediaPlayer{
            id: mp1
            autoLoad: true
            autoPlay: false
            source: ""
            property int durationMsecs: 0
            property int seekMsecs: 0
            property bool isAudio: false
            onPlaying:{
                console.log("MP1::onPlay " + mp1.source + " " + volume)
                antiFlickTimerVideo.activateTimer(true)
                //videoOutput2.opacity = 0.0
                //videoOutput1.opacity = 1.0
                videoPlayerTimer.interval = Math.max(mp1.durationMsecs, 5000) - 300

                console.log("MP1::VPT " + videoPlayerTimer.interval)
                videoPlayerTimer.start()
                if (mp1.seekMsecs > 0)
                    mp1.seek(mp1.seekMsecs)
                if(isAudio)
                    audioIcon.visible = true
                else
                    audioIcon.visible = false
            }
            volume: 0.5
           // volume: 100
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
            function setFillMode(fill_mode){
                if (fill_mode === "stretch")
                    videoOutput1.fillMode = VideoOutput.Stretch
                else if (fill_mode === "preserveAspectCrop")
                    videoOutput1.fillMode = VideoOutput.PreserveAspectCrop
                else
                    videoOutput1.fillMode = VideoOutput.PreserveAspectFit
            }

        }

        MediaPlayer{
            id: mp2
            autoLoad: true
            autoPlay: false
            source: ""
            property int durationMsecs: 0
            property int seekMsecs: 0
            property bool isAudio: false
            onPlaying: {
                console.log("MP2::onPlay " + mp2.source)
                antiFlickTimerVideo.activateTimer(false)
               // videoOutput1.opacity = 0.0
               // videoOutput2.opacity = 1.0
                videoPlayerTimer.interval = Math.max(mp2.durationMsecs, 5000) - 300
                videoPlayerTimer.start()
                if (mp2.seekMsecs > 0)
                    mp2.seek(mp2.seekMsecs)
                if(isAudio)
                    audioIcon.visible = true
                else
                    audioIcon.visible = false
            }
            //volume: volumeValue
        }

        VideoOutput{
            id: videoOutput2
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            source: mp2
            opacity: 0.0

            function setFillMode(fill_mode){
                if (fill_mode === "stretch")
                    videoOutput2.fillMode = VideoOutput.Stretch
                else if (fill_mode === "preserveAspectCrop")
                    videoOutput2.fillMode = VideoOutput.PreserveAspectCrop
                else
                    videoOutput2.fillMode = VideoOutput.PreserveAspectFit
            }

        }

        Item{
            id: audioIcon
            visible: false
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            Rectangle{
                x: parent.x
                y: parent.y
                width: parent.width
                height: parent.height
                color: "#333e47"
            }
            Image{
                id: audioIconImage
                source: "audio.svg"
                sourceSize.width: parent.width
                sourceSize.height: parent.height
                width: parent.width/2
                height: parent.height/2
                x: parent.width/2 - width/2
                y: parent.height/2 - height/2
            }
        }
    }

    Item {
        id: imageContent
        x: parent.x
        y: parent.y
        width: parent.width
        height: parent.height
        opacity: brightness
        property bool isFirstImage: false
        Rectangle {
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            color: "#000000"
            visible: contentImage1.visible || contentImage2.visible
        }

        Timer {
            id: imageOffTimer
            repeat: false
            onTriggered: {
                console.log("image off timer")
                if (nextItemType === "image"){
                    console.log("next item is image")
                    if (contentImage1.visible){
                        contentImage1.visible = false
                        contentImage2.showPreloaded()
                    }
                    else {
                        contentImage2.visible = false
                        contentImage1.showPreloaded()
                    }
                }
                else if (nextItemType === "video"){
                    console.log("next item is video")
                    imageContent.stop()
                    videoPlayer.playPreloaded()
                    currentType = "video"
                }
                else if (nextItemType === "browser"){
                    console.log("next item is browser")
                    imageContent.stop()
                    browser.showPreloaded()
                    currentType = "browser"
                }
                askNext(areaID)
            }
        }

        function preload(url, showtime, fill_mode){
            console.log("image.preload")
            contentImage1.source = url
            contentImage1.showtime = showtime
            contentImage1.setFillMode(fill_mode)
            isFirstImage = true
        }

        function showPreloaded()
        {
            contentImage1.showPreloaded();
        }

        function load(url, showtime, fill_mode){
            //load first Time
            console.log("Image::load " + showtime + " " + isFirstImage);
            if (isFirstImage == false && contentImage1.source == ""){
                console.log("ImageContent::init")
                contentImage1.preload(url, showtime)
                contentImage1.showPreloaded()
                isFirstImage = true
                contentImage1.setFillMode(fill_mode)
                return
            }
            if (isFirstImage && contentImage2.source == ""){
                contentImage2.preload(url, showtime)
                contentImage2.setFillMode(fill_mode)
                return
            }
            if (isFirstImage){
                contentImage2.showPreloaded()
                contentImage1.preload(url, showtime)
                isFirstImage = false
                contentImage1.setFillMode(fill_mode)
                return
            }
            else{
                contentImage1.showPreloaded()
                contentImage2.preload(url, showtime)
                contentImage2.setFillMode(fill_mode)
                isFirstImage = true
                return
            }
        }

        function stop(){
            contentImage1.visible = false
            contentImage2.visible = false
            contentImage1.stop()
            contentImage2.stop()
            isFirstImage = false
            imageOffTimer.stop()
        }

        Image {
            id: contentImage1
            visible: false
            property int showtime: 500
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            source: ""
            fillMode: Image.PreserveAspectFit

            function preload(url, showtime_){
                contentImage1.source = url
                console.log("setting up showtime")
                showtime = showtime_
                console.log(showtime)
            }

            function showPreloaded(){
                imageOffTimer.interval = showtime
                imageOffTimer.restart()
                visible = true
                contentImage2.visible = false
            }

            function stop(){
                source = ""
            }
            function setFillMode(fill_mode){
                //console.log("setFillmode image1 " + fill_mode)
                if (fill_mode === "stretch")
                    contentImage1.fillMode = Image.Stretch
                else if (fill_mode === "preserveAspectCrop")
                    contentImage1.fillMode = Image.PreserveAspectCrop
                else
                    contentImage1.fillMode = Image.PreserveAspectFit
            }
        }

        Image {
            id: contentImage2
            visible: false
            property int showtime: 0
            x: parent.x
            y: parent.y
            width: parent.width
            height: parent.height
            source: ""

            fillMode: Image.PreserveAspectFit

            function preload(url, showtime_){
                contentImage2.source = url
                contentImage2.showtime = showtime_
            }

            function showPreloaded(){
                imageOffTimer.interval = showtime
                imageOffTimer.restart()
                visible = true
                contentImage1.visible = false
            }

            function stop(){
                source = ""
            }

            function setFillMode(fill_mode){
                if (fill_mode === "stretch")
                    contentImage2.fillMode = Image.Stretch
                else if (fill_mode === "preserveAspectCrop")
                    contentImage2.fillMode = Image.PreserveAspectCrop
                else
                    contentImage2.fillMode = Image.PreserveAspectFit
            }
        }
    }


    function setRotation(_rotation)
    {
        console.log("set rotation " + _rotation)
        if (_rotation === 180 || _rotation === 0)
        {
            console.log("no need to recalculate boundrect =" + (_rotation % 90))
            //no need to recalculate boundrect
            browser.width = parent.width
            browser.height = parent.height
            browser.x = parent.x
            browser.y = parent.y
            browser.rotation = _rotation
        } else
        {
            console.log("rotation -> redraw" + _rotation)
            browser.width = parent.height
            browser.height = parent.width
            browser.x = parent.x + (parent.width - parent.height)/2;
            browser.y = parent.y - (parent.width - parent.height)/2;
            browser.rotation = _rotation
        }
    }

    Item {
        id: browser
        visible: true
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
        opacity: brightness
        property bool prepareStop: false
        property bool isFirstBrowser: false


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
            prepareStop = false
        }

        Timer {
            id: turnOffTimer
            repeat: false
            onTriggered: {
                console.log("browser::onStop")
                if (nextItemType === "browser"){
                    console.log("next item is browser")
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
                    console.log("next item is video")
                    browser.stopBrowser()
                    videoPlayer.playPreloaded()
                    currentType = "video"
                }
                else if (nextItemType === "image"){
                    console.log("next item is image")
                    browser.stopBrowser()
                    imageContent.showPreloaded()
                    currentType = "image"
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

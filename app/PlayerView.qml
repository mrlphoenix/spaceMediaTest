import QtQuick 2.2
import QtMultimedia 5.5
import QtQuick.Controls 1.1
//import QtWebView 1.1
import QtWebKit 3.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2


Item {
    id: playerViewRoot
    property string areaID: ""
    property double brightness: 1.0
    property double volumeValue: 1.0
    property int delay: 0

    signal askNext(string areaId)
    signal onStop()


    property bool isPlaying: false
    property string currentType: "null"
    property string nextItemType: "--"
    property bool stopAfterPlaying: false

    function prepareStop()
    {
        if (currentType != "null")
            stopAfterPlaying = true
    }

    function setMuted(isMuted){
        mp1.muted = isMuted
        mp2.muted = isMuted
    }

    function skipCurrentItem(){
        if (videoPlayerTimer.isActivated)
            videoPlayerTimer.forceTriggered()
        else if (imageOffTimer.isActivated)
            imageOffTimer.forceTriggered()
        else if (turnOffTimer.isActivated)
            turnOffTimer.forceTriggered()
        delayManager.reset()
    }

    function play(contentId, length, type, skip, fillMode)
    {
        var isAudio = false
        if (type === "audio"){
            isAudio = true
            type = "video"
        }
        stopAfterPlaying = false

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
        console.log("PlayerView::stop")
        //stop and hide everything
        videoPlayer.reset()
        browser.stopBrowser()
        imageContent.stop()
        currentType = "null"
        onStop()
        delayManager.reset()
    }

    Rectangle {
        id: bgRect
        color: "#000000"
        width: parent.width
        height: parent.height
        x: parent.x
        y: parent.y
    }


    Timer {
        id: askNextAntiFlick
        repeat: false
        interval: 500
        onTriggered: {
            askNext(areaID)
        }
    }

    Timer {
        id: nextVideoTimerAF1
        repeat: false
        interval: 310
        onTriggered: {
            mp1.play()
        }
    }

    Timer {
        id: nextVideoTimerAF2
        repeat: false
        interval: 310
        onTriggered: {
            mp2.play()
        }
    }

    Item {
        id: videoPlayer
        visible: currentType != "browser" && currentType != "null"
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

            property bool isActivated: false

            function startTimer(){
                videoPlayerTimer.isActivated = true
                videoPlayerTimer.start()
            }
            function stopTimer(){
                videoPlayerTimer.isActivated = false
                videoPlayerTimer.stop()
            }

            function forceTriggered(){
                videoPlayerTimer.interval = 1
                videoPlayerTimer.restart()
            }

            onTriggered: {
                console.log("VPT::onTriggered")
                videoPlayerTimer.isActivated = false
                if (stopAfterPlaying)
                {
                    console.log("STOP AFTER PLAYING!")
                    stopAfterPlaying = false
                    playerViewRoot.stop()
                    return
                }
                if (nextItemType === "video"){
                    console.log("VPT:video")
                    if (videoPlayer.firstPlayer){
                        videoPlayer.firstPlayer = false
                        if (mp1.isAudio && mp2.isAudio)
                        {
                            console.log("VPT: audio->audio")
                            mp1.stop()
                            nextVideoTimerAF2.restart()
                        }
                        else
                            mp2.play()
                    }
                    else {
                        videoPlayer.firstPlayer = true
                        if (mp1.isAudio && mp2.isAudio)
                        {
                            mp2.stop()
                            nextVideoTimerAF1.restart()
                        }
                        else
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
                askNextAntiFlick.restart()
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
            else {
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
            videoPlayerTimer.stopTimer()
            mp1.stop()
            mp1.source = ""
            mp2.stop()
            mp2.source = ""
            firstPlayer = true
            //currentType = "null"
            audioIcon.visible = false
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
            interval: 300
            onTriggered: {
                 if (showFirstVideo){

                     videoOutput2.opacity = 0.0
                     videoOutput1.opacity = 1.0

                     if(mp1.isAudio)
                         audioIcon.visible = true
                     else
                         audioIcon.visible = false
                 }
                 else{
                     videoOutput1.opacity = 0.0
                     videoOutput2.opacity = 1.0
                     if(mp2.isAudio)
                         audioIcon.visible = true
                     else
                         audioIcon.visible = false
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
                var dur = Math.max(mp1.durationMsecs, 5000) - 250 + delay
                videoPlayerTimer.interval = dur
                videoPlayerTimer.startTimer()
                if (delay != 0)
                    delayManager.run(dur)
                if (mp1.seekMsecs > 0)
                    mp1.seek(mp1.seekMsecs)
                if(isAudio)
                    audioIcon.visible = true
                else
                    audioIcon.visible = false
                mp2.stop()
            }
            volume: volumeValue
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


                var dur = Math.max(mp2.durationMsecs, 5000) - 250 + delay
                videoPlayerTimer.interval = dur
                videoPlayerTimer.startTimer()
                if (delay != 0)
                    delayManager.run(dur)

               // videoPlayerTimer.interval = Math.max(mp2.durationMsecs, 5000) - 250
                //videoPlayerTimer.startTimer()
                if (mp2.seekMsecs > 0)
                    mp2.seek(mp2.seekMsecs)
              /*  if(isAudio)
                    audioIcon.visible = true
                else
                    audioIcon.visible = false*/
                mp1.stop()
            }
            volume: volumeValue
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
                sourceSize.width: Math.min(parent.width, parent.height)
                sourceSize.height: Math.min(parent.height, parent.width)
                width: parent.width/2
                height: parent.height/2
                x: parent.width/2 - width/2
                y: parent.height/2 - height/2
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        color: "#000000"
        visible: contentImage1.visible || contentImage2.visible
    }

    Item {
        id: imageContent
        x: parent.x
        y: parent.y
        width: parent.width
        height: parent.height
        property bool isFirstImage: false

        Timer {
            id: imageOffTimer
            repeat: false
            property bool isActivated: false

            function startTimer(){
                imageOffTimer.isActivated = true
                imageOffTimer.start()
            }

            function stopTimer(){
                imageOffTimer.isActivated = false
                imageOffTimer.stop()
            }

            function restartTimer(){
                imageOffTimer.isActivated = true
                imageOffTimer.restart()
            }

            function forceTriggered(){
                imageOffTimer.interval = 1
                imageOffTimer.restart()
            }

            onTriggered: {
                console.log("image off timer")
                if (stopAfterPlaying)
                {
                    console.log("STOP AFTER PLAYING!")
                    stopAfterPlaying = false
                    playerViewRoot.stop()
                    return
                }
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
                    //imageContent.stop()
                    imageAntiFlickTimer.restart()
                    videoPlayer.playPreloaded()
                    currentType = "video"
                }
                else if (nextItemType === "browser"){
                    console.log("next item is browser")
                    imageContent.stop()
                    browser.showPreloaded()
                    currentType = "browser"
                }
                askNextAntiFlick.restart()
               // askNext(areaID)
            }
        }

        Timer {
            id: imageAntiFlickTimer
            interval: 400
            repeat: false
            onTriggered: {
                imageContent.stop()
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
            imageOffTimer.stopTimer()
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
                //imageOffTimer.interval = showtime
                imageOffTimer.interval = showtime + delay
                imageOffTimer.restartTimer()
                if (delay != 0)
                    delayManager.run(showtime + delay)

                visible = true
                contentImage2.visible = false
            }

            function stop(){
                source = ""
                visible = false
                showtime = 0
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
                //imageOffTimer.interval = showtime
                imageOffTimer.interval = showtime + delay
                imageOffTimer.restartTimer()
                if (delay != 0)
                    delayManager.run(showtime + delay)
                visible = true
                contentImage1.visible = false
            }

            function stop(){
                source = ""
                visible = false
                showtime = 0
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
        //opacity: brightness
        property bool prepareStop: false
        property bool isFirstBrowser: false

        function preload(url, showtime)
        {
            if (sideBrowser1.opacity > 1.05)
            {
                preloadAntiFlickTimer.restart()
            }
            else
            {
                console.log("browser.preload")
                browser.isFirstBrowser = true
                sideBrowser1.preload(url, showtime)
                sideBrowser2.prevUrl = "#none"
                console.log(browser.isFirstBrowser)
                return
            }
        }

        Timer {
            id: preloadAntiFlickTimer
            repeat: false
            interval: 500
            onTriggered: {
                console.log("browser.preload")
                browser.isFirstBrowser = true
                sideBrowser1.preload(url, showtime)
                sideBrowser2.prevUrl = "#none"
                console.log(browser.isFirstBrowser)
                return
            }
        }

        function showPreloaded()
        {
            sideBrowser1.showPreloaded()
        }

        function load(url, showtime){
            //load first time
            console.log("WebBrowser::load");
            console.log("Is first browser: " + isFirstBrowser + "prev url =" + sideBrowser1.prevUrl)
            if ((browser.isFirstBrowser == false && sideBrowser1.prevUrl === "#none") ||
                (sideBrowser1.prevUrl === "#none" && sideBrowser2.prevUrl === "#none")){
                console.log("loading to SB1")
                sideBrowser1.load(url, showtime)
                isFirstBrowser = true
                return
            }

            if (isFirstBrowser && sideBrowser2.prevUrl === "#none"){
                console.log("PREloading to SB2 " + isFirstBrowser +" " + sideBrowser2.prevUrl)
                sideBrowser2.preload(url,showtime)
                return
            }

            if (isFirstBrowser){
                console.log("PREloading to SB1, showing SB2")
                sideBrowser2.showPreloaded()
                sideBrowser1.preload(url, showtime)
                isFirstBrowser = false
                console.log("IFB changed to false" + isFirstBrowser)
                return
            }

            else
            {
                console.log("PREloading to SB2, showing SB1")
                sideBrowser1.showPreloaded()
                sideBrowser2.preload(url, showtime)
                isFirstBrowser = true
                return
            }
        }

        function stopBrowser(){
            console.log("stop browser")
            sideBrowser1.opacity = false
            sideBrowser2.opacity = false
            sideBrowser1.stop()
            sideBrowser2.stop()

            //console.log("IFB changed to false" + isFirstBrowser)
            //isFirstBrowser = false
            turnOffTimer.stopTimer()
            prepareStop = false
        }
        function hideBrowser(){
            console.log("hide browser")

            sideBrowser2.opacity = 0.0
            sideBrowser1.opacity = 0.0
            turnOffTimer.stopTimer()
        }


        Timer {
            id: turnOffTimer
            repeat: false
            property bool isActivated: false

            function startTimer(){
                isActivated = true
                turnOffTimer.start()
            }
            function stopTimer(){
                isActivated = false
                turnOffTimer.stop()
            }
            function restartTimer(){
                isActivated = true
                turnOffTimer.restart()
            }
            function forceTriggered(){
                turnOffTimer.interval = 1
                turnOffTimer.restart()
            }

            onTriggered: {
                console.log("browser::onStop")

                if (stopAfterPlaying)
                {
                    console.log("STOP AFTER PLAYING!")
                    stopAfterPlaying = false
                    playerViewRoot.stop()
                    return
                }

                if (nextItemType === "browser"){
                    console.log("next item is browser")
                    if (sideBrowser1.opacity > 0.00){
                        sideBrowser1.opacity = 0.00
                        sideBrowser2.showPreloaded();
                    }
                    else {
                        sideBrowser2.opacity = 0.00
                        sideBrowser1.showPreloaded()
                    }
                }
                else if (nextItemType === "video"){
                    console.log("next item is video")
                    browserAntiFlickTimer.restart()
                    videoPlayer.playPreloaded()
                    currentType = "video"
                }
                else if (nextItemType === "image"){
                    console.log("next item is image")
                    browser.hideBrowser()
                    imageContent.showPreloaded()
                    currentType = "image"
                }
                askNextAntiFlick.restart()
            }
        }

        Timer {
            id: browserAntiFlickTimer
            repeat: false
            interval: 420
            onTriggered: {
                browser.hideBrowser()
            }
        }

        Timer {
            id: browserTurnOnTimer
            repeat: false
            interval: 400
            property bool isFirstBrowserProp: true
            function runTimer(isFirst){
                isFirstBrowserProp = isFirst
                browserTurnOnTimer.start()
            }

            onTriggered: {
                if (isFirstBrowserProp){
                    sideBrowser2.opacity = 0.00
                }
                else {
                    sideBrowser1.opacity = 0.00
                }
            }
        }

        WebView {
            id: sideBrowser1
            //visible: false
            opacity: 0.00
            anchors.fill: parent
            property string prevUrl:"#none"
            property int showtime: 0
            url: ""

            function preload(_url, _showtime){
                if (sideBrowser1.prevUrl === _url)
                    reload()
                else
                    url = _url
                showtime = _showtime
                console.log("sb1:prev =" + _url)
                sideBrowser1.prevUrl = _url
            }

            function showPreloaded(){
                //turnOffTimer.interval = showtime
                turnOffTimer.interval = showtime + delay
                if (delay != 0)
                    delayManager.run(showtime + delay)
                turnOffTimer.restartTimer()
                browserTurnOnTimer.runTimer(true)
                sideBrowser1.opacity = 1.0
                browserTurnOnTimer.runTimer(true)
                console.log(url)
            }

            function load(_url, _showtime){
                preload(_url, _showtime)
                showPreloaded()
            }

            function stop(){
                console.log("sb1::stop")
                prevUrl = "#none"
                url = ""
            }
        }



        WebView {
            id: sideBrowser2
            //visible: false
            opacity: 0.00
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
                //turnOffTimer.interval = showtime
                turnOffTimer.interval = showtime + delay
                if (delay != 0)
                    delayManager.run(showtime + delay)
                turnOffTimer.restartTimer()
                sideBrowser2.opacity = 1.00
                browserTurnOnTimer.runTimer(false)
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

    Rectangle{
        anchors.fill: parent
        color: "#000000"
        opacity: 1 - brightness
    }

    Item {
        id: delayManager
        width: parent.width
        height: parent.height
        function run(start)
        {
            console.log("DelayManager::run " + start + "/" + delay)
            delayStartTimer.interval = start - delay + 375
            var lonDelay = start + 400
            console.log("DelayEndTimer:: " + lonDelay)
            delayEndTimer.interval = lonDelay
            delayStartTimer.restart()
            resetEndTimer()
        }

        function reset()
        {
            console.log("DelayManager::reset " + delay)
            delayStartTimer.stop()
            delayOnAntiFlickTimer.stop()
            delayEndTimer.stop()
            delayBlackScreenRect.visible = false
        }
        function resetEndTimer()
        {
            delayOnAntiFlickTimer.restart()
            delayEndTimer.restart()
        }

        Timer {
            id: delayOnAntiFlickTimer
            interval: 500
            onTriggered: {
                delayBlackScreenRect.visible = false
            }
        }

        Timer {
            id: delayStartTimer
            repeat: false
            onTriggered: {
                console.log("DelayManager::start")
                delayBlackScreenRect.visible = true
            }
        }

        Timer {
            id: delayEndTimer
            repeat: false
            onTriggered: {
                console.log("DelayManager::end")
                delayBlackScreenRect.visible = false
            }
        }

        Rectangle {
            id: delayBlackScreenRect
            anchors.fill: parent
            color: "#000000"
            visible: false
        }
    }
}

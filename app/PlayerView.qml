import QtQuick 2.2
import QtMultimedia 5.5
import QtQuick.Controls 1.1
import QtWebView 1.1
//import QtWebKit 3.0
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
    property bool isActive: false
    property bool hideWebViews: false

    property string currentType: "null"
    property string nextItemType: "--"
    property bool stopAfterPlaying: false

    property bool browserVisible: false
    property bool isStandartMode: false

    function prepare(name, campaignWidth, campaignHeight, x, y, w, h, _rotation, screenWidth, screenHeight, _opacity)
    {
        console.log("PlayerView::prepare " + name + " " + campaignWidth + " " + campaignHeight + " " + w + " " + h + " " + x + " " + y + "()" + screenWidth +" " + screenHeight)
        playerViewRoot.areaID = name
        playerViewRoot.x = screenWidth * x / campaignWidth
        playerViewRoot.y = screenHeight * y / campaignHeight
        playerViewRoot.width = screenWidth * w / campaignWidth
        playerViewRoot.height = screenHeight * h / campaignHeight
        playerViewRoot.setRotation(_rotation)
        playerViewRoot.visible = true
        playerViewRoot.opacity = _opacity

        sideBrowser1.width = playerViewRoot.width
        sideBrowser1.height = playerViewRoot.height
        sideBrowser2.width = playerViewRoot.width
        sideBrowser2.height = playerViewRoot.height

        isPlaying = true
        isActive = true
    }

    function restore()
    {
        if (isActive)
        {
            sideBrowser1.visible = browser.shouldBeVisibleFirst
            sideBrowser2.visible = browser.shouldBeVisibleSecond
        }
    }

    function prepareStop()
    {
        if (currentType != "null")
            stopAfterPlaying = true
    }

    function setMuted(isMuted){
        vp1.muted = isMuted
        vp2.muted = isMuted
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



/*
    function playStandartMode(contentId, length, type, skip, fillMode)
    {
        var isAudio = false
        playerViewRoot.isStandartMode = true
        if (type === "audio"){
            isAudio = true
            type = "video"
        }
        stopAfterPlaying = false
        if (type === "video")
        {
            currentType = "video"
            videoPlayer.play(contentId, length, skip, isAudio, fillMode)
        }
        else if (type === "html5_online")
        {
            currentType = "browser"
            browser.load(contentId, length)
        }
        else if (type === "image")
        {
            currentType = "image"
            imageContent.load(contentId, length, fillMode)
        }
    }
*/

    Item {
        id: cachedItem

        function reset()
        {
            cachedItem.url = ""
            cachedItem.length = 0
            cachedItem.skip = 0
            cachedItem.type = ""
            cachedItem.fillMode = ""
        }

        function set(_url, _length, _skip, _type, _fillmode)
        {
            console.log("caching video item")
            cachedItem.url = _url
            cachedItem.length = _length
            cachedItem.skip = _skip
            cachedItem.type = _type
            cachedItem.fillMode = _fillmode
        }

        function isActive()
        {
            if (cachedItem.url === "")
                return false;
            else
                return true;
        }

        function isAudio()
        {
            return cachedItem.type === "audio"
        }

        property string url: ""
        property int length: 0
        property int skip: 0
        property string type: ""
        property string fillMode: ""
    }

    function playStandartMode(contentId, length, type, skip, fillMode)
    {
        playerViewRoot.isStandartMode = true

        var isAudio = false
        var originalType = type

        if (type === "audio")
        {
            isAudio = true
            type = "video"
        }
        stopAfterPlaying = false

        if (type === "video")
        {
            if (currentType === "null")
            {
                console.log("playStandart:Video/Null")
                videoPlayer.play(contentId, length, skip, isAudio, fillMode)
                currentType = "video"
                console.log("ask Next: PlayStandart:Video/Null")
                askNext(areaID)
                cachedItem.reset()
            }
            else if (currentType === "video")
            {
                console.log("playStandart:Video/Video")
                cachedItem.set(contentId, length, skip, originalType, fillMode)
                nextItemType = "video"
            }
            else if (currentType === "browser" || currentType === "image")
            {
                console.log("playStandart:Video/Browser-Image")
                nextItemType = "video"
                videoPlayer.preloadItem(contentId, length, skip, isAudio, fillMode)
            }
        }
        else if (type === "html5_online")
        {
            if (currentType === "null")
            {
                console.log("PlayStandart:Browser/Null")
                browser.load(contentId, length)
                currentType = "browser"
                console.log("AskNext:PlayStandart:Browser/Null")
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                console.log("PlayStandart:Browser/Video")
                nextItemType = "browser"
                browser.preload(contentId, length)
            }
            else if (currentType === "browser")
            {
                console.log("PlayStandart:Browser/Browser")
                nextItemType = "browser"
                browser.load(contentId, length)
            }
        }
        else if (type === "image")
        {
            if (currentType === "null")
            {
                console.log("PlayStandart:Image/Null")
                currentType = "image"
                imageContent.load(contentId, length, fillMode)
                console.log("askNext play::image::null")
                askNext(areaID)
            }
            else if (currentType === "video")
            {
                console.log("PlayStandart:Image/Video")
                nextItemType = "image"
                imageContent.preload(contentId, length, fillMode)
            }
            else if (currentType === "browser")
            {
                console.log("PlayStandart:Image/Browser")
                nextItemType = "image"
                imageContent.preload(contentId, length, fillMode)
            }
            else if (currentType === "image")
            {
                console.log("PlayStandart:Image/Image")
                nextItemType = "image"
                imageContent.load(contentId, length, fillMode)
            }
        }
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
        playerViewRoot.opacity = 0.0
        isActive = false
    }

    Rectangle {
        id: bgRect
        color: "#000000"
        width: parent.width
        height: parent.height
        x: 0
        y: 0
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
            vp1.play()
        }
    }

    Timer {
        id: nextVideoTimerAF2
        repeat: false
        interval: 310
        onTriggered: {
            vp2.play()
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


                /*if (isStandartMode)
                {
                    videoPlayer.reset()
                    askNext(areaID)
                    return
                }*/

                if (stopAfterPlaying)
                {
                    console.log("STOP AFTER PLAYING!")
                    stopAfterPlaying = false
                    playerViewRoot.stop()
                    return
                }
                if (nextItemType === "video"){

                    if (isStandartMode)
                    {
                        console.log("VPT:video/StandartMode")
                        videoPlayer.firstPlayer = true
                        vp1.source = ""
                        console.log("VPT:/after clearing source. Now playing")
                        videoPlayer.play(cachedItem.url, cachedItem.length, cachedItem.skip, cachedItem.isAudio(), cachedItem.fillMode)
                        cachedItem.reset()
                    }
                    else {
                        console.log("VPT:video")
                        if (videoPlayer.firstPlayer){
                            videoPlayer.firstPlayer = false
                            if (vp1.isAudio && vp2.isAudio)
                            {
                                console.log("VPT: audio->audio")
                                vp1.stop()
                                nextVideoTimerAF2.restart()
                            }
                            else
                                vp2.play()
                        }
                        else {
                            videoPlayer.firstPlayer = true
                            if (vp1.isAudio && vp2.isAudio)
                            {
                                vp2.stop()
                                nextVideoTimerAF1.restart()
                            }
                            else
                                vp1.play()
                        }
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
            console.log("PlayerView::play")
            if (vp1.source == ""){
                console.log("Filling src < vp1")
                vp1.durationMsecs = _length
                vp1.seekMsecs = _skip
                vp1.source = _url
                vp1.isAudio = _isAudio
                vp1.play()
                if (_isAudio === false)
                    vp1.setFillMode(_fillMode)
            }
            else if (vp2.source == "")
            {
                console.log("Filling src < vp2")
                vp2.source = _url
                vp2.durationMsecs = _length
                vp2.seekMsecs = _skip
                vp2.isAudio = _isAudio
                if (_isAudio === false)
                    vp2.setFillMode(_fillMode)
            }
            else {
                if (firstPlayer){
                    console.log("Load to MP2")
                    vp2.source = _url
                    vp2.durationMsecs = _length
                    vp2.seekMsecs = _skip
                    vp2.isAudio = _isAudio
                    if (_isAudio === false)
                        vp2.setFillMode(_fillMode)
                }
                else{
                    console.log("Load to vp1")
                    vp1.source = _url
                    vp1.durationMsecs = _length
                    vp1.seekMsecs = _skip
                    vp1.isAudio = _isAudio
                    if (_isAudio === false)
                        vp1.setFillMode(_fillMode)
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
            vp1.stop()
            vp1.source = ""
            vp2.stop()
            vp2.source = ""
            firstPlayer = true
            //currentType = "null"
            audioIcon.visible = false
        }

        function preloadItem(_url, length, skip, _isAudio, _fillMode)
        {
            console.log("preload item to vp1")
            vp1.source = _url
            vp1.durationMsecs = length
            vp1.seekMsecs = skip
            vp1.isAudio = _isAudio
            if (_isAudio === false)
                vp1.setFillMode(_fillMode)
        }

        function playPreloaded()
        {
            vp1.play()
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

                     vp2.opacity = 0.0
                     vp1.opacity = 1.0

                     if(vp1.isAudio)
                         audioIcon.visible = true
                     else
                         audioIcon.visible = false
                 }
                 else{
                     vp1.opacity = 0.0
                     vp2.opacity = 1.0
                     if(vp2.isAudio)
                         audioIcon.visible = true
                     else
                         audioIcon.visible = false
                 }
            }
        }

        Video {
            id: vp1
            autoLoad: true
            autoPlay: false
            source: ""
            property int durationMsecs: 0
            property int seekMsecs: 0
            property bool isAudio: false
            
            x: -videoPlayer.x
            y: -videoPlayer.y
            width: videoPlayer.width
            height: videoPlayer.height
            opacity: 0.0
            visible: true
            
            onPlaying:{
                console.log("vp1::onPlay " + vp1.source + " " + volume)
                antiFlickTimerVideo.activateTimer(true)
                var dur = Math.max(vp1.durationMsecs, 5000) - 250 + delay
                videoPlayerTimer.interval = dur
                videoPlayerTimer.startTimer()
                if (delay != 0)
                    delayManager.run(dur)
                if (vp1.seekMsecs > 0)
                    vp1.seek(vp1.seekMsecs)
                if(isAudio)
                    audioIcon.visible = true
                else
                    audioIcon.visible = false
                vp2.stop()
            }
            function setFillMode(fill_mode){
                if (fill_mode === "stretch")
                    vp1.fillMode = VideoOutput.Stretch
                else if (fill_mode === "preserveAspectCrop")
                    vp1.fillMode = VideoOutput.PreserveAspectCrop
                else
                    vp1.fillMode = VideoOutput.PreserveAspectFit
            }
            volume: volumeValue
        }

        Video{
            id: vp2
            autoLoad: true
            autoPlay: false
            source: ""
            property int durationMsecs: 0
            property int seekMsecs: 0
            property bool isAudio: false

            x: -videoPlayer.x
            y: -videoPlayer.y
            width: videoPlayer.width
            height: videoPlayer.height
            opacity: 0.0
            visible: true

            function setFillMode(fill_mode){
                if (fill_mode === "stretch")
                    vp2.fillMode = VideoOutput.Stretch
                else if (fill_mode === "preserveAspectCrop")
                    vp2.fillMode = VideoOutput.PreserveAspectCrop
                else
                    vp2.fillMode = VideoOutput.PreserveAspectFit
            }
            onPlaying: {
                console.log("vp2::onPlay " + vp2.source)
                antiFlickTimerVideo.activateTimer(false)


                var dur = Math.max(vp2.durationMsecs, 5000) - 250 + delay
                videoPlayerTimer.interval = dur
                videoPlayerTimer.startTimer()
                if (delay != 0)
                    delayManager.run(dur)

                if (vp2.seekMsecs > 0)
                    vp2.seek(vp2.seekMsecs)
                if(isAudio)
                    audioIcon.visible = true
                else
                    audioIcon.visible = false
                vp1.stop()
            }
            volume: volumeValue
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
        //visible: contentImage1.visible || contentImage2.visible
        visible: false
    }

    Item {
        id: imageContent
        x: 0
        y: 0
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
                /*if (isStandartMode)
                {
                    imageContent.stop()
                    askNext(areaID)
                    return
                }*/

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

            //browser.width = parent.width
            //browser.height = parent.height
            //browser.x = parent.x
            //rowser.y = parent.y
            browser.x = 0
            browser.y = 0
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
        //x: parent.x
        //y: parent.y
        x: 0
        y: 0
        //opacity: brightness
        property bool prepareStop: false
        property bool isFirstBrowser: false

        property bool shouldBeVisibleFirst: false
        property bool shouldBeVisibleSecond: false

        function showBrowser1(isVisible)
        {
            shouldBeVisibleFirst = isVisible
            if (hideWebViews)
            {
                return
            }
            //if (isVisible === false) return
            console.log("show browser 1 " + isVisible + " shouldBeHidden = " + hideWebViews)
            console.log("is browser visible: " + browserVisible)
            console.log("bw: " + sideBrowser1.visible + " " + parent.visible)

            console.log ("BROWSER SIZE: " + sideBrowser1.width + " " + sideBrowser1.height)
            if (isVisible)
            {
                if (browserVisible)
                    sideBrowser1.opacity = 1.0
                else
                {
                    sideBrowser1.visible = true

                }
                sideBrowser1.update()
            }
            else
            {
                if (browserVisible)
                    sideBrowser1.opacity = 0.0
                else
                    sideBrowser1.visible = false
            }
            sideBrowser1.update()
        }

        function showBrowser2(isVisible)
        {
            shouldBeVisibleSecond = isVisible
            if (hideWebViews)
                return
            //if (isVisible === false) return
            console.log("show browser 2 " + isVisible)
            console.log("BROWSER SIZE: " + sideBrowser2.width + " " + sideBrowser2.height)

            console.log("bw: " + sideBrowser2.visible + " " + parent.visible)
            if (isVisible)
            {
                if (browserVisible)
                    sideBrowser2.opacity = 1.0
                else
                    sideBrowser2.visible = true
            }
            else
            {
                if (browserVisible)
                    sideBrowser2.opacity = 0.0
                else
                    sideBrowser2.visible = false
            }
            sideBrowser2.update()
        }

        function checkBrowserVisible(isFirst)
        {
            if (isFirst)
            {
                if (browserVisible)
                    return sideBrowser1.opacity > 0.0
                else
                    return sideBrowser1.visible
            }
            else
            {
                if (browserVisible)
                    return sideBrowser2.opacity > 0.0
                else
                    return sideBrowser2.visible
            }
        }

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
            showBrowser1(false)
            showBrowser2(false)
            //sideBrowser1.opacity = 0
            //sideBrowser2.opacity = 0
            sideBrowser1.stop()
            sideBrowser2.stop()

            //console.log("IFB changed to false" + isFirstBrowser)
            //isFirstBrowser = false
            turnOffTimer.stopTimer()
            prepareStop = false
        }

        function hideBrowser(){
            console.log("hide browser")

            showBrowser2(false)
            showBrowser1(false)
            //sideBrowser2.opacity = 0.0
            //sideBrowser1.opacity = 0.0
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


                /*if (isStandartMode)
                {
                    browser.stopBrowser()
                    askNext(areaID)
                    return
                }*/

                if (nextItemType === "browser"){
                    console.log("next item is browser")
                    if (browser.checkBrowserVisible(true)){
                        browser.showBrowser1(false)
                        sideBrowser2.showPreloaded()
                    }
                    else {
                        browser.showBrowser2(false)
                        sideBrowser1.showPreloaded()
                    }

                    /*
                    if (sideBrowser1.opacity > 0.00){
                        sideBrowser1.opacity = 0.00
                        sideBrowser2.showPreloaded();
                    }
                    else {
                        sideBrowser2.opacity = 0.00
                        sideBrowser1.showPreloaded()
                    }*/
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
                    browser.showBrowser2(false)
                    //sideBrowser2.opacity = 0.00
                }
                else {
                    browser.showBrowser1(false)
                    //sideBrowser1.opacity = 0.00
                }
            }
        }

        WebView {
            id: sideBrowser1
            //visible: true
            visible: browserVisible
            opacity: 0.00

            x: 0
            y: 0
            width: 1920//playerViewRoot.width
            height: 1080//playerViewRoot.height

            property string prevUrl:"#none"
            property int showtime: 0
            url: ""
            //width: vp1.width
            //height: vp1.height

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
                browser.showBrowser1(true)
                //sideBrowser1.opacity = 1.0
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
            //visible: true
            visible: browserVisible
            opacity: 0.00
            x: 0
            y: 0
            width: 1920//playerViewRoot.width
            height: 1080//playerViewRoot.height

            //anchors.fill: parent
            property string prevUrl:"#none"
            property int showtime: 0
            url: ""

            //width: vp1.width
            //height: vp1.height
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
                browser.showBrowser2(true)
                //sideBrowser2.opacity = 1.00
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

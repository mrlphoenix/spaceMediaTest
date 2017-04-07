import QtQuick 2.2
import QtQuick.Controls 1.1
//import QtWebView 1.1
import QtWebKit 3.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2


Item{
    //anchors.fill: parent

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
        function stop()
        {
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
        function stop()
        {
            prevUrl = "#none"
            url = ""
        }
    }
}

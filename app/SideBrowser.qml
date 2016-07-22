import QtQuick 2.2
import QtQuick.Controls 1.1
import QtWebView 1.1
//import QtWebKit 3.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2


Item{
    anchors.fill: parent

    function load(url, showtime){
        //load first time
        console.log("WebBrowser::load -> reloading page")
        if (isFirstBrowser == false && webView1.prevUrl === "#none"){
            webView1.load(url, showtime)
            isFirstBrowser = true
            next()
            return
        }
        if (isFirstBrowser && webView2.prevUrl === "#none"){
            webView2.preload(url,showtime)
            return
        }
        if (isFirstBrowser){
            webView2.showPreloaded()
            webView1.preload(url, showtime)
            isFirstBrowser = false
            return
        }
        else
        {
            webView1.showPreloaded()
            webView2.preload(url, showtime)
            isFirstBrowser = true
            return
        }
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
        id: webView1
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
            webView1.visible = false
        }
        function load(_url, _showtime){
            preload(_url, _showtime)
            showPreloaded()
        }
    }
    WebView {
        id: webView2
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
            webView1.visible = false
        }
        function load(_url, _showtime){
            preload(_url, _showtime)
            showPreloaded()
        }
    }
}

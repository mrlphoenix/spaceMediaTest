import QtQuick 2.2
import QtQuick.Controls 1.1
import QtWebView 1.1
//import QtWebKit 3.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

/*ApplicationWindow {
    property bool showProgress: webView.loading
                                && Qt.platform.os !== "ios"
                                && Qt.platform.os !== "winphone"
                                && Qt.platform.os !== "winrt"
    visible: true
    x: initialX
    y: initialY
    width: initialWidth
    height: initialHeight
    title: webView.title

    WebView {
        id: webView
        anchors.fill: parent
        url: initialUrl
    }

}
*/
Item{
    anchors.fill: parent
    function load(url){
        webView.url = url
    }
    function setShowTime(msecs){
        turnOffTimer.interval = msecs
        console.log("browser show time = " + msecs)
    }
    function startShow(){
        turnOffTimer.start()
    }

    property bool isActive: false

    signal loadFinished()
    signal endShow()

    Timer {
        id: turnOffTimer
        repeat: false
        onTriggered: {
            endShow()
            isActive = false
        }
    }

    WebView {
        id: webView
        visible: true
        anchors.fill: parent
        url: "http://ya.ru"
        //enabled: false
        onLoadingChanged: {
            if (loadRequest === WebView.LoadSucceededStatus){
                loadFinished()
            }
        }
    }
}

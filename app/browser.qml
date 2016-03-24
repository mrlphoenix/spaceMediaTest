
import QtQuick 2.2
import QtQuick.Controls 1.1
//import QtWebView 1.0
import QtWebKit 3.0
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

}*/

Item{
    id: root
    anchors.fill: parent
    visible: true
    WebView {
        id: webView
        anchors.fill: parent
        url: "http://html5-banner.ru/"
    }
}

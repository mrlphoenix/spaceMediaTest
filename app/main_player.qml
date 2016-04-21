import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.2




Item {
    id: item
    property bool invokeNext:true
    property double videoOutBrightness: 1.0
    property double hValue: Math.min(height, width)
    property double aspect: Math.min(hValue / 1080.0, 1.0)
    property int heightP: height
    property int widthP: width
    property int decreasingTextValue: 0

    //properties for seamless video player
    property bool preloader: false
    property bool useSecondPlayer: false


    /*
      1. play item1 -> mp1
      2. preload item2 -> mp2 | preload = true; useSecondPlayer = false
      3. play item3 => |preload = true|useSecondPlayer=false| -> {mp1->visible = false; mp2->visible = true; mp2->play; mp1->load item3; useSecondPlayer = true;}
      */
    signal nextItem()
    signal refreshId()
    signal gpsChanged(double lat, double lgt)
    focus: true
    onHeightChanged: {
        heightP = height
        widthP = width
        //console.log("native height " + height + "heightp " + heightP)
        //console.log("native width" + width + "widthp" + width) // 2048
    }



    function displayTrafficInfo(trafficIn, trafficOut, memory, cpuLoad)
    {
        console.log("cpuload=", cpuLoad)
        trafficDebugDisplay.text = "In: " + trafficIn + " Out: " + trafficOut + " memory: " + memory + "% cpuload: " + cpuLoad
    }

    function enablePreloading(filename)
    {
        console.log("Preloading: MP2->: " +filename)
        videoPlayer.playItem(filename)
    }

    function playFile(filename){
        console.debug("playfile is called " + filename)
        videoPlayer.playItem(filename)
    }

    function hideVideoLayer(object)
    {
        object.visible = false
    }

    function downloadComplete(){
        console.debug("download complete. Hiding Progress Bars, Showing Video Player");
        filesProgressBar.visible = false
        downloadProgressBar.visible = false
        videoPlayer.visible = true
        playerName.visible = false
        bgLogoImage.visible = false
        logoColumn.visible = false
        videoPlayer.opacity = videoOutBrightness
    }
    function setPlayerName(playerId){
        playerName.text = playerId
    }
    function setTotalProgress(p, name){
        filesProgressBar.value = p
        playerName.text = name
    }
    function setProgress(p){
        downloadProgressBar.value = p
    }
    function showVideo(isVisible){
        videoPlayer.visible = isVisible
        logoColumn.visible = false
        if (!isVisible)
        {
            overlayBgRect.color = "#333e47"
        }
        else if (videoOutBrightness > 1.0)
        {
            overlayBgRect.color = "#FFFFFF"
        }
        else if (videoOutBrightness < 0.99){
            overlayBgRect.color = "#000000"
        }
        else{
            overlayBgRect.color = "#333e47"
        }
    }
    function setBrightness(value){
        videoOutBrightness = value
        if (value > 1.0) {
            overlayBgRect.color = "#FFFFFF"
            videoPlayer.opacity = 2.0 - value
        }
        else {
            overlayBgRect.color = "#000000"
            videoPlayer.opacity = value
        }
    }

    function setNoItemsLogo(link){
        logoColumn.visible = true
        videoPlayer.opacity = 0
        titleText.text = "No playlist available"
        progressText.text = "Go to <a href=\"" + link + "\">" + link +  "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDItem.visible = false
        waitingBlock.visible = false
        waitingText.visible = false
        waitingRefreshInText.visible = false
    }
    function setDownloadLogo(){
        logoColumn.visible = true
        videoPlayer.opacity = 0
        titleText.text = "Downloading playlist"
        progressText.text = "Please wait..."
        logoDownloadProgressBar.visible = true
        playerIDItem.visible = false
        waitingBlock.visible = false
        waitingText.visible = false
        waitingRefreshInText.visible = false
    }
    function setNeedActivationLogo(link, playerID, updateDelay){
        logoColumn.visible = true
        videoPlayer.opacity = 0
        titleText.text = "To setup this player use the following code at"
        progressText.text = "<a href=\"" + link + "\">" + link + "</a></html>"
        logoDownloadProgressBar.visible = false
        playerIDText.text = playerID

        playerIDItem.visible = true
      //  refreshPlayerID.sourceSize.width = playerIDRect.height
       // refreshPlayerID.sourceSize.height = playerIDRect.height
       // refreshPlayerID.visible = true
        waitingBlock.visible = true
       // refreshButtonDeactivator.start()
        playerIDText.color = "#333e47"
   //     refreshTimeTimer.updateDelay = updateDelay - 1
   //     refreshTimeTimer.start()
    }
    function getPointSize(text){
        if (text.length > 40)
            decreasingTextValue = text.length-40
        else
            decreasingTextValue = 0
        if (item.widthP > item.heightP)
            return 32 - decreasingTextValue
        else
        {
            if (decreasingTextValue)
                decreasingTextValue += 2
            testText.text = text
            if (testText.width < item.widthP)
                decreasingTextValue -= 4
            return 32 - decreasingTextValue
        }
    }
    function getPointSizeSecond(text){
        if (item.widthP > item.heightP)
            return 20
        else
            return 18
    }

    function getRefreshButtonPositionY(w, h)
    {
        if (w > h)
        {
            return playerIDRect.y
        }
        else
        {
            return playerIDRect.y + playerIDRect.height + 8
        }
    }

    function setDownloadProgressSimple(value)
    {
        logoDownloadProgressBar.value = value
    }
    function getTopValue(h)
    {
        console.log("y: " + waitingRefreshInText.y)
     //   if (waitingRefreshInText.y + waitingRefreshInText.height > h)
        console.log("DIFFS: " + (waitingRefreshInText.y + waitingRefreshInText.height - h))
        console.log("BASIC: " + (h * 0.248148148 * aspect))
        console.log("RETURN: " + (h * 0.248148148 * aspect - (waitingRefreshInText.y + h * 0.248148148 + waitingRefreshInText.height - h)))
        var result = h * 0.248148148 - Math.max(waitingRefreshInText.y + h * 0.248148148 + waitingRefreshInText.height - h,0)*1.5
        if (result < 0)
            return h * 0.248148148 * aspect
        else
            return result
    //    return parent.height * 0.248148148 * aspect
    }

    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            console.log("Coordinate:", coord.longitude, coord.latitude);
            gpsChanged(coord.latitude, coord.longitude)
            coordsDisplay.text = "Coords: lat = " + coord.latitude + ", lng = " + coord.longitude
        }
    }
    Timer{
        property var object: ({})
        id: hideVideoDelayTimer
        interval: 50
        repeat: false
        onTriggered: {
            object.visible = false
        }
    }

    Timer {
        property int updateDelay: 10
        id: refreshTimeTimer
        interval: 1000
        repeat: true
        running: true
        onTriggered:{
            updateDelay = updateDelay-1;
            if (updateDelay == -1)
                updateDelay = 10;
          //  console.log("refreshTimer triggered" + updateDelay.toString())

           /* if (refreshplayerIDAnimation.running)
                waitingRefreshInText.text = "Refreshing"
            else*/
            if (updateDelay != 0)
                waitingRefreshInText.text = "Refresh in " + updateDelay + "s"
            else
                waitingRefreshInText.text = "Refreshing..."
        }
    }

    Timer {
        id: dialogCloseTimer
        repeat: false
        interval: 10000
        onTriggered: {
            dialogAndroid.close()
            item.focus = true
        }
    }

    Dialog {
        id: dialogAndroid
      //  width: 600  // for desktop ::TODO
       // height: 500 // for desktop ::TODO

        // Создаём содержимое диалогового окна
        contentItem: Rectangle {
            width: 600
            height: 500
            color: "#333e47"

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: dividerHorizontal.top
                color: "#333e47"

                Label {
                    id: textLabel
                    text: qsTr("Are you sure?")
                    color: "#00cdc1"
                    anchors.centerIn: parent
                }
            }

            // Создаём горизонтальный разделитель с помощью Rectangle</center> <p/> <center
            Rectangle {
                id: dividerHorizontal
                color: "#d7d7d7"
                height: 2
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: row.top
            }

            Row {
                id: row
                height: 100
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right

                Button {
                    id: dialogButtonCancel
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.width / 2 - 1

                    style: ButtonStyle {
                        background: Rectangle {
                            color: control.pressed ? "#d7d7d7" : "#333e47"
                            border.width: 0
                        }

                        label: Text {
                            text: qsTr("Cancel")
                            color: "#00cdc1"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                    onClicked: {
                        dialogAndroid.close()
                        item.focus = true
                    }
                }

                Rectangle {
                    id: dividerVertical
                    width: 2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    color: "#d7d7d7"
                }

                Button {
                    id: dialogButtonOk
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.width / 2 - 1

                    style: ButtonStyle {
                        background: Rectangle {
                            color: control.pressed ? "#d7d7d7" : "#333e47"
                            border.width: 0
                        }

                        label: Text {
                            text: qsTr("Ok")
                            color: "#00cdc1"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                    Keys.onReleased: {
                        console.log("DIALOG ON RELEASED!!!")
                        if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                            dialogAndroid.close()
                            item.focus = true
                            event.accepted = true
                        }
                    }
                    onClicked: Qt.quit()
                }
            }
        }
    }

    FontLoader {
        id: ubuntuFont
        source: "ubuntu-m.ttf"
    }

    Rectangle {
        id: overlayBgRect
        color: "#333e47"
        width: parent.width
        height: parent.height
    }

    Image {
        id: bgLogoImage
        source: "logo_bg.svg"
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        width: parent.width
        height: parent.height
      //  fillMode: Image.Stretch
       // fillMode: Image.Tile
       // horizontalAlignment: Image.AlignLeft
      //  verticalAlignment: Image.AlignTop
    }
    Image {
        id: bgLogoImageV
        visible: parent.width < parent.height
        source: "logo_bg_vertical.svg"
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        width: parent.width
        height: parent.height
       // fillMode: Image.Stretch
      //  horizontalAlignment: Image.AlignLeft
      //  verticalAlignment: Image.AlignTop
    }
    Item
    {
        id: logoColumn
        width: parent.width
        //y: parent.height * 0.248148148 * aspect
        y: getTopValue(parent.height)
     //   y: 0
        Text{
            id: testText
            font.family: ubuntuFont.name
            font.pointSize: 26
            visible: false
            text: "kek"
        }

        Image {
            id: teledsLogoImg
            source: "logo.svg"
            sourceSize.width: 195.0 * aspect
            sourceSize.height: 156.0 * aspect
            x: parent.width/2 - width/2
        }
        Image {
            id: teledsLogo
            y: teledsLogoImg.y + teledsLogoImg.height + (22.0 * aspect)
            x: parent.width/2 - width/2
            source: "logo_teleds.png"

            scale: aspect
        }

        Rectangle {
            id: brRect
            width: 124 * aspect
            height: 8 * aspect
            y: teledsLogo.y + teledsLogo.height + (75.0 * aspect)
            x: parent.width/2 - width/2
            color: "#00cdc1"
        }
        Text {
            id: titleText
            font.family: ubuntuFont.name
            font.pointSize: getPointSize(text)
            text: "Initialization"
         //   x: parent.width/2 - width/2
            y: brRect.y + brRect.height + (75.0 * aspect)
            color: "white"
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }
        Text {
            id: progressText
            font.family: ubuntuFont.name
            font.pointSize: getPointSize(text)
            text: "..."
            x: parent.width/2 - width/2
            y: titleText.y + titleText.height + 18 * aspect
            color: "#00cdc1"
            linkColor: "#00cdc1"
            onLinkActivated: Qt.openUrlExternally(link)
        }
        ProgressBar{
            id: logoDownloadProgressBar
            visible: false
            value: 0.0
            y: progressText.y + progressText.height + 110 * aspect
            width: parent.width
            style: ProgressBarStyle {
                    background: Rectangle {
                        color: "transparent"
                        border.color: "black"
                        border.width: 1
                        implicitHeight: 68 * aspect
                    }
                    progress: Rectangle {
                        color: "#00cdc1"
                        border.color: "#00cdc1"
                    }
                }
        }


        Item{
            id: playerIDItem
            width: parent.width
            height: parent.height
            visible: false
            Rectangle {
                id: playerIDRect
                color: "#00cdc1"
                Text {
                    id: playerIDText
                    text: ""
                    color: "#333e47"
                    font.family: ubuntuFont.name
                    font.pointSize: 38
                }
                width: childrenRect.width
                height: childrenRect.height
                y: progressText.y + progressText.height + 55 * aspect
                x: parent.width/2 - width/2
            }
        }

        Item{
            id: waitingBlock
            width: parent.width
            visible: false
            Text {
                y: playerIDRect.y + playerIDRect.height + 55 * aspect
                x: item.width/2 - width/2
            //    anchors.top: parent.top
            //    anchors.left: refreshPlayerID.right
                id: waitingText

                font.family: ubuntuFont.name
                font.pointSize: 20
                text: "Waiting for activation"
                color: "#00cdc1"
            }
            Text {
                y: waitingText.y + waitingText.height
                x: item.width/2 - width/2
                id: waitingRefreshInText
                font.family: ubuntuFont.name
                font.pointSize: 20
                text: "Refresh in 10s"
                color: "#00cdc1"
            }
        }
    }
    //
    //

    Column
    {
        visible: false
        width: parent.width
        Label{
            id: playerName
            width: parent.width
            font.pointSize: 48
        }

        ProgressBar{
            id: filesProgressBar
            value: 0.0
            width: parent.width
            style: ProgressBarStyle {
                    background: Rectangle {
                        radius: 2
                        color: "lightgray"
                        border.color: "gray"
                        border.width: 1
                        implicitHeight: 24
                    }
                    progress: Rectangle {
                        color: "lightsteelblue"
                        border.color: "steelblue"
                    }
                }
        }
        ProgressBar{
            id: downloadProgressBar
            value: 0.0
            width: parent.width
            style: ProgressBarStyle {
                       background: Rectangle {
                           radius: 2
                           color: "lightgray"
                           border.color: "gray"
                           border.width: 1
                           implicitHeight: 24
                       }
                       progress: Rectangle {
                           color: "lightsteelblue"
                           border.color: "steelblue"
                       }
                   }
        }
    }

    VideoPlayer {
        id: videoPlayer
        onNext: {
            nextItem()
        }
    }

    Text{
        id: coordsDisplay
        text: "Coords: "
        y: 0
        x: parent.width - coordsDisplay.width
    }
    Text{
        id: trafficDebugDisplay
        text: "in: out:"
        y: coordsDisplay.height
        x: parent.width - trafficDebugDisplay.width
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
            dialogCloseTimer.start()
            dialogAndroid.open()
            dialogButtonOk.focus = true
            event.accepted = true
            console.log("back key pressed: main")
        }
    }
}

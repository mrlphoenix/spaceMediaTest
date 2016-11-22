import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    //visible: false
    width: parent.width
    height: parent.height
    property color color1: "black"
    property color color2: "white"
    property url logo: ""
    property int screenWidth: 600
    property int screenHeight: 400

    property string eula: ""
    property string privacyPolicy: ""
    property string openSource: ""
    property string legal: ""

    property string deviceName: ""
    property string deviceConnection: ""
    property string deviceVersion: ""

    property bool showTeleDSLogo: true

    signal playerClicked()
    signal exitClicked()

    function getBoundaryWidth(){
        if (Screen.width > Screen.height){
            return screenWidth * 0.325;
        }
        else
        {
            return Screen.width * 0.55
        }
    }
    function fontSizeSP(coef)
    {
        var decreaseCoef = 580.0
        var screenSizeMin = Math.min(Screen.width, Screen.height)
        if (screenSizeMin > 820)
        {
            if (coef < 14)
                decreaseCoef = 430.0
            else
                decreaseCoef = 500.0
        }
       // var totalCoef = Math.min(Screen.width, Screen.height) * coef /  decreaseCoef
       // console.log("fontSizeSP>> height = " + screenSizeMin + " coef =" + coef + " result = " + totalCoef)
        return Math.min(Screen.width, Screen.height) * coef / decreaseCoef
    }
    function percentPixels(coef)
    {
        return Math.min(Screen.width, Screen.height) * coef
    }

    function leftPercentPixels(coef)
    {
        return Screen.width * coef
    }
    function fontBrakeLength()
    {
        if (Screen.width > Screen.height)
            return Screen.width * 0.45
        else
            return Screen.width * 0.9
    }

    function restore()
    {
        reinit()
        visible = true
    }
    function reinit()
    {
        playerTabRect.visible = false
        aboutTabRect.visible = true
        exitTabRect.visible = false
    }

    function setTileMode()
    {
        menuBgTiledImage.visible = true
        menuBgImage.visible = false
    }
    function setDefaultMode()
    {
        menuBgImage.visible = true
        menuBgTiledImage.visible = false
    }
    function getLogoTextOpacity()
    {
        if (showTeleDSLogo)
            return 1.0
        else
            return 0.0
    }
    function getMenuTitleText()
    {
        if (showTeleDSLogo)
            return "About TeleDS"
        else
            return "About"
    }
    function getMenuVersionText()
    {
        if (showTeleDSLogo)
            return "You are running TeleDS Player Version "
        else
            return "You are running Player Version "
    }

    Image {
        id: menuBgImage
        source: brand_menu_background
        fillMode: Image.PreserveAspectCrop
        clip: true
        anchors.fill: parent
    }
    Image {
        id: menuBgTiledImage
        source: brand_menu_background
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop
        anchors.fill: parent
        visible: false
    }

    Image {
        x: leftPercentPixels(0.05)
        y: percentPixels(0.038888889)
        id: menuLogoImage
        source: logo
        sourceSize.height: menuLogoText.height*1.8
        height: menuLogoText.height*1.8
    }
    Text {
        id: menuLogoText
        font.family: ubuntuFont.name
        font.pixelSize: fontSizeSP(33.3)
        text: "<b>TeleDS</b>"
        color: color1
        anchors.verticalCenter: menuLogoImage.Center
        anchors.left: menuLogoImage.right
        y: menuLogoImage.y + (menuLogoImage.height-height)/2
        opacity: getLogoTextOpacity()
    }

    RowLayout{
        id: menuTabsBlock
        width: getBoundaryWidth()
        height: menuLogoText.height
        x: menuLogoText.x + menuLogoText.width + leftPercentPixels(0.0625)
        y: menuLogoText.y + menuLogoText.height/12

        Item {
            width: childrenRect.width
            height: childrenRect.height
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    playerTabRect.visible = true
                    aboutTabRect.visible = false
                    exitTabRect.visible = false
                    playerClicked()
                }
            }

            Text {
                id: playerTabText
                font.family: ubuntuFont.name
                text: "<b>PLAYER</b>"
                color: color1

                font.pixelSize: fontSizeSP(11.1)
                //anchors.verticalCenter: parent
                x: (playerTabRect.width - width) /2
            }
            Rectangle {
                id: playerTabRect
                color: color2
                y: playerTabText.y + playerTabText.height + 5
                width: Math.max(playerTabText.width + 4, 70)
                height: 5
                radius: 2
                visible: false
            }
        }

        Item {
            width: childrenRect.width
            height: childrenRect.height
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    playerTabRect.visible = false
                    aboutTabRect.visible = true
                    exitTabRect.visible = false
                }
            }
            Text {
                id: aboutTabText
                font.family: ubuntuFont.name
                text: "<b>ABOUT</b>"
                color: color1

                font.pixelSize: fontSizeSP(11.1)
                //anchors.verticalCenter: parent
                x: (aboutTabRect.width - width) /2
            }
            Rectangle {
                id: aboutTabRect
                color: color2
                y: aboutTabText.y + aboutTabText.height + 5
                width: Math.max(aboutTabText.width + 4, 70)
                height: 5
                radius: 2
            }
        }

        Item {
            width: childrenRect.width
            height: childrenRect.height
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    playerTabRect.visible = false
                    aboutTabRect.visible = false
                    exitTabRect.visible = true
                    exitClicked()
                }
            }
            Text {
                id: exitTabText
                font.family: ubuntuFont.name
                text: "<b>CLOSE</b>"
                color: color1

                font.pixelSize: fontSizeSP(11.1)
                //anchors.verticalCenter: parent
                x: (exitTabRect.width - width) /2
            }
            Rectangle {
                id: exitTabRect
                color: color2
                y: exitTabText.y + exitTabText.height + 5
                width: Math.max(exitTabText.width + 4, 70)
                height: 5
                radius: 2
                visible: false
            }
        }
    }

    Item
    {
        anchors.top: menuTabsBlock.bottom
        anchors.topMargin: menuLogoImage.height*1.5
        id: menuTitleBlock
        height: (menuTitleBlockText.height + 5)*1.2
        width: menuTitleBlockText.width + 4
        Text{
            id: menuTitleBlockText
            text: getMenuTitleText()
            font.family: ubuntuFont.name
            color: color1
            font.pixelSize: fontSizeSP(27.75)
        }
        x: leftPercentPixels(0.05)
        Rectangle {
            id: menuTitleBlockTextRectangle
            color: color2
            anchors.bottom: parent.bottom
            width: parent.width
            height: 10
            radius: 4
        }
    }

    Text {
        id: menuTitleBlockVersionText
        text: getMenuVersionText() + deviceVersion
        color: color1
        font.pixelSize: fontSizeSP(14.8)
        font.weight: Font.Light

        anchors.top: menuTitleBlock.bottom
        anchors.topMargin: menuTitleBlockVersionText.height
        x: leftPercentPixels(0.05)
    }

    Item {
        id: menuInfoDeviceNameBlock
        anchors.top: menuTitleBlockVersionText.bottom
        anchors.topMargin: menuTitleBlockVersionText.height * 1.33
        x: leftPercentPixels(0.05)
        Text {
            id: menuInfoDeviceNameBlockTitle
            text: "Device Name"
         //   font.family:  ubuntuFont.name
            color: color1
            font.pixelSize: fontSizeSP(14.8)
            font.weight: Font.Light
        }
        Text {
            id: menuInfoDeviceNameBlockData
            text: deviceName
           // font.family:  ubuntuFont.name
            color: color1
            font.pixelSize: fontSizeSP(14.8)
            x: Math.max(menuTitleBlockTextRectangle.x + menuTitleBlockTextRectangle.width, menuInfoDeviceConnectionInfoTitle.x + menuInfoDeviceConnectionInfoTitle.width + leftPercentPixels(0.01))
        }
        Rectangle {
            id: menuInfoDeviceNameBlockLine
            color: "white"
            opacity: 0.4
            width: fontBrakeLength()
            height: 1
            anchors.top: menuInfoDeviceNameBlockTitle.bottom
            anchors.topMargin: 12
        }

        Text {
            id: menuInfoDeviceConnectionInfoTitle
            text: "Connection status"
         //   font.family:  ubuntuFont.name
            color: color1
            font.pixelSize: fontSizeSP(14.8)
            font.weight: Font.Light
            anchors.top: menuInfoDeviceNameBlockLine.bottom
            anchors.topMargin: 12
        }
        Text {
            id: menuInfoDeviceConnectionInfoData
            text: deviceConnection
           // font.family:  ubuntuFont.name
            color: color1
            font.pixelSize: fontSizeSP(14.8)
            x: Math.max(menuTitleBlockTextRectangle.x + menuTitleBlockTextRectangle.width, menuInfoDeviceConnectionInfoTitle.x + menuInfoDeviceConnectionInfoTitle.width + leftPercentPixels(0.01))
            anchors.top: menuInfoDeviceConnectionInfoTitle.top
        }
        Rectangle {
            id: menuInfoDeviceConnectionInfoLine
            color: "white"
            opacity: 0.4
            width: fontBrakeLength()
            height: 1
            anchors.top: menuInfoDeviceConnectionInfoTitle.bottom
            anchors.topMargin: 12
        }
    }

    Text {
        id: menuTeleDSCopyright
        text: "© 2016 TeleDS"
        color: color1
        font.pixelSize: fontSizeSP(11.7)
        font.weight: Font.Light
        x: leftPercentPixels(0.05)
        y: parent.height*0.89
    }

    RowLayout{
        id: menuFooterItems
        x: leftPercentPixels(0.05)
        anchors.top: menuTeleDSCopyright.bottom
        anchors.topMargin: 12
        width: leftPercentPixels(0.4)

        Text {
            id: menuEULAText
            text: "EULA"
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    restoreTextDialog(eula, "End User License Agreement")
                }
            }
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: " "
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: "|"
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: " "
        }

        Text {
            id: menuPrivacyPolicy
            text: "Privacy policy"
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    restoreTextDialog(privacyPolicy, "Privacy policy")
                }
            }
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: " "
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: "|"
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: " "
        }
        Text {
            id: menuOpenSource
            text: "Open Source"
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    restoreTextDialog(openSource, "Open source")
                }
            }
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: " "
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: "|"
        }
        Text {
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            text: " "
        }
        Text {
            id: menuLegal
            text: "Legal"
            color: color1
            font.pixelSize: fontSizeSP(11.7)
            font.weight: Font.Light
            MouseArea{
                width: parent.width
                height: parent.height
                onClicked: {
                    restoreTextDialog(legal, "Legal")
                }
            }
        }
    }

    function restoreTextDialog(text, title){
        showTextDialogTitle.text = title
        dialogContentText.text = text
        showTextDialog.open()
        showTextDialogCloseButton.focus = true
    }

    Dialog{
        id: showTextDialog
        contentItem: Item{
            width: Screen.width * 0.85
            height: Screen.height * 0.85
            Rectangle {
                color: brand_default_backgroundColor
                width: parent.width
                height: parent.height
                radius: 10
                border.color: brand_default_borderGrayColor
                border.width: 1
            }
            Text {
                id: showTextDialogTitle
                color: color2
                text: ""
                font.pointSize: 18
                y: 10
                x: (parent.width - width)/2
            }

            TextArea {
                id: dialogContentText
                text: ""
                readOnly: true
                textFormat: TextEdit.RichText
                width: parent.width
                height: parent.height*0.9 - showTextDialogTitle.height - 10
                textColor: color1
                backgroundVisible: false
                anchors.top: showTextDialogTitle.bottom
                textMargin: parent.width * 0.05
                font.pixelSize: fontSize(9)
            }
            Button {
                id: showTextDialogCloseButton
                width: parent.width
                height: parent.height*0.1 - 10
                anchors.top: dialogContentText.bottom
                anchors.topMargin: 10
                style: ButtonStyle {
                    background: Rectangle {
                        color: control.pressed ? brand_borderGrayColor : "#333e47"
                        border.width: 1
                        border.color: brand_default_borderGrayColor
                        radius: 10
                    }

                    label: Text {
                        text: qsTr("Close")
                        color: color2
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                Keys.onReleased: {
                    console.log("TEXTDIALOG BU ON RELEASED!!!")
                    if (event.key === Qt.Key_Back || event.key === Qt.Key_Q) {
                        showTextDialog.close()
                        item.focus = true
                        event.accepted = true
                    }
                }
                onClicked: {
                    showTextDialog.close()
                    item.focus = true
                }
            }
        }
    }
}

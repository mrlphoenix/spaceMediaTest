import QtQuick 2.0
import QtQuick.Layouts 1.1

Item {
    //visible: false
    width: parent.width
    height: parent.height
    property color color1: "black"
    property color color2: "white"
    property url logo: ""

    function getBoundaryWidth(){
        if (parent.width > parent.height)
            return parent.width / 1.6;
        else
            return parent.width - 96;
    }
    Image {
        id: menuBgImage
        source: brand_menu_bg_image
        fillMode: Image.PreserveAspectCrop
        clip: true
        anchors.fill: parent
    }
    Rectangle {
        id: grayRect
        anchors.fill: parent
        color: "#333e47"
        opacity: 0.92
    }

    RowLayout{
        width: getBoundaryWidth()
        height: exitToPlayerContent.height
        x: 48
        y: 64

        Image {
            id: menuLogoImage
            source: logo
            sourceSize.width: menuLogoText.height*1.2
            sourceSize.height: menuLogoText.height*1.2
            width: menuLogoText.height*1.2
            height: menuLogoText.height*1.2
        }
        Item{
            id: menuLogoTextItem
            width: Math.max(menuLogoText.width + 4, 70)
        Text {
            id: menuLogoText
            font.family: ubuntuFont.name
            font.pointSize: 36
            text: "<b>TeleDS</b>"
            color: color2
            anchors.centerIn: parent
        }
        anchors.left: menuLogoImage.right
        }

        Item{
            id: exitToPlayerTab
            width: exitToPlayerContent.width
            height: exitToPlayerContent.height
            MouseArea {
                width: parent.width
                height: parent.height
                onClicked: {

                    closePlayerActiveRect.visible = false
                    aboutPlayerActiveRect.visible = false
                    exitToPlayerActiveRect.visible = true
                }
            }

            Rectangle{
                id: exitToPlayerContent
                color: "transparent"
                width: Math.max(exitToPlayerTabText.width + 4, 70)
                height: (exitToPlayerTabText.height + 5)*1.6
                Text {
                    id: exitToPlayerTabText
                    font.family: ubuntuFont.name
                    text: "<b>PLAYER</b>"
                    color: color2

                    font.pointSize: 12.5
                    anchors.centerIn: parent
                }
                Rectangle {
                    id: exitToPlayerActiveRect
                    color: color2
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 5
                    radius: 2
                    visible: false
                }
            }
        }

        Item{
            id: aboutPlayerTab
            width: aboutPlayerContent.width
            height: aboutPlayerContent.height
            //anchors.left: exitToPlayerTab.right
            MouseArea {
                width: parent.width
                height: parent.height
                onClicked: {

                    closePlayerActiveRect.visible = false
                    aboutPlayerActiveRect.visible = true
                    exitToPlayerActiveRect.visible = false
                }
            }

            Rectangle{
                id: aboutPlayerContent
                color: "transparent"
                width: Math.max(aboutPlayerTabText.width + 4, 70)
                height: (aboutPlayerTabText.height + 5)*1.6
                Text {
                    id: aboutPlayerTabText
                    font.family: ubuntuFont.name
                    text: "<b>ABOUT</b>"
                    color: color2
                    font.pointSize: 12.5
                    anchors.centerIn: parent
                }
                Rectangle {
                    id: aboutPlayerActiveRect
                    color: color2
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 5
                    radius: 2
                }
            }
        }

        Item{
            id: closePlayerTab
            width: closePlayerContent.width
            height: closePlayerContent.height
           // anchors.left: aboutPlayerTab.right
            MouseArea {
                width: parent.width
                height: parent.height
                onClicked: {
                    closePlayerActiveRect.visible = true
                    aboutPlayerActiveRect.visible = false
                    exitToPlayerActiveRect.visible = false
                }
            }

            Rectangle{
                id: closePlayerContent
                color: "transparent"
                width: Math.max(closePlayerTabText.width + 4, 70)
                height: (closePlayerTabText.height + 5)*1.6
                Text {
                    id: closePlayerTabText
                    font.family: ubuntuFont.name
                    text: "<b>CLOSE</b>"
                    font.pointSize: 12.5
                    color: color2
                    anchors.centerIn: parent
                }
                Rectangle {
                    id: closePlayerActiveRect
                    color: color2
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 5
                    radius: 2
                    visible: false
                }
            }
        }
    }
}

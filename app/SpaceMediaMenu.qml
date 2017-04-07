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

    function invokeMenuDisplayRotationSelected()
    {
        hdmiGroupItem.isActivated = false
        hdmiModeItem.isActivated = false
        hdmiDriveItem.isActivated = false
        hdmiBoostItem.isActivated = false
        displayRotationItem.isActivated = true
        wifiNetworkItem.isActivated = false
    }

    function invokeMenuDisplayRotationChanged(value, text)
    {
        displayRotationText.text = text
    }

    function invokeMenuHDMIGroupSelected()
    {
        hdmiGroupItem.isActivated = true
        hdmiModeItem.isActivated = false
        hdmiDriveItem.isActivated = false
        hdmiBoostItem.isActivated = false
        displayRotationItem.isActivated = false
        loadSettingsItem.isActivated = false
    }

    function invokeMenuHDMIGroupChanged(value, text)
    {
        hdmiGroupText.text = text
    }

    function invokeMenuHDMIModeSelected()
    {
        hdmiGroupItem.isActivated = false
        hdmiModeItem.isActivated = true
        hdmiDriveItem.isActivated = false
        hdmiBoostItem.isActivated = false
        displayRotationItem.isActivated = false
    }

    function invokeMenuHDMIModeChanged(value, text)
    {
        hdmiModeText.text = text
    }

    function invokeMenuHDMIDriveSelected()
    {
        hdmiGroupItem.isActivated = false
        hdmiModeItem.isActivated = false
        hdmiDriveItem.isActivated = true
        hdmiBoostItem.isActivated = false
        displayRotationItem.isActivated = false
    }

    function invokeMenuHDMIDriveChanged(value, text)
    {
        hdmiDriveText.text = text
    }

    function invokeMenuHDMIBoostSelected()
    {
        hdmiGroupItem.isActivated = false
        hdmiModeItem.isActivated = false
        hdmiDriveItem.isActivated = false
        hdmiBoostItem.isActivated = true
        displayRotationItem.isActivated = false
    }

    function invokeMenuHDMIBoostChanged(value)
    {
        hdmiBoostText.text = value.toString()
    }

    function invokeMenuWifiNetworkSelected()
    {
        displayRotationItem.isActivated = false
        wifiNetworkItem.isActivated = true
        wifiNameItem.isActivated = false
    }

    function invokeMenuWifiNetworkChanged(text)
    {
        wifiNetworkText.text = text
    }

    function invokeMenuWifiNameSelected()
    {
        wifiNameItem.isActivated = true
        wifiNetworkItem.isActivated = false
        wifiPassItem.isActivated = false
    }

    function invokeMenuWifiNameChanged(text)
    {
        wifiNameText.text = text
    }

    function invokeMenuWifiPassSelected()
    {
        wifiNameItem.isActivated = false
        wifiPassItem.isActivated = true
        saveSettingsItem.isActivated = false
    }

    function invokeMenuWifiPassChanged(text)
    {
        wifiPassText.text = text
    }

    function invokeMenuSaveSelected()
    {
        wifiPassItem.isActivated = false
        saveSettingsItem.isActivated = true
        loadSettingsItem.isActivated = false
    }

    function invokeMenuSavePressed()
    {

    }

    function invokeMenuCancelSelected()
    {
        saveSettingsItem.isActivated = false
        loadSettingsItem.isActivated = true
        hdmiGroupItem.isActivated = false
    }

    function invokeMenuCancelPressed()
    {

    }

    Image {
        id: menuBgImage
        source: "menu_background_horizontal.png"
        fillMode: Image.PreserveAspectCrop
        clip: true
        anchors.fill: parent
    }

    Text {
        id: settingsTitle
        y: 30
        x: (parent.width - width) /2
        text: "Settings"
        font.pointSize: 16
        color: "#00cdc1"
    }

    Item {
        id: hdmiGroupItem
        x: parent.width/2 - childrenRect.width/2
        y: settingsTitle.y + 40
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: true
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: hdmiGroupItem.rectOpacity
        }

        Text {
            id: hdmiGroupLabel
            text: "HDMI Group: "
            color: hdmiGroupItem.isActivated ? "#000000" : "#adadad"
        }

        Image {
            x: hdmiGroupLabel.x + hdmiGroupLabel.width + 16
            id: hdmiGroupLessImage
            width: height
            source: "less.png"
            height: hdmiGroupText.height
        }
        Text {
            id: hdmiGroupText
            x: hdmiGroupLessImage.x + hdmiGroupLessImage.width + 16
            y: hdmiGroupLessImage.y
            text: "UNKNOWN"

            color: hdmiGroupItem.isActivated ? "#000000" : "#adadad"
        }
        Image {
            id: hdmiGroupMoreImage
            x: hdmiGroupText.x + hdmiGroupText.width + 16
            y: hdmiGroupLessImage.y
            width: height
            height: hdmiGroupText.height
            source: "more.png"
        }
    }

    Item {
        id: hdmiModeItem
        x: parent.width/2 - childrenRect.width/2
        y: hdmiGroupItem.y + hdmiGroupItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: hdmiModeItem.rectOpacity
        }

        Text {
            id: hdmiModeLabel
            text: "HDMI Mode: "
            color: hdmiModeItem.isActivated ? "#000000" : "#adadad"
        }

        Image {
            x: hdmiModeLabel.x + hdmiModeLabel.width + 16
            id: hdmiModeLessImage
            width: height
            source: "less.png"
            height: hdmiModeText.height
        }
        Text {
            id: hdmiModeText
            x: hdmiModeLessImage.x + hdmiModeLessImage.width + 16
            y: hdmiModeLessImage.y
            text: "UNKNOWN"
            color: hdmiModeItem.isActivated ? "#000000" : "#adadad"
        }
        Image {
            id: hdmiModeMoreImage
            x: hdmiModeText.x + hdmiModeText.width + 16
            y: hdmiModeLessImage.y
            width: height
            height: hdmiModeText.height
            source: "more.png"
        }
    }

    Item {
        id: hdmiDriveItem
        x: parent.width/2 - childrenRect.width/2
        y: hdmiModeItem.y + hdmiModeItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: hdmiDriveItem.rectOpacity
        }

        Text {
            id: hdmiDriveLabel
            text: "HDMI Drive: "
            color: hdmiDriveItem.isActivated ? "#000000" : "#adadad"
        }

        Image {
            x: hdmiDriveLabel.x + hdmiDriveLabel.width + 16
            id: hdmiDriveLessImage
            width: height
            source: "less.png"
            height: hdmiDriveText.height
        }
        Text {
            id: hdmiDriveText
            x: hdmiDriveLessImage.x + hdmiDriveLessImage.width + 16
            y: hdmiDriveLessImage.y
            text: "UNKNOWN"
            color: hdmiDriveItem.isActivated ? "#000000" : "#adadad"
        }
        Image {
            id: hdmiDriveMoreImage
            x: hdmiDriveText.x + hdmiDriveText.width + 16
            y: hdmiDriveLessImage.y
            width: height
            height: hdmiDriveText.height
            source: "more.png"
        }
    }

    Item {
        id: hdmiBoostItem
        x: parent.width/2 - childrenRect.width/2
        y: hdmiDriveItem.y + hdmiDriveItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: hdmiBoostItem.rectOpacity
        }

        Text {
            id: hdmiBoostLabel
            text: "HDMI Boost: "
            color: hdmiBoostItem.isActivated ? "#000000" : "#adadad"
        }

        Image {
            x: hdmiBoostLabel.x + hdmiBoostLabel.width + 16
            id: hdmiBoostLessImage
            width: height
            source: "less.png"
            height: hdmiBoostText.height
        }
        Text {
            id: hdmiBoostText
            x: hdmiBoostLessImage.x + hdmiBoostLessImage.width + 16
            y: hdmiBoostLessImage.y
            text: "UNKNOWN"
            color: hdmiBoostItem.isActivated ? "#000000" : "#adadad"
        }
        Image {
            id: hdmiBoostMoreImage
            x: hdmiBoostText.x + hdmiBoostText.width + 16
            y: hdmiBoostLessImage.y
            width: height
            height: hdmiBoostText.height
            source: "more.png"
        }
    }

    Item {
        id: displayRotationItem
        x: parent.width/2 - childrenRect.width/2
        y: hdmiBoostItem.y + hdmiBoostItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: displayRotationItem.rectOpacity
        }

        Text {
            id: displayRotationLabel
            text: "Display Rotation: "
            color: displayRotationItem.isActivated ? "#000000" : "#adadad"
        }

        Image {
            x: displayRotationLabel.x + displayRotationLabel.width + 16
            id: displayRotationLessImage
            width: height
            source: "less.png"
            height: displayRotationText.height
        }
        Text {
            id: displayRotationText
            x: displayRotationLessImage.x + displayRotationLessImage.width + 16
            y: displayRotationLessImage.y
            text: "UNKNOWN"
            color: displayRotationItem.isActivated ? "#000000" : "#adadad"
        }
        Image {
            id: displayRotationMoreImage
            x: displayRotationText.x + displayRotationText.width + 16
            y: displayRotationLessImage.y
            width: height
            height: displayRotationText.height
            source: "more.png"
        }
    }

    //wifi
    Item {
        id: wifiNetworkItem
        x: parent.width/2 - childrenRect.width/2
        y: displayRotationItem.y + displayRotationItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: wifiNetworkItem.rectOpacity
        }

        Text {
            id: wifiNetworkLabel
            text: "Wifi Network: "
            color: wifiNetworkItem.isActivated ? "#000000" : "#adadad"
        }

        Image {
            x: wifiNetworkLabel.x + wifiNetworkLabel.width + 16
            id: wifiNetworkLessImage
            width: height
            source: "less.png"
            height: wifiNetworkText.height
        }
        Text {
            id: wifiNetworkText
            x: wifiNetworkLessImage.x + wifiNetworkLessImage.width + 16
            y: wifiNetworkLessImage.y
            text: "UNKNOWN"
            color: wifiNetworkItem.isActivated ? "#000000" : "#adadad"
        }
        Image {
            id: wifiNetworkMoreImage
            x: wifiNetworkText.x + wifiNetworkText.width + 16
            y: wifiNetworkLessImage.y
            width: height
            height: wifiNetworkText.height
            source: "more.png"
        }
    }

    Item {
        id: wifiNameItem
        x: parent.width/2 - childrenRect.width/2
        y: wifiNetworkItem.y + wifiNetworkItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: wifiNameItem.rectOpacity
        }

        Text {
            id: wifiNameLabel
            text: "Wifi Name: "
            color: wifiNameItem.isActivated ? "#000000" : "#adadad"
        }
        Text {
            id: wifiNameText
            x: wifiNameLabel.x + wifiNameLabel.width + 16
            y: wifiNameLabel.y
            text: ""
            color: wifiNameItem.isActivated ? "#000000" : "#adadad"
        }
    }

    Item {
        id: wifiPassItem
        x: parent.width/2 - childrenRect.width/2
        y: wifiNameItem.y + wifiNameItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: wifiPassItem.rectOpacity
        }

        Text {
            id: wifiPassLabel
            text: "Wifi Pass: "

            color: wifiPassItem.isActivated ? "#000000" : "#adadad"
        }
        Text {
            id: wifiPassText
            x: wifiPassLabel.x + wifiPassLabel.width + 16
            y: wifiPassLabel.y
            text: ""
            color: wifiPassItem.isActivated ? "#000000" : "#adadad"
        }
    }

    Item {
        id: saveSettingsItem
        x: parent.width/2 - childrenRect.width/2
        y: wifiPassItem.y + wifiPassItem.height + 32
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: saveSettingsItem.rectOpacity
        }
        Text {
            id: saveSettingsText
            text: "SAVE"
            font.pointSize: 16
            color: saveSettingsItem.isActivated ? "#000000" : "#adadad"
        }
    }

    Item {
        id: loadSettingsItem
        x: parent.width/2 - childrenRect.width/2
        y: saveSettingsItem.y + saveSettingsItem.height + 16
        width: childrenRect.width
        height: childrenRect.height
        property bool isActivated: false
        property double rectOpacity: isActivated ? 0.4 : 0.1

        Rectangle {
            color: "#00cdc1"
            width: parent.width
            height: parent.height
            opacity: loadSettingsItem.rectOpacity
        }
        Text {
            id: loadSettingsText
            text: "CANCEL"
            font.pointSize: 16

            color: loadSettingsItem.isActivated ? "#000000" : "#adadad"
        }
    }

}

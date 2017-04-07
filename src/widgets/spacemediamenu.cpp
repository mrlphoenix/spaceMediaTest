#include "spacemediamenu.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QProcess>
#include "platformspecific.h"
SpaceMediaMenuBackend::SpaceMediaMenuBackend(QObject *parent) : QObject(parent)
{
    for (int i = 0; i < 256; i++)
        keys[i] = false;
    isActivated = false;
}

void SpaceMediaMenuBackend::onKeyDown(int code)
{
    //0: group; 1: mode; 2: drive; 3: boost; 4: rotation; 5: wifiSelection; 6: wifiName; 7: wifiPassword
    keys[code] = true;
    if (!isActivated)
        return;
    if (code == 108)                        //down pressed
        settingsCursor = (settingsCursor + 1)%10;
    else if (code == 103)                   //up pressed
        settingsCursor = (settingsCursor + 9)%10;
    if (code == 108 || code == 103)
    {
        switch (settingsCursor )
        {
        default:
        case 0:
            emit hdmiGroupSelected();
            break;
        case 1:
            emit hdmiModeSelected();
            break;
        case 2:
            emit hdmiDriveSelected();
            break;
        case 3:
            emit hdmiBoostSelected();
            break;
        case 4:
            emit displayRotationSelected();
            break;
        case 5:
            emit wifiNetworkSelected();
            needToResetWifiName = true;
            break;
        case 6:
            emit wifiNameSelected();
            needToResetWifiPass = true;
            break;
        case 7:
            emit wifiPassSelected();
            needToResetWifiName = true;
            break;
        case 8:
            emit saveSelected();
            needToResetWifiPass = true;
            break;
        case 9:
            emit cancelSelected();
            break;
        }
    }

    if (code == 105 || code == 106) //if left or right is pressed
    {
        QVector<RaspberryConfigResource::RPIConfigEntry> currentHDMIResource;
        switch (settingsCursor)
        {
        case 0: //HDMI GROUP
            if (code == 105)
                groupCursor.index = (groupCursor.index + resource.groups.count() - 1)%resource.groups.count();
            else
                groupCursor.index = (groupCursor.index + 1)%resource.groups.count();
            foreach (const RaspberryConfigResource::RPIConfigEntry &entry, resource.groups)
                if (entry.id == groupCursor.index)
                    groupCursor.text = entry.toString();
            emit hdmiGroupChanged(groupCursor.index, groupCursor.text);
            bootConfigLines["hdmi_group"] = QString::number(groupCursor.index);
            break;
        case 1: //HDMI MODE
            if (groupCursor.index == 1)
                currentHDMIResource = resource.hdmi_mode_CEA;
            else
                currentHDMIResource = resource.hdmi_mode_DMT;
            if (code == 105)
                modeCursor.index = (modeCursor.index + currentHDMIResource.count() - 1) % currentHDMIResource.count();
            else
                modeCursor.index = (modeCursor.index + 1) % currentHDMIResource.count();
            foreach (const RaspberryConfigResource::RPIConfigEntry &entry, currentHDMIResource)
                if (entry.id == modeCursor.index)
                    modeCursor.text = entry.toString();
            emit hdmiModeChanged(modeCursor.index, modeCursor.text);
            bootConfigLines["hdmi_mode"] = QString::number(modeCursor.index);
            break;
        case 2: //HDMI DRIVE
            driveCursor.index = 3 - driveCursor.index; //1 -> 2; 2 -> 1;
            foreach (const RaspberryConfigResource::RPIConfigEntry &entry, resource.hdmi_drive)
                if (entry.id == driveCursor.index)
                    driveCursor.text = entry.toString();
            emit hdmiDriveChanged(driveCursor.index, driveCursor.text);
            bootConfigLines["hdmi_drive"] = QString::number(driveCursor.index);
            break;
        case 3: //HDMI BOOST
            if (code == 105)
                boostCursor.index = (boostCursor.index + 10) % 11;
            else
                boostCursor.index = (boostCursor.index + 1) % 11;
            emit hdmiBoostChanged(boostCursor.index);
            bootConfigLines["config_hdmi_boost"] = QString::number(boostCursor.index);
            break;
        case 4: //DISPLAY ROTATION
            if (code == 105)
                rotationCursor.index = (rotationCursor.index + 3)%4;
            else
                rotationCursor.index = (rotationCursor.index + 1)%4;
            rotationCursor.text = QString::number(rotationCursor.index * 90) + "°";
            emit displayRotationChanged(rotationCursor.index, rotationCursor.text);
            bootConfigLines["display_rotate"] = QString::number(rotationCursor.index);
            break;
        case 5: //Wifi Selection
            if (code == 105)
                currentWifi = (currentWifi + wifiList.count() - 1) % wifiList.count();
            else
                currentWifi = (currentWifi + 1) % wifiList.count();
            emit wifiNetworkChanged(wifiList[currentWifi]);
            if (wifiList[currentWifi] != "Hidden" &&
                wifiList[currentWifi] != "Disabled")
            {
                currentWifiName = wifiList[currentWifi];
                emit wifiNameChanged(wifiList[currentWifi]);
            }
            else
            {
                currentWifiName = "";
                emit wifiNameChanged("");
            }
            break;
        default:
            break;
        }
    }
    else if (PlatformSpecificService.isKeySymbol(code))
    {

        if (wifiList[currentWifi] != "Disabled")
        {
            switch (settingsCursor)
            {
            case 6: //Wifi Name
                if (wifiList[currentWifi] == "Hidden")
                {
                    if (needToResetWifiName)
                    {
                        currentWifiName = "";
                        needToResetWifiName = false;
                    }
                    currentWifiName += PlatformSpecificService.mapKey(code, keys[42] || keys[54]);
                    emit wifiNameChanged(currentWifiName);
                }
                break;
            case 7: //Wifi Pass
                if (needToResetWifiPass)
                {
                    currentWifiPass = "";
                    needToResetWifiPass = false;
                }
                currentWifiPass += PlatformSpecificService.mapKey(code, keys[42] || keys[54]);
                emit wifiPassChanged(currentWifiPass);
                break;
            default:
                break;
            }
        }
    }
    else if (code == 28)
    {
        switch (settingsCursor)
        {
        case 8: //Save Button
            qDebug() << "Save Enter";
            save();
            emit savePressed();
            break;
        case 9: //Cancel Button
            load();
            emit cancelPressed();
            break;
        default:
            break;
        }
    }
    else if (code == 14)
    {
        if (wifiList[currentWifi] != "Disabled")
        {
            switch (settingsCursor)
            {
            case 6: //Wifi Name
                if (wifiList[currentWifi] == "Hidden")
                {
                    if (currentWifiName != "")
                        currentWifiName = currentWifiName.mid(0,currentWifiName.count() - 1);
                }
                emit wifiNameChanged(currentWifiName);
                break;
            case 7: //Wifi Pass
                if (currentWifiPass != "")
                    currentWifiPass = currentWifiPass.mid(0, currentWifiPass.count() - 1);
                emit wifiPassChanged(currentWifiPass);
                break;
            default:
                break;
            }
        }
    }
}

void SpaceMediaMenuBackend::onKeyUp(int code)
{
    keys[code] = false;
}

void SpaceMediaMenuBackend::load()
{
    qDebug() << "SpaceMediaMenu::initialization";

    bootConfigLines.clear();
    QFile f("/boot/config.txt");
    if (f.open(QFile::ReadOnly))
    {
        qDebug() << "Loading menu config";
        QByteArray data = f.readAll();
        QFile outFile("/home/pi/teleds/boot_config.cache");
        if (outFile.open(QFile::WriteOnly))
        {
            outFile.write(data);
            outFile.flush();
            outFile.close();
        }
        QStringList bootConfig = QString(data).split("\n");
        foreach (const QString &s, bootConfig)
        {
            QStringList tokens = s.simplified().split("=");
            if (tokens.count() >= 2 && !s.contains("#"))
            {
                QString key = tokens[0];
                for (int i = 1; i < tokens.count() - 1; i++)
                {
                    key += "=";
                    key += tokens[i];
                }
                bootConfigLines[key] = tokens.last();
            }
        }
        f.close();
    }
    else
        qDebug() << "Error: Cant open boot config";

    QFile hdmiFile("data/hdmi_modes.json");
    if (hdmiFile.open(QFile::ReadOnly))
    {
        qDebug() << "Loading HDMI Config";
        QJsonDocument jDoc = QJsonDocument::fromJson(hdmiFile.readAll());
        resource = RaspberryConfigResource::fromJson(jDoc.object());
        hdmiFile.close();
    }
    else
        qDebug() << "Error: Cant open HDMI config";

    QProcess scanWifiProcess;
    scanWifiProcess.start("bash data/scan_wifi.sh");
    scanWifiProcess.waitForStarted();
    scanWifiProcess.waitForFinished();
    QStringList wifiLines = QString(scanWifiProcess.readAll()).split("\n");
    wifiList.clear();
    for (int i = 0; i < wifiLines.count(); i++)
    {
        qDebug() << wifiLines[i];
        if (!wifiLines[i].simplified().isEmpty())
        {
            QStringList tokens = wifiLines[i].split(":");
            if (tokens.count() == 2)
            {
                if (tokens[1] != "")
                    wifiList.append(tokens[1].simplified().replace("\"",""));
            }
        }
    }
    wifiList.append("Hidden");
    wifiList.append("Disabled");
    currentWifi = 0;
    initCursors();

    //Load current wifi


    QStringList wifiConfigLines;

    QFile wifiF("/etc/wpa_supplicant/wpa_supplicant.conf");
    if (wifiF.open(QFile::ReadOnly))
    {
        wifiConfigLines = QString(wifiF.readAll()).split("\n");
        foreach (const QString &s, wifiConfigLines)
        {
            qDebug() << s;
            if (s.indexOf("disabled") != -1)
            {
                qDebug() << "Disabled flag found";
                //"disabled" string found
                QString isDisabledString = s.simplified();
                qDebug() << isDisabledString;
                QStringList tokens = isDisabledString.split("=");
                if (tokens.count() > 1)
                {
                    if (tokens[1].simplified() == "1")
                    {
                        //wifi is disabled
                        currentWifi = wifiList.indexOf("Disabled");
                        //emit wifiNameChanged("");
                        emit wifiNetworkChanged("Disabled");
                    }
                }
            }
            else if (s.indexOf("scan_ssid=1") != -1)
            {
                qDebug() << "Scan Flag found";
                //Hidden Wifi scan enabled
                if (wifiList[currentWifi] != "Disabled")
                {
                    currentWifi = wifiList.indexOf("Hidden");
                    emit wifiNetworkChanged("Hidden");
                }
            }
            else if (s.indexOf("ssid=") != -1)
            {
                qDebug() << "SSID flag found";
                currentWifiName = s.simplified().replace("\"", "").replace("ssid=","");
                qDebug() << "SSID test " << currentWifiName;
                qDebug() << wifiList;
                if (wifiList[currentWifi] != "Disabled" && wifiList.contains(currentWifiName))
                {
                    currentWifi = wifiList.indexOf(currentWifiName);
                    emit wifiNameChanged(currentWifiName);
                    emit wifiNetworkChanged(currentWifiName);
                }
            }
            else if (s.indexOf("psk=") != -1)
            {
                currentWifiPass = s.simplified().replace("\"","").replace("psk=", "");

                qDebug() << "currentWifiPass" << currentWifiPass;
            }
        }
    }
    else
    {
        qDebug() << "Wifi config not found. Assuming Wifi is disabled";
        currentWifi = wifiList.count() - 1; //"Disabled"
        emit wifiNetworkChanged(wifiList[currentWifi]);
    }
}

void SpaceMediaMenuBackend::save()
{
    qDebug() << "Saving Menu Data";
    QFile outFile("/boot/config.txt");
    if (outFile.open(QFile::WriteOnly))
    {
        foreach (const QString &s, bootConfigLines.keys())
        {
            QString line = s + "=" + bootConfigLines[s] + "\n";
            outFile.write(line.toLocal8Bit());
        }
        outFile.flush();
        outFile.close();
    }
    else
    {
        qDebug() << "Error: cant save RPI Boot Config";
    }
    //
    QFile wifiFile("/etc/wpa_supplicant/wpa_supplicant.conf");
    if (wifiFile.open(QFile::WriteOnly))
    {
        wifiFile.write(QString("ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev\n").toLocal8Bit());
        wifiFile.write(QString("update_config=1\n").toLocal8Bit());
        wifiFile.write(QString("\n\n").toLocal8Bit());
        wifiFile.write(QString("network={\n").toLocal8Bit());
        if (wifiList[currentWifi] == "Disabled")
            wifiFile.write(QString("	disabled=1\n").toLocal8Bit());
        else
            wifiFile.write(QString("	disabled=0\n").toLocal8Bit());
        if (wifiList[currentWifi] == "Hidden")
            wifiFile.write(QString("	scan_ssid=1\n").toLocal8Bit());
        wifiFile.write(QString("    ssid=\"" + currentWifiName + "\"\n").toLocal8Bit());
        wifiFile.write(QString("    psk=\"" + currentWifiPass + "\"\n").toLocal8Bit());
        wifiFile.write(QString("}\n").toLocal8Bit());
        wifiFile.flush();
        wifiFile.close();
    }
    else
    {
        qDebug() << "Error: cant open WPA_SUPPLICANT config file";
    }
    QProcess p;
    p.start("reboot");
    p.waitForStarted();
    p.waitForFinished();
}

void SpaceMediaMenuBackend::initCursors()
{
    settingsCursor = 0;

    if (bootConfigLines.contains("hdmi_group"))
        groupCursor.index = bootConfigLines["hdmi_group"].toInt();
    else
        groupCursor.index = 2;
    foreach (const RaspberryConfigResource::RPIConfigEntry &entry, resource.groups)
        if (entry.id == groupCursor.index)
            groupCursor.text = entry.toString();
    emit hdmiGroupChanged(groupCursor.index, groupCursor.text);

    if (bootConfigLines.contains("hdmi_mode"))
        modeCursor.index = bootConfigLines["hdmi_mode"].toInt();
    else
        modeCursor.index = 4;
    QVector<RaspberryConfigResource::RPIConfigEntry> currentHDMIResource;
    if (groupCursor.index == 1)
        currentHDMIResource = resource.hdmi_mode_CEA;
    else
        currentHDMIResource = resource.hdmi_mode_DMT;
    foreach (const RaspberryConfigResource::RPIConfigEntry &entry, currentHDMIResource)
        if (entry.id == modeCursor.index)
            modeCursor.text = entry.toString();

    emit hdmiModeChanged(modeCursor.index, modeCursor.text);

    if (bootConfigLines.contains("hdmi_drive"))
    {
        driveCursor.index = bootConfigLines["hdmi_drive"].toInt();
    }
    else
        driveCursor.index = 2;
    foreach (const RaspberryConfigResource::RPIConfigEntry &entry, resource.hdmi_drive)
        if (entry.id == driveCursor.index)
            driveCursor.text = entry.toString();

    emit hdmiDriveChanged(driveCursor.index, driveCursor.text);

    if (bootConfigLines.contains("config_hdmi_boost"))
        boostCursor.index = bootConfigLines["config_hdmi_boost"].toInt();
    else
        boostCursor.index = 5;

    emit hdmiBoostChanged(boostCursor.index);

    if (bootConfigLines.contains("display_rotate"))
    {
        rotationCursor.index = bootConfigLines["display_rotate"].toInt();
        rotationCursor.text = QString::number(rotationCursor.index * 90) + "°";
    }
    else
    {
        rotationCursor.index = 0;
        rotationCursor.text = "0°";
    }
    emit displayRotationChanged(rotationCursor.index, rotationCursor.text);
}


SpaceMediaMenuBackend::RaspberryConfigResource SpaceMediaMenuBackend::RaspberryConfigResource::fromJson(const QJsonObject &object)
{
    RaspberryConfigResource result;
    QJsonArray groupsArray = object["groups"].toArray();
    foreach (const QJsonValue &v, groupsArray)
    {
        QJsonObject gObject = v.toObject();
        RPIConfigEntry entry;
        entry.id = gObject["id"].toInt();
        entry.text = gObject["text"].toString();
        result.groups.append(entry);
    }
    QJsonArray hdmiCEAArray = object["hdmi_modes"].toObject()["1"].toArray();
    foreach (const QJsonValue &v, hdmiCEAArray)
    {
        QJsonObject object = v.toObject();
        RPIConfigEntry entry;
        entry.id = object["id"].toInt();
        entry.text = object["text"].toString();
        result.hdmi_mode_CEA.append(entry);
    }
    QJsonArray hdmiDMTArray = object["hdmi_modes"].toObject()["2"].toArray();
    foreach (const QJsonValue &v, hdmiDMTArray)
    {
        QJsonObject object = v.toObject();
        RPIConfigEntry entry;
        entry.id = object["id"].toInt();
        entry.text = object["text"].toString();
        entry.freq = object["freq"].toString();
        result.hdmi_mode_DMT.append(entry);
    }

    // hdmi_drive=1 Normal DVI mode (No sound)
    //hdmi_drive=2 Normal HDMI mode (Sound will be sent if supported and enabled)
    RPIConfigEntry hdmiDrive1, hdmiDrive2;
    hdmiDrive1.id = 1;
    hdmiDrive1.text = "DVI";
    result.hdmi_drive.append(hdmiDrive1);
    hdmiDrive2.id = 2;
    hdmiDrive2.text = "HDMI";
    result.hdmi_drive.append(hdmiDrive2);
    return result;
}

QString SpaceMediaMenuBackend::RaspberryConfigResource::RPIConfigEntry::toString() const
{
    return QString::number(id) + ": " + text + " " + freq;

}

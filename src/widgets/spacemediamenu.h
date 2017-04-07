#ifndef SPACEMEDIAMENU_H
#define SPACEMEDIAMENU_H
#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QHash>
#include <QStringList>

class SpaceMediaMenuBackend : public QObject
{
    Q_OBJECT
public:
    SpaceMediaMenuBackend(QObject * parent = 0);
    ~SpaceMediaMenuBackend(){}
public slots:
    void onKeyDown(int code);
    void onKeyUp(int code);
    void load();
    void save();
    void initCursors();
    void setActive(bool isActive) {isActivated = isActive;}

signals:
    void tabChanged(int index);

    void hdmiGroupChanged(int value, QString text);
    void hdmiGroupSelected();
    void hdmiModeChanged(int value, QString text);
    void hdmiModeSelected();
    void hdmiBoostChanged(int value);
    void hdmiBoostSelected();
    void hdmiDriveChanged(int value, QString text);
    void hdmiDriveSelected();
    void displayRotationChanged(int value, QString text);
    void displayRotationSelected();
    //methods for network configuration
    void wifiNetworkSelected();
    void wifiNetworkChanged(QString text);
    void wifiNameSelected();
    void wifiNameChanged(QString text);
    void wifiPassSelected();
    void wifiPassChanged(QString text);
    //methods for saving and loading config
    void saveSelected();
    void savePressed();
    void cancelSelected();
    void cancelPressed();

    void show();
    void hide();

private:

    bool isActivated;
    bool keys[256];

    int settingsCursor; //0: group; 1: mode; 2: drive; 3: boost; 4: rotation;

    struct SpaceMediaMenuCursor {
        int index;
        QString text;
    };

    struct RaspberryConfigResource
    {
        static RaspberryConfigResource fromJson(const QJsonObject &object);

        struct RPIConfigEntry
        {
            int id;
            QString text;
            QString freq;
            QString toString() const;
        };
        QVector <RPIConfigEntry> groups;
        QVector <RPIConfigEntry> hdmi_mode_CEA;
        QVector <RPIConfigEntry> hdmi_mode_DMT;
        QVector <RPIConfigEntry> hdmi_drive;
    };
    RaspberryConfigResource resource;
    QStringList wifiList;
    int currentWifi;
    QString currentWifiName, currentWifiPass;
    bool needToResetWifiName, needToResetWifiPass;

    SpaceMediaMenuCursor groupCursor, modeCursor, boostCursor, driveCursor, rotationCursor;
    QHash<QString, QString> bootConfigLines;
};

#endif // SPACEMEDIAMENU_H

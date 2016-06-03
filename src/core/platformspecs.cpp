#include <QDebug>

#include <QStringList>
#include <QProcess>
#include <QString>
#include <QNetworkInterface>
#include <QDateTime>
#include <QStorageInfo>
#include "platformspecs.h"
#include "singleton.h"
#include "globalstats.h"

#ifdef PLATFORM_DEFINE_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include "sys/system_properties.h"
#include "sys/sysinfo.h"
#include "JlCompress.h"

#define ANDROID_OS_BUILD_VERSION_RELEASE     "ro.build.version.release"          // * The user-visible version string. E.g., "1.0" or "3.4b5".
#define ANDROID_OS_BUILD_VERSION_INCREMENTAL "ro.build.version.incremental"      // The internal value used by the underlying source control to represent this build.
#define ANDROID_OS_BUILD_VERSION_CODENAME    "ro.build.version.codename"         // The current development codename, or the string "REL" if this is a release build.
#define ANDROID_OS_BUILD_VERSION_SDK         "ro.build.version.sdk"              // The user-visible SDK version of the framework.

#define ANDROID_OS_BUILD_MODEL               "ro.product.model"                  // * The end-user-visible name for the end product..
#define ANDROID_OS_BUILD_MANUFACTURER        "ro.product.manufacturer"           // The manufacturer of the product/hardware.
#define ANDROID_OS_BUILD_BOARD               "ro.product.board"                  // The name of the underlying board, like "goldfish".
#define ANDROID_OS_BUILD_BRAND               "ro.product.brand"                  // The brand (e.g., carrier) the software is customized for, if any.
#define ANDROID_OS_BUILD_DEVICE              "ro.product.device"                 // The name of the industrial design.
#define ANDROID_OS_BUILD_PRODUCT             "ro.product.name"                   // The name of the overall product.
#define ANDROID_OS_BUILD_HARDWARE            "ro.hardware"                       // The name of the hardware (from the kernel command line or /proc).
#define ANDROID_OS_BUILD_CPU_ABI             "ro.product.cpu.abi"                // The name of the instruction set (CPU type + ABI convention) of native code.
#define ANDROID_OS_BUILD_CPU_ABI2            "ro.product.cpu.abi2"               // The name of the second instruction set (CPU type + ABI convention) of native code.

#define ANDROID_OS_BUILD_DISPLAY             "ro.build.display.id"               // * A build ID string meant for displaying to the user.
#define ANDROID_OS_BUILD_HOST                "ro.build.host"
#define ANDROID_OS_BUILD_USER                "ro.build.user"
#define ANDROID_OS_BUILD_ID                  "ro.build.id"                       // Either a changelist number, or a label like "M4-rc20".
#define ANDROID_OS_BUILD_TYPE                "ro.build.type"                     // The type of build, like "user" or "eng".
#define ANDROID_OS_BUILD_TAGS                "ro.build.tags"                     // Comma-separated tags describing the build, like "unsigned,debug".

#define ANDROID_OS_BUILD_FINGERPRINT         "ro.build.fingerprint"              // A string that uniquely identifies this build. 'BRAND/PRODUCT/DEVICE:RELEASE/ID/VERSION.INCREMENTAL:TYPE/TAGS'

#endif

#ifdef PLATFORM_DEFINE_LINUX
#include <stdio.h>
#include "sys/sysinfo.h"
#include "sys/utsname.h"
#endif

#ifdef PLATFORM_DEFINE_RPI

#endif



PlatformSpecific::PlatformSpecific(QObject *parent) : QObject(parent)
{

}



QString PlatformSpecific::getUniqueId()
{
#ifdef PLATFORM_DEFINE_ANDROID
/*
    QString result = "666";
    char model_id[PROP_VALUE_MAX];
    int len = 0;
    len = __system_property_get(ANDROID_OS_BUILD_BOARD, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_BRAND, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_CPU_ABI, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_DEVICE, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_ID, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_MANUFACTURER, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_MODEL, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_PRODUCT, model_id);
    result = result + QString::number(len);
    len = __system_property_get(ANDROID_OS_BUILD_HARDWARE, model_id);
    result = result + QString::number(len);*/


    QAndroidJniEnvironment env;
    jclass contextClass = env->FindClass("android/content/Context");
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "TELEPHONY_SERVICE", "Ljava/lang/String;");
    jstring telephonyManagerType = (jstring) env->GetStaticObjectField(contextClass, fieldId);
    jclass telephonyManagerClass = env->FindClass("android/telephony/TelephonyManager");
    jmethodID methodId = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    jobject telephonyManager = env->CallObjectMethod(qtActivityObj.object<jobject>(), methodId, telephonyManagerType);
    methodId = env->GetMethodID(telephonyManagerClass, "getDeviceId", "()Ljava/lang/String;");
    jstring jstr = (jstring) env->CallObjectMethod(telephonyManager, methodId);
    jsize len = env->GetStringUTFLength(jstr);
    if (len == 0)
        return "";
    qDebug() << "get unique id" << len;

    char* buf_devid = new char[64];
    env->GetStringUTFRegion(jstr, 0, len, buf_devid);
    QString imei(buf_devid);
    delete buf_devid;
    return imei;
#endif

#ifdef PLATFORM_DEFINE_LINUX
    char PSN[30];
    int varEAX, varEBX, varECX, varEDX;
    char str[9];
    //%eax=1 gives most significant 32 bits in eax
    __asm__ __volatile__ ("cpuid"   : "=a" (varEAX), "=b" (varEBX), "=c" (varECX), "=d" (varEDX) : "a" (1));
    sprintf(str, "%08X", varEAX); //i.e. XXXX-XXXX-xxxx-xxxx-xxxx-xxxx
    sprintf(PSN, "%C%C%C%C-%C%C%C%C", str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);
    //%eax=3 gives least significant 64 bits in edx and ecx [if PN is enabled]
    __asm__ __volatile__ ("cpuid"   : "=a" (varEAX), "=b" (varEBX), "=c" (varECX), "=d" (varEDX) : "a" (3));
    sprintf(str, "%08X", varEDX); //i.e. xxxx-xxxx-XXXX-XXXX-xxxx-xxxx
    sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C", PSN, str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);
    sprintf(str, "%08X", varECX); //i.e. xxxx-xxxx-xxxx-xxxx-XXXX-XXXX
    sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C", PSN, str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    return QString(PSN);
#endif

#ifdef PLATFORM_DEFINE_RPI
    QProcess cpuInfoProcess;
    QProcess grepProcess;

    cpuInfoProcess.setStandardOutputProcess(&grepProcess);

    cpuInfoProcess.start("cat /proc/cpuinfo");
    grepProcess.start("grep Serial");

    grepProcess.waitForFinished(-1);
    QByteArray result = grepProcess.readAll();
    return QString(result).replace("Serial\t\t: ","").replace("\n","");
#endif
    return "";
}

int64_t PlatformSpecific::getTrafficIn()
{
#ifdef PLATFORM_DEFINE_ANDROID
    qDebug() << "get Traffic";
    jlong value = 0;
    QAndroidJniEnvironment env;
    jclass clazz = env->FindClass("android/net/TrafficStats");
    if (clazz)
    {
        jmethodID mid = env->GetStaticMethodID(clazz,"getTotalRxBytes","()J");
        if (mid){
            qDebug() << "getValue";
            value = env->CallStaticLongMethod(clazz,mid);
            qDebug() << "TRAFFIC BYTES = " + QString::number(value);
        }
    }
    /*
    QProcess uidListProcess;
    uidListProcess.start("ls /proc/uid_stat");
    uidListProcess.waitForFinished();
    QByteArray result = uidListProcess.readAll();
    qDebug() << result;
    QStringList uids = QString(result).split("\n");
    int64_t traffic = 0;
    foreach (const QString &uid, uids)
    {
        QProcess tcpValueProcess;
        tcpValueProcess.start("cat /proc/uid_stat/" + uid + "/tcp_rcv");
        tcpValueProcess.waitForFinished();
        traffic += QString(tcpValueProcess.readAll()).toLongLong();
    }*/
    return value;
#endif

#ifdef PLATFORM_DEFINE_LINUX
    QProcess cpuUsageProcess;
    cpuUsageProcess.start("bash data/cpu_usage.sh");
    cpuUsageProcess.waitForFinished();
    QString result = cpuUsageProcess.readAll();
    qDebug() << result;
    QStringList tokens = result.split(" ");
    qlonglong trafficIn = tokens[2].toLongLong();
    qlonglong trafficOut = tokens[3].toLongLong();
    GlobalStatsInstance.setTraffic(trafficIn, trafficOut);
    return GlobalStatsInstance.getTrafficIn();
#endif

#ifdef PLATFORM_DEFINE_RPI
    QProcess cpuUsageProcess;
    cpuUsageProcess.start("bash data/cpu_usage.sh");
    cpuUsageProcess.waitForFinished();
    QString result = cpuUsageProcess.readAll();
    qDebug() << result;
    QStringList tokens = result.split(" ");
    qlonglong trafficIn = tokens[2].toLongLong();
    qlonglong trafficOut = tokens[3].toLongLong();
    GlobalStatsInstance.setTraffic(trafficIn, trafficOut);
    return GlobalStatsInstance.getTrafficIn();
#endif
    return 0;
}

int64_t PlatformSpecific::getTrafficOut()
{
#ifdef PLATFORM_DEFINE_ANDROID
    QProcess uidListProcess;
    uidListProcess.start("ls /proc/uid_stat");
    uidListProcess.waitForFinished();
    QByteArray result = uidListProcess.readAll();
    QStringList uids = QString(result).split("\n");
    int64_t traffic = 0;
    foreach (const QString &uid, uids)
    {
        QProcess tcpValueProcess;
        tcpValueProcess.start("cat /proc/uid_stat/" + uid + "/tcp_snd");
        tcpValueProcess.waitForFinished();
        traffic += QString(tcpValueProcess.readAll()).toLongLong();
    }
    return traffic;
#endif
#ifdef PLATFORM_DEFINE_LINUX
    QProcess cpuUsageProcess;
    cpuUsageProcess.start("bash data/cpu_usage.sh");
    cpuUsageProcess.waitForFinished();
    QString result = cpuUsageProcess.readAll();
    qDebug() << result;
    QStringList tokens = result.split(" ");
    qlonglong trafficIn = tokens[2].toLongLong();
    qlonglong trafficOut = tokens[3].toLongLong();
    GlobalStatsInstance.setTraffic(trafficIn, trafficOut);
    return GlobalStatsInstance.getTrafficOut();
#endif

#ifdef PLATFORM_DEFINE_RPI
    QProcess cpuUsageProcess;
    cpuUsageProcess.start("bash data/cpu_usage.sh");
    cpuUsageProcess.waitForFinished();
    QString result = cpuUsageProcess.readAll();
    qDebug() << result;
    QStringList tokens = result.split(" ");
    qlonglong trafficIn = tokens[2].toLongLong();
    qlonglong trafficOut = tokens[3].toLongLong();
    GlobalStatsInstance.setTraffic(trafficIn, trafficOut);
    return GlobalStatsInstance.getTrafficOut();
#endif
    return 0;
}

int PlatformSpecific::getMemoryUsage()
{
#ifdef PLATFORM_DEFINE_LINUX
    struct sysinfo info;
    sysinfo(&info);
    return double(info.freeram) / double(info.totalram) * 100.;
#endif
#ifdef PLATFORM_DEFINE_ANDROID
    struct sysinfo info;
    sysinfo(&info);
    return double(info.freeram) / double(info.totalram) * 100.;
#endif
    return 0;
}

int PlatformSpecific::getFreeMemory()
{
#ifdef PLATFORM_DEFINE_LINUX
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram;
#endif
#ifdef PLATFORM_DEFINE_ANDROID
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram;
#endif
#ifdef PLATFORM_DEFINE_RPI
    QProcess freeProcess;
    freeProcess.start("free");
    freeProcess.waitForFinished();
    QString data = freeProcess.readAll();
    QStringList lines = data.split("\n");
    if (lines.count() > 1)
    {
        QString dataLine = lines.at(1);
        QStringList items = dataLine.simplified().split(" ");
        if (items.count() > 3)
        {
            QString freeMemString = items.at(3);
            return freeMemString.toInt();
        }
    }
    return 0;

#endif
}

bool PlatformSpecific::getHdmiCEC()
{
#ifdef PLATFORM_DEFINE_RPI
    return true;
#endif
    return true;
}

bool PlatformSpecific::getHdmiGPIO()
{
#ifdef PLATFORM_DEFINE_RPI
    return true;
#endif
    return true;
}

double PlatformSpecific::getBattery()
{
#ifdef PLATFORM_DEFINE_ANDROID
    QProcess process;
    process.start("cat /sys/class/power_supply/battery/capacity");
    if (process.waitForFinished())
        return QString(process.readAll()).toInt();
#endif
    return 0.;
}

double PlatformSpecific::getAvgUsage()
{
#ifdef PLATFORM_DEFINE_WINDOWS
    return 0.;
#else
    QProcess loadAvgProcess;
    loadAvgProcess.start("cat /proc/loadavg");
    loadAvgProcess.waitForFinished();
    QStringList items = QString(loadAvgProcess.readAll()).split(" ");
    if (items.count())
        return items[0].toDouble();
    return 0.;
#endif
}

QString PlatformSpecific::getWifiMac()
{
#ifdef PLATFORM_DEFINE_ANDROID
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QString result;
    if (interfaces.count() > 0)
    {
        foreach (const QNetworkInterface &interface, interfaces)
        {
            if (interface.flags().testFlag(QNetworkInterface::IsUp))
            {
                QString macAddress = interface.hardwareAddress();
                qDebug() << "FOUND MAC: " << macAddress;
                if (macAddress.indexOf("00") != 0)
                    return macAddress;
                result = macAddress;
            }
        }
    }
    return result;
#endif
    return "";
}

int PlatformSpecific::getFreeSpace()
{
#ifdef PLATFORM_DEFINE_ANDROID
    QStorageInfo info("/sdcard/");
    return info.bytesAvailable()/1024;
#endif
    QStorageInfo infoRoot = QStorageInfo::root();
    return infoRoot.bytesAvailable()/1024;
}

PlatformSpecific::HardwareInfo PlatformSpecific::getHardwareInfo()
{
    HardwareInfo result;
#ifdef PLATFORM_DEFINE_WINDOWS
    result.osName = "Windows";
    result.vendor = "Microsoft";
#endif
#ifdef PLATFORM_DEFINE_RPI
    result.vendor = "Raspberry Pi";
    QProcess cpuInfoProcess;
    cpuInfoProcess.start("cat /proc/cpuinfo");
    cpuInfoProcess.waitForFinished();
    QStringList lines = QString(cpuInfoProcess.readAll()).split("\n");
    QString cpuName;
    QString deviceName;
    foreach (const QString &s, lines)
    {
        if (s.contains("model name"))
        {
            QStringList tokens = s.split(":");
            if (tokens.count() > 1)
            {
                cpuName = tokens.at(1);
                cpuName = cpuName.simplified();
            }
            continue;
        }
        if (s.contains("Revision"))
        {
            QStringList tokens = s.split(":");
            if (tokens.count() > 1)
            {
                QString deviceId = tokens.at(1);
                deviceId = deviceId.simplified();
                deviceName = getRpiDeviceNameById(deviceId);
            }
            continue;
        }
    }
    result.cpuName = cpuName;
    result.deviceModel = deviceName;
    result.deviceName = "Raspberry Pi";

    QProcess OSVersionProcess;
    OSVersionProcess.start("cat /proc/version");
    OSVersionProcess.waitForFinished();
    lines = QString(OSVersionProcess.readAll()).split(" ");
    result.osName = lines.at(0);
    result.osVersion = lines.at(2);
#endif

#ifdef PLATFORM_DEFINE_LINUX
    QProcess cpuInfoProcess;
    cpuInfoProcess.start("cat /proc/cpuinfo");
    cpuInfoProcess.waitForFinished();
    QStringList lines = QString(cpuInfoProcess.readAll()).split("\n");
    foreach (const QString& s, lines)
    {
        if (s.contains("model name"))
        {
            QStringList tokens = s.split(":");
            if (tokens.count() > 1)
            {
                QString cpuName = tokens.at(1);
                result.cpuName = cpuName.simplified();
                break;
            }
        }
    }
    utsname buf;
    uname(&buf);
    result.osName = QString(buf.sysname);
    result.osVersion = QString(buf.release);
    result.deviceName = QString(buf.machine);
    result.deviceModel = QString(buf.machine);
    result.vendor = "Linux";
#endif

#ifdef PLATFORM_DEFINE_ANDROID

    char buf[PROP_VALUE_MAX];

    result.osName = "Android";
    __system_property_get(ANDROID_OS_BUILD_MANUFACTURER, buf);
    result.vendor = QString(buf);

    __system_property_get(ANDROID_OS_BUILD_VERSION_RELEASE, buf);
    result.osVersion = QString(buf);
    __system_property_get(ANDROID_OS_BUILD_DEVICE, buf);
    result.deviceName = QString(buf);
    __system_property_get(ANDROID_OS_BUILD_MODEL, buf);
    result.deviceModel = QString(buf);


    QProcess cpuInfoProcess;
    cpuInfoProcess.start("cat /proc/cpuinfo");
    cpuInfoProcess.waitForFinished();
    QStringList lines = QString(cpuInfoProcess.readAll()).split("\n");
    foreach (const QString& s, lines)
    {
        if (s.contains("Processor"))
        {
            QStringList tokens = s.split(":");
            if (tokens.count() > 1)
            {
                QString cpuName = tokens.at(1);
                result.cpuName = cpuName.simplified();
                break;
            }
        }
    }
    qDebug() << "Vendor: " << result.vendor << "; OsVersion: " << result.osVersion << "; deviceName: " << result.deviceName
             << "; deviceModel: " << result.deviceModel << "; CpuName: " << result.cpuName;
#endif
    return result;
}

void PlatformSpecific::extractFile(QString file, QString id)
{
    QDir dir(VIDEO_FOLDER + id);
    dir.removeRecursively();
    QDir().mkdir(VIDEO_FOLDER + id);
    QFile::rename(VIDEO_FOLDER + file + "_", VIDEO_FOLDER + file);
#ifdef PLATFORM_DEFINE_ANDROID
    QFile * zipContentFile = new QFile(VIDEO_FOLDER + file);
    zipContentFile->open(QFile::ReadOnly);
    JlCompress::extractDir(zipContentFile,VIDEO_FOLDER + id + "/");
    zipContentFile->close();
    delete zipContentFile;
#endif

#ifdef PLATFORM_DEFINE_RPI
    QProcess unzipProc;
    unzipProc.start("unzip -o " + VIDEO_FOLDER + file + " -d " + VIDEO_FOLDER + id);
    unzipProc.waitForFinished();
    unzipProc.close();
#endif
#ifdef PLATFORM_DEFINE_LINUX
    QProcess unzipProc;
    unzipProc.start("unzip -o " + VIDEO_FOLDER + file + " -d " + VIDEO_FOLDER + id);
    unzipProc.waitForFinished();
    unzipProc.close();
#endif
}

void PlatformSpecific::writeToFile(QByteArray data, QString filename)
{
    QFile f(filename);
    f.open(QFile::WriteOnly);
    f.write(data);
    f.flush();
    f.close();
}

QString PlatformSpecific::getRpiDeviceNameById(QString id)
{
    if (id == "0002")
        return "Model B Revision 1.0";
    if (id == "0003")
        return "Model B Revision 1.0 + ECN0001 (no fuses, D14 removed)";
    if (id == "0004" || id == "0005" || id == "0006")
        return "Model B Revision 2.0/Mounting holes";
    if (id == "0007" || id == "0008" || id == "0009")
        return "Model A/Mounting holes";
    if (id == "000d" || id == "000e" || id == "000f")
        return "Model B Revision 2.0/Mounting holes";
    if (id == "0010")
        return "Model B+";
    if (id == "0011")
        return "Compute Module";
    if (id == "0012")
        return "Model A+";
    if (id == "a01041" || id == "a21041")
        return "Pi 2 Model B";
    if (id == "900092")
        return "PiZero";
    if (id == "a02082" || id == "a22082")
        return "Pi 3 Model B";
    return "";
}


PlatformSpecific::SystemInfo PlatformSpecific::SystemInfo::get()
{
    qDebug() << "PlatformSpecific::SystemInfo::get()";
    SystemInfo result;
    result.time = QDateTime::currentDateTimeUtc();
    result.cpu = PlatformSpecific::getAvgUsage();
    result.latitude = GlobalStatsInstance.getLatitude();
    result.longitude = GlobalStatsInstance.getLongitude();
    result.battery = PlatformSpecific::getBattery();

    GlobalStatsInstance.setTraffic(PlatformSpecific::getTrafficIn(),0.0);
    result.traffic = GlobalStatsInstance.getTrafficIn();
    result.free_memory = PlatformSpecific::getFreeMemory();
    result.wifi_mac = PlatformSpecific::getWifiMac();
    result.hdmi_cec = PlatformSpecific::getHdmiCEC();
    result.hdmi_gpio = PlatformSpecific::getHdmiGPIO();
    result.free_space = PlatformSpecific::getFreeSpace();

    qDebug() << result.time << result.cpu <<
                result.latitude << result.longitude <<
                result.battery << result.traffic << result.free_memory <<
                result.wifi_mac << result.hdmi_cec << result.hdmi_gpio <<
                result.free_space;

    return result;

}

PlatformSpecific::SystemInfo PlatformSpecific::SystemInfo::fromRecord(const QSqlRecord &record)
{
    SystemInfo result;
    result.time = QDateTime::fromString(record.value("time").toString(),"yyyy-MM-dd HH:mm:ss");
    result.battery = record.value("battery").toDouble();
    result.cpu = record.value("cpu").toDouble();
    result.free_memory = record.value("free_memory").toInt();
    result.free_space = record.value("free_space").toInt();
    result.hdmi_cec = record.value("hdmi_cec").toInt();
    result.hdmi_gpio = record.value("hdmi_gpio").toInt();
    result.latitude = record.value("latitude").toDouble();
    result.longitude = record.value("longitude").toDouble();
    result.traffic = record.value("traffic_in").toInt();
    result.wifi_mac = record.value("wifi_mac").toString();
 /*   qDebug() << result.time << result.battery << result.free_memory << result.free_space <<
                result.hdmi_cec << result.hdmi_gpio << result.latitude << result.longitude <<
                result.traffic <<result.wifi_mac;*/
    return result;
}

QJsonObject PlatformSpecific::SystemInfo::serialize() const
{
    QJsonObject result;
    result["timestamp"] = qint64(this->time.toTime_t());
    result["cpu_load"] = cpu;
    QJsonObject gpsObject;
    gpsObject["latitude"] = latitude;
    gpsObject["longitude"] = longitude;
    result["gps"] = gpsObject;
    result["battery"] = battery;
    result["traffic"] = traffic;
    result["free_memory"] = free_memory;
    result["wifi_mac"] = wifi_mac;
    result["hdmi_cec"] = hdmi_cec ? true:false;
    result["hdmi_gpio"] = hdmi_gpio ? true:false;
    result["free_space"] = free_space;
    return result;
}

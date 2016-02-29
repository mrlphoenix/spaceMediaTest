#include <QDebug>

#include <QStringList>
#include <QProcess>
#include "platformspecs.h"

#ifdef PLATFORM_DEFINE_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include "sys/system_properties.h"
#include "sys/sysinfo.h"
#endif

#ifdef PLATFORM_DEFINE_LINUX
#include <stdio.h>
#include "sys/sysinfo.h"
#endif

#ifdef PLATFORM_DEFINE_RPI

#endif



PlatformSpecs::PlatformSpecs(QObject *parent) : QObject(parent)
{

}



QString PlatformSpecs::getUniqueId()
{
#ifdef PLATFORM_DEFINE_ANDROID
/*
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

#define ANDROID_OS_BUILD_FINGERPRINT         "ro.build.fingerprint"              // A string that uniquely identifies this build. 'BRAND/PRODUCT/DEVICE:RELEASE/ID/VERSION.INCREMENTAL:TYPE/TAGS'.

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

int64_t PlatformSpecs::getTrafficIn()
{
#ifdef PLATFORM_DEFINE_ANDROID
    qDebug() << "get Traffic In";
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
    }
    return traffic;
#endif
    return 0;
}

int64_t PlatformSpecs::getTrafficOut()
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
    return 0;
}

int PlatformSpecs::getMemoryUsage()
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
}

double PlatformSpecs::getAvgUsage()
{
#ifdef PLATFORM_DEFINE_WINDOWS
    return 0.;
#else
    qDebug() << "AVG CALLED";
    QProcess loadAvgProcess;
    loadAvgProcess.start("cat /proc/loadavg");
    loadAvgProcess.waitForFinished();
    QStringList items = QString(loadAvgProcess.readAll()).split(" ");
    if (items.count())
        return items[0].toDouble();
    return 0.;
#endif
}


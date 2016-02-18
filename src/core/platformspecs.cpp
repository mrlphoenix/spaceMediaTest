#include <QDebug>
#include "platformspecs.h"

#ifdef PLATFORM_DEFINE_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include "sys/system_properties.h"
#endif

#ifdef PLATFORM_DEFINE_LINUX
#include <stdio.h>
#endif

#ifdef PLATFORM_DEFINE_RPI
#include <QProcess>
#endif



PlatformSpecs::PlatformSpecs(QObject *parent) : QObject(parent)
{

}



QString PlatformSpecs::getUniqueId()
{
#ifdef PLATFORM_DEFINE_ANDROID
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


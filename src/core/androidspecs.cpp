
#include "androidspecs.h"
#ifdef PLATFORM_DEFINE_ANDROID
#include <QDebug>
#include "sys/system_properties.h"
AndroidSpecs::AndroidSpecs(QObject *parent) : QObject(parent)
{

}

QString AndroidSpecs::getImei()
{

  /*  int ir = __system_property_get("ro.gsm.imei", imei_start);

      if(ir > 0)
    {
      imei_start[15]=0;//strz end
      printf("method1 got imei %s len %d\r\n",imei_start,strlen(imei_start));
      strcpy(g_imei,imei_start);
    }
      else
    {
      printf("method1 imei failed - trying method2\r\n");
      //old dumpsys imei getter
      char* res = exec_get_out("dumpsys iphonesubinfo");
      const char* imei_start_match = "ID = ";
      int imei_start_match_len = strlen(imei_start_match);
      char* imei_start = strstr(res,imei_start_match);
      if(imei_start && strlen(imei_start)>=15+imei_start_match_len)
        {
          imei_start += imei_start_match_len;
          imei_start[15] = 0;
          printf("method2 IMEI [%s] len %d\r\n",imei_start,strlen(imei_start));
          strcpy(g_imei,imei_start);
        }
    }*/
/*
    QAndroidJniEnvironment qenv;
    JNIEnv *env = &(*qenv);
    jobject context;
    jclass cls = (*env).FindClass("android/context/Context");
    jmethodID mid = (*env).GetMethodID(cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jfieldID fid = (*env).GetStaticFieldID(cls, "TELEPHONY_SERVICE", "Ljava/lang/String;");
    jstring str = static_cast<jstring>((*env).GetStaticObjectField(cls, fid));
    jobject telephony = (*env).CallObjectMethod(context, mid, str);
    cls = (*env).FindClass("android/telephony/TelephonyManager");
    mid =(*env).GetMethodID(cls, "getDeviceId", "()Ljava/lang/String;");
    str = static_cast<jstring>((*env).CallObjectMethod(telephony, mid));
    jsize len = (*env).GetStringUTFLength(str);

    qDebug() << "given len" << len;
    char* deviceId = (char*)calloc(len + 1, 1);
    (*env).GetStringUTFRegion(str, 0, len, deviceId);
    (*env).DeleteLocalRef(str);

    qDebug() << deviceId;*/

    qDebug() << "ENV";
    QAndroidJniEnvironment env;

    qDebug() << "context";
    jclass contextClass = env->FindClass("android/content/Context");

    qDebug() << "field";
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "TELEPHONY_SERVICE", "Ljava/lang/String;");

    qDebug() << "telephony";
    jstring telephonyManagerType = (jstring) env->GetStaticObjectField(contextClass, fieldId);


    qDebug() << "manager";

    jclass telephonyManagerClass = env->FindClass("android/telephony/TelephonyManager");

    qDebug() << "method";

    jmethodID methodId = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    qDebug() << "activity";

    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");

    qDebug() << "call method";
    jobject telephonyManager = env->CallObjectMethod(qtActivityObj.object<jobject>(), methodId, telephonyManagerType);

    qDebug() << "getDeviceId";
    methodId = env->GetMethodID(telephonyManagerClass, "getDeviceId", "()Ljava/lang/String;");


    qDebug() << "calling manager method";
    jstring jstr = (jstring) env->CallObjectMethod(telephonyManager, methodId);


    qDebug() << "utf len";
    jsize len = env->GetStringUTFLength(jstr);
    qDebug() << len;

    qDebug() << "allocating buffer" << len;
    char* buf_devid = new char[64];


    qDebug() << "get string into buffer " << jstr << " g " << len << " g " << buf_devid ;
    env->GetStringUTFRegion(jstr, 0, len, buf_devid);


    qDebug() << "generating string";
    QString imei(buf_devid);


    qDebug() << "delete";
    delete buf_devid;
    return imei;

    //return "";
}
#endif

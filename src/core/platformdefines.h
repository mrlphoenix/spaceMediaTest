#ifndef PLATFORMDEFINES
#define PLATFORMDEFINES


#define PLATFORM_DEFINE_RPI

//Enable this if you want to enable GPIO
//Disable for debugging in QtCreator
#define PLATFORM_RPI_ENABLE_GPIO

//enable this if you want to encode config file so users couldnt read
//#define PLATFORM_ENCODE_CONFIG


#define CONFIG_FOLDER_LINUX         QString("data/")
#define VIDEO_FOLDER_LINUX          QString("data/video/")

#define CONFIG_FOLDER_RPI           QString("data/")
#define VIDEO_FOLDER_RPI            QString("data/video/")

#define CONFIG_FOLDER_WINDOWS       QString("data/")
#define VIDEO_FOLDER_WINDOWS        QString("data/video/")

#define CONFIG_FOLDER_ANDROID       QString("/sdcard/download/teleds/")
#define VIDEO_FOLDER_ANDROID        QString("/sdcard/download/teleds/")

#define DATABASE_FOLDER_LINUX       QString("")
#define DATABASE_FOLDER_RPI         QString("")
#define DATABASE_FOLDER_WINDOWS     QString("")
#define DATABASE_FOLDER_ANDROID     QString("/sdcard/download/teleds/")

#define CONFIG_BUILD_NAME_ANDROID   QString("android")
#define CONFIG_BUILD_NAME_RPI       QString("rpi")
#define CONFIG_BUILD_NAME_LINUX     QString("linux")
#define CONFIG_BUILD_NAME_WINDOWS   QString("windows")


#ifdef PLATFORM_DEFINE_ANDROID
    #define CONFIG_FOLDER CONFIG_FOLDER_ANDROID
    #define VIDEO_FOLDER VIDEO_FOLDER_ANDROID
    #define PLAYER_MODE_FULLSCREEN
    #define DATABASE_FOLDER DATABASE_FOLDER_ANDROID
    #define CONFIG_BUILD_NAME CONFIG_BUILD_NAME_ANDROID
#endif

#ifdef PLATFORM_DEFINE_LINUX
    #define CONFIG_FOLDER CONFIG_FOLDER_LINUX
    #define VIDEO_FOLDER VIDEO_FOLDER_LINUX
    #define PLAYER_MODE_WINDOWED
    #define DATABASE_FOLDER DATABASE_FOLDER_LINUX
    #define CONFIG_BUILD_NAME CONFIG_BUILD_NAME_LINUX
#endif

#ifdef PLATFORM_DEFINE_RPI
    #define CONFIG_FOLDER CONFIG_FOLDER_RPI
    #define VIDEO_FOLDER VIDEO_FOLDER_RPI
    #define PLAYER_MODE_FULLSCREEN
    #define DATABASE_FOLDER DATABASE_FOLDER_RPI
    #define CONFIG_BUILD_NAME CONFIG_BUILD_NAME_RPI
#endif

#ifdef PLATFORM_DEFINE_WINDOWS
    #define CONFIG_FOLDER CONFIG_FOLDER_WINDOWS
    #define VIDEO_FOLDER VIDEO_FOLDER_WINDOWS
    #define PLAYER_MODE_WINDOWED
    #define DATABASE_FOLDER DATABASE_FOLDER_WINDOWS
    #define CONFIG_BUILD_NAME CONFIG_BUILD_NAME_WINDOWS
#endif

#endif // PLATFORMDEFINES

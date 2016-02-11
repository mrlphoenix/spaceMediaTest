#ifndef PLATFORMDEFINES
#define PLATFORMDEFINES


#define PLATFORM_DEFINE_RPI


#define CONFIG_FOLDER_LINUX QString("data/")
#define VIDEO_FOLDER_LINUX QString("data/video/")

#define CONFIG_FOLDER_RPI QString("data/")
#define VIDEO_FOLDER_RPI QString("data/video/")

#define CONFIG_FOLDER_WINDOWS QString("data/")
#define VIDEO_FOLDER_WINDOWS QString("data/video/")

#define CONFIG_FOLDER_ANDROID QString("/sdcard/download/teleds/")
#define VIDEO_FOLDER_ANDROID QString("/sdcard/download/teleds/")


#ifdef PLATFORM_DEFINE_ANDROID
    #define CONFIG_FOLDER CONFIG_FOLDER_ANDROID
    #define VIDEO_FOLDER VIDEO_FOLDER_ANDROID
#endif

#ifdef PLATFORM_DEFINE_LINUX
    #define CONFIG_FOLDER CONFIG_FOLDER_LINUX
    #define VIDEO_FOLDER VIDEO_FOLDER_LINUX
#endif

#ifdef PLATFORM_DEFINE_RPI
    #define CONFIG_FOLDER CONFIG_FOLDER_RPI
    #define VIDEO_FOLDER VIDEO_FOLDER_RPI
#endif

#ifdef PLATFORM_DEFINE_WINDOWS
    #define CONFIG_FOLDER CONFIG_FOLDER_WINDOWS
    #define VIDEO_FOLDER VIDEO_FOLDER_WINDOWS
#endif


#endif // PLATFORMDEFINES


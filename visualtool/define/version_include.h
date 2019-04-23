#ifndef VERSIONINCLUDE_H
#define VERSIONINCLUDE_H

#define VISUALNOC_TITLE "Visual Network-on-Chip Develop Studio"
#if defined( Q_OS_LINUX )
    #define VISUALNOC_PLATFORM  "Ubuntu"
    #define VISUALNOC_PLATFORM_CODE	"74"
#elif defined( Q_OS_WIN32 ) || defined( Q_OS_WIN64 )
    #define VISUALNOC_PLATFORM    "Windows"
    #define VISUALNOC_PLATFORM_CODE   "40"
#endif

#define VISUALNOC_VERSION	"07"

#define VISUALARCH_TITLE "Visual System-on-Chip Develop Studio"
#if defined( Q_OS_LINUX )
    #define VISUALARCH_PLATFORM  "Ubuntu"
    #define VISUALARCH_PLATFORM_CODE	"74"
#elif defined( Q_OS_WIN32 ) || defined( Q_OS_WIN64 )
    #define VISUALARCH_PLATFORM    "Windows"
    #define VISUALARCH_PLATFORM_CODE   "40"
#endif

#define VISUALARCH_VERSION	"05"

#endif // VERSIONINCLUDE_H

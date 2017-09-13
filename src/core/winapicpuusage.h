#ifndef WINAPICPUUSAGE_H
#define WINAPICPUUSAGE_H

#include <windows.h>
#define GlobalWINAPICPUInstance Singleton<WINAPICpuUsage>::instance()
#include "singleton.h"

class WINAPICpuUsage
{
public:
    WINAPICpuUsage(void);

    short  get();
private:
    ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
    bool EnoughTimePassed();
    inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

    //system total times
    FILETIME m_ftPrevSysKernel;
    FILETIME m_ftPrevSysUser;

    //process times
    FILETIME m_ftPrevProcKernel;
    FILETIME m_ftPrevProcUser;

    short m_nCpuUsage;
    ULONGLONG m_dwLastRun;

    volatile LONG m_lRunCount;
};

#endif // WINAPICPUUSAGE_H

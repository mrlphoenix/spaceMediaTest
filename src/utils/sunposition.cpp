#include "singleton.h"
#include "globalconfig.h"
#include "globalstats.h"
#include "sunposition.h"
#include <QDebug>


double SunsetSystem::CalcHourAngle(double dLat, double dSolarDec, bool bTime)
{
    double dLatRad = dDegToRad(dLat);
    if(bTime)  //Sunrise
        return  (acos(cos(dDegToRad(90.833))/(cos(dLatRad)*cos(dSolarDec))-tan(dLatRad) * tan(dSolarDec)));
    else
        return -(acos(cos(dDegToRad(90.833))/(cos(dLatRad)*cos(dSolarDec))-tan(dLatRad) * tan(dSolarDec)));
}

double SunsetSystem::calcSunsetGMT(int julDay, double latitude, double longitude)
{
    double dGamma = CalcGamma(julDay + 1);
    double eqTime = CalcEqofTime(dGamma);
    double solarDec = CalcSolarDec(dGamma);
    double hourAngle = CalcHourAngle(latitude, solarDec, 0);
    double delta = longitude - dRadToDeg(hourAngle);
    double timeDiff = 4 * delta;
    double setTimeGMT = 720 + timeDiff - eqTime;

    double gamma_sunset = CalcGamma2(julDay, setTimeGMT/60);
    eqTime = CalcEqofTime(gamma_sunset);


    solarDec = CalcSolarDec(gamma_sunset);

    hourAngle = CalcHourAngle(latitude, solarDec, false);
    delta = longitude - dRadToDeg(hourAngle);
    timeDiff = 4 * delta;
    setTimeGMT = 720 + timeDiff - eqTime;

    return setTimeGMT;
}

double SunsetSystem::calcSunriseGMT(int julDay, double latitude, double longitude)
{
    double gamma = CalcGamma(julDay);
    double eqTime = CalcEqofTime(gamma);
    double solarDec = CalcSolarDec(gamma);
    double hourAngle = CalcHourAngle(latitude, solarDec, true);
    double delta = longitude - dRadToDeg(hourAngle);
    double timeDiff = 4 * delta;
    double timeGMT = 720 + timeDiff - eqTime;

    double gamma_sunrise = CalcGamma2(julDay, timeGMT/60);
    eqTime = CalcEqofTime(gamma_sunrise);
    solarDec = CalcSolarDec(gamma_sunrise);
    hourAngle = CalcHourAngle(latitude, solarDec, 1);
    delta = longitude - dRadToDeg(hourAngle);
    timeDiff = 4 * delta;
    timeGMT = 720 + timeDiff - eqTime;

    return timeGMT;
}

double  SunsetSystem::calcSolNoonGMT(int julDay, double longitude)
{
    double gamma_solnoon = CalcGamma2(julDay, 12 + (longitude/15));
    double eqTime = CalcEqofTime(gamma_solnoon);
    double solNoonGMT = 720 + (longitude * 4) - eqTime;

    return solNoonGMT;
}

QDateTime SunsetSystem::GetSunrise()
{
    if (!GlobalStatsInstance.shouldUpdateSunrise())
    {
        return QDateTime(QDate::currentDate(),GlobalStatsInstance.getSunrise());
    }
    QDateTime currentDate = QDateTime::currentDateTimeUtc();
    int iJulianDay = currentDate.date().dayOfYear();
    double dLat = GlobalStatsInstance.getLatitude();
    double dLon = GlobalStatsInstance.getLongitude();

    double timeGMT = calcSunriseGMT(iJulianDay, dLat,dLon);


    if ((dLat > 66.4) && (iJulianDay > 79) && (iJulianDay < 267))
        timeGMT = findRecentSunrise(iJulianDay, dLat, dLon);
    else if ((dLat > 66.4) && ((iJulianDay < 83) || (iJulianDay > 263)))
        timeGMT = findNextSunrise(iJulianDay, dLat, dLon);
    else if((dLat < -66.4) && ((iJulianDay < 83) || (iJulianDay > 263)))
        timeGMT = findRecentSunrise(iJulianDay, dLat, dLon);
    else if((dLat < -66.4) && (iJulianDay > 79) && (iJulianDay < 267))
        timeGMT = findNextSunrise(iJulianDay, dLat, dLon);

    double dHour = timeGMT / 60;
    int iHour = (int)dHour;
    double dMinute = 60 * (dHour - iHour);
    int iMinute = (int)dMinute;
    double dSecond = 60 * (dMinute - iMinute);
    int iSecond = (int)dSecond;

    currentDate.setTime(QTime(iHour, iMinute, iSecond));
    GlobalStatsInstance.setSunrise(currentDate.time());
    return currentDate;
}

QDateTime SunsetSystem::GetSunset()
{
    if (!GlobalStatsInstance.shouldUpdateSunset())
    {
        return QDateTime(QDate::currentDate(),GlobalStatsInstance.getSunset());
    }
    QDateTime currentDate = QDateTime::currentDateTimeUtc();
    int iJulianDay = currentDate.date().dayOfYear();
    double dLat = GlobalStatsInstance.getLatitude();
    double dLon = GlobalStatsInstance.getLongitude();

    double timeGMT = calcSunsetGMT(iJulianDay, dLat,dLon);

    if ((dLat > 66.4) && (iJulianDay > 79) && (iJulianDay < 267))
        timeGMT = findRecentSunset(iJulianDay, dLat, dLon);
    else if ((dLat > 66.4) && ((iJulianDay < 83) || (iJulianDay > 263)))
        timeGMT = findNextSunset(iJulianDay, dLat, dLon);
    else if ((dLat < -66.4) && ((iJulianDay < 83) || (iJulianDay > 263)))
        timeGMT = findRecentSunset(iJulianDay, dLat, dLon);
    else if ((dLat < -66.4) && (iJulianDay > 79) && (iJulianDay < 267))
        timeGMT = findNextSunset(iJulianDay, dLat, dLon);

    double dHour = timeGMT / 60;
    int iHour = (int)dHour;
    double dMinute = 60 * (dHour - iHour);
    int iMinute = (int)dMinute;
    double dSecond = 60 * (dMinute - iMinute);
    int iSecond = (int)dSecond;

    currentDate.setTime(QTime(iHour, iMinute, iSecond));
    GlobalStatsInstance.setSunset(currentDate.time());
    return currentDate;
}

QDateTime SunsetSystem::GetSolarNoon()
{
    QDateTime currentDate = QDateTime::currentDateTimeUtc();
    int iJulianDay = currentDate.date().dayOfYear();
    double dLon = GlobalStatsInstance.getLongitude();

    double timeGMT = calcSolNoonGMT(iJulianDay, dLon);

    double dHour = timeGMT / 60;
    int iHour = (int)dHour;
    double dMinute = 60 * (dHour - iHour);
    int iMinute = (int)dMinute;
    double dSecond = 60 * (dMinute - iMinute);
    int iSecond = (int)dSecond;

    currentDate.setTime(QTime(iHour, iMinute, iSecond));
    return currentDate;
}

double SunsetSystem::getSinPercent()
{
    QTime sunRise = GetSunrise().time();
    QTime sunSet = GetSunset().time();
    QTime currentTime = QTime::currentTime();

    if (currentTime < sunRise || currentTime > sunSet)
        return 0.;

    int dayLong = sunRise.secsTo(sunSet);
    int secsToSunset = currentTime.secsTo(sunSet);
    return sin(double(secsToSunset)/double(dayLong)*M_PI);
}

double SunsetSystem::getLinPercent()
{
    QTime sunRise = GetSunrise().time();
    QTime sunSet = GetSunset().time();
    QTime currentTime = QTime::currentTime();

    if (currentTime < sunRise || currentTime > sunSet)
        return 0.;

    int dayLong = sunRise.secsTo(sunSet);
    int secsToSunset = currentTime.secsTo(sunSet);
    return double(secsToSunset)/double(dayLong);
}

double SunsetSystem::findRecentSunrise(int julDay, double latitude, double dLongitude)
{
    int jday = julDay;

    double dTime = calcSunriseGMT(jday, latitude, dLongitude);

    while(!IsInteger(dTime) )
    {
        jday--;
        if (jday < 1)
            jday = 365;
        dTime = calcSunriseGMT(jday, latitude, dLongitude);
    }

    return jday;
}

bool SunsetSystem::IsInteger(double value)
{
    int iTemp = (int)value;
    double dTemp = value - iTemp;
    if(dTemp == 0)
        return true;
    else
        return false;
}

double SunsetSystem::findRecentSunset(int iJulDay, double dLatitude, double dLongitude)
{
    int jday = iJulDay;

    double dTime = calcSunsetGMT(jday, dLatitude, dLongitude);

    while(!IsInteger(dTime) )
    {
        jday--;
        if (jday < 1)
            jday = 365;
        dTime = calcSunsetGMT(jday, dLatitude, dLongitude);
    }

    return jday;
}

double SunsetSystem::findNextSunrise(int julDay, double latitude, double longitude)
{
    int jday = julDay;

    double dTime = calcSunriseGMT(jday, latitude, longitude);

    while(!IsInteger(dTime) )
    {
        jday++;
        if (jday > 366)
            jday = 1;
        dTime = calcSunriseGMT(jday, latitude, longitude);
    }

    return jday;
}

double SunsetSystem::findNextSunset(int julDay, double latitude, double longitude)
{
    int jday = julDay;

    double dTime = calcSunsetGMT(jday, latitude, longitude);

    while(!IsInteger(dTime) )
    {
        jday++;
        if (jday > 366)
            jday = 1;
        dTime = calcSunsetGMT(jday, latitude, longitude);
    }
    return jday;
}

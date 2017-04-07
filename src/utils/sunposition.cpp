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
    int utcOffset = GlobalStatsInstance.getUTCOffset();
    QDateTime currentDate = QDateTime::currentDateTimeUtc();
    currentDate = currentDate.addSecs(utcOffset);
    int iJulianDay = currentDate.date().dayOfYear();
    double dLat = GlobalStatsInstance.getLatitude();
    double dLon = GlobalStatsInstance.getLongitude();

    qDebug() << "getSunrise::lat/lon" << dLat << dLon;
    QTime sunrise = getSunState(dLat, dLon, iJulianDay, true);
    sunrise = sunrise.addSecs(utcOffset);
    currentDate.setTime(sunrise);
    GlobalStatsInstance.setSunrise(sunrise);
    return currentDate;
}

QDateTime SunsetSystem::GetSunset()
{
    if (!GlobalStatsInstance.shouldUpdateSunset())
    {
        return QDateTime(QDate::currentDate(),GlobalStatsInstance.getSunset());
    }
    int utcOffset = GlobalStatsInstance.getUTCOffset();
    QDateTime currentDate = QDateTime::currentDateTimeUtc();
    currentDate = currentDate.addSecs(utcOffset);
    int iJulianDay = currentDate.date().dayOfYear();
    double dLat = GlobalStatsInstance.getLatitude();
    double dLon = GlobalStatsInstance.getLongitude();

    qDebug() << "getSunrise::lat/lon" << dLat << dLon;
    QTime sunrise = getSunState(dLat, dLon, iJulianDay, false);
    sunrise = sunrise.addSecs(utcOffset);
    currentDate.setTime(sunrise);
    GlobalStatsInstance.setSunset(sunrise);
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
    int sunriseHour = sunRise.hour();
    int sunsetHour = sunSet.hour();
    if (sunriseHour > 0)
        sunriseHour -= 1;
    if (sunsetHour < 23)
        sunsetHour += 1;
    sunRise.setHMS(sunriseHour,sunRise.minute(), sunRise.second());
    sunSet.setHMS(sunsetHour, sunSet.minute(), sunSet.second());

    qDebug() << "SUNSET/SUNRISE" << sunRise << sunSet;
    QTime currentTime = QDateTime::currentDateTimeUtc().time().addSecs(GlobalStatsInstance.getUTCOffset());

    if (currentTime < sunRise || currentTime > sunSet)
        return 0.;

    int dayLong = sunRise.secsTo(sunSet);
    qDebug() << dayLong;
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

QTime SunsetSystem::getSunState(double dLat, double dLon, int iJulianDay, bool isSunrise)
{
    double toRAD = M_PI/180.;
    double lngHour = dLon / 15;
    double t;
    if (isSunrise)
        t = iJulianDay + ((6 -lngHour)/24);
    else
        t = iJulianDay + ((18 -lngHour)/24);
    double sunMean = (0.9856 * t) - 3.289;
    double l = sunMean + (1.916 * sin(M_PI*sunMean)) + (0.02 * sin(toRAD*2*sunMean)) + 282.634;

    if      (l < 0)     l = l + 360;
    else if (l > 360)   l = l - 360;

    double RA = 1/toRAD * atan(0.91764 * tan(toRAD * l));
    if      (RA < 0)     RA = RA + 360;
    else if (RA > 360)   RA = RA - 360;
    double lQuad = floor(l/90) * 90;
    double rQuad = floor(RA/90) * 90;
    RA = RA + (lQuad - rQuad);
    RA /= 15;
    double sinDec = 0.39782 * sin(toRAD*l);
    double cosDec = cos(asin(sinDec));
    double cosH = cos(toRAD * 90.8) - (sinDec * sin(toRAD * dLat)) / (cosDec * cos(toRAD*dLat));
    if (cosH > 1)
        qDebug() << "error the sun never rises on this location";
    if (cosH < -1)
        qDebug() << "the sun never sets on this location";
    double h;
    if (isSunrise)
        h = 360 - 1/toRAD * acos(cosH);
    else
        h = 1/toRAD * acos(cosH);
    h = h/15;
    double T = h + RA - (0.06571 *t) - 6.622;
    double ut = T - lngHour;
    if (ut < 0) ut += 24;
    if (ut > 24) ut -= 24;
    int uth = ut;
    int utm = (ut - uth) * 60.;
    return QTime(uth, utm);
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

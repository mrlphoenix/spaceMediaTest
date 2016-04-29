#ifndef SUNPOSITION_H
#define SUNPOSITION_H

#include <QDateTime>

class SunsetSystem
{
public:
    SunsetSystem(){}

    //Returns in GMT
    QDateTime GetSunset();
    QDateTime GetSunrise();
    QDateTime GetSolarNoon();
    double getSinPercent();
    double getLinPercent();
private:
    double dRadToDeg(double dAngleRad)
    {
        return (180 * dAngleRad / M_PI);
    }
    double dDegToRad(double dAngleDeg)
    {
        return (M_PI * dAngleDeg / 180);
    }

    double CalcGamma(int iJulianDay)
    {
        return (2.0 * M_PI / 365.0) * (iJulianDay - 1);
    }

    double CalcGamma2(int iJulianDay, int iHour)
    {
        return (2.0 * M_PI / 365) * (iJulianDay - 1 + (iHour/24.0));
    }

    double CalcEqofTime(double dGamma)

    {
        return (229.18 * (0.000075 + 0.001868 * cos(dGamma) - 0.032077 * sin(dGamma)- 0.014615 * cos(2 * dGamma) - 0.040849 *  sin(2 * dGamma)));
    }

    double CalcSolarDec(double dGamma)
    {
        return (0.006918 - 0.399912 * cos(dGamma) + 0.070257 * sin(dGamma) - 0.006758 * cos(2 * dGamma) + 0.000907 *  sin(2 * dGamma));
    }

    int CalcDayLength(double dHourAngle)
    {
    //	Return the length of the day in minutes.
        return (int)((2.0 * abs(dRadToDeg(dHourAngle))) / 15.0);
    }


    double CalcHourAngle(double dLat, double dSolarDec, bool bTime);

    double calcSunsetGMT(int julDay, double latitude, double longitude);
    double calcSunriseGMT(int julDay, double latitude, double longitude);
    double calcSolNoonGMT(int julDay, double longitude);

    double findRecentSunrise(int julDay, double latitude, double dLongitude);
    double findRecentSunset(int iJulDay, double dLatitude, double dLongitude);

    double findNextSunrise(int julDay, double latitude, double longitude);
    double findNextSunset(int julDay, double latitude, double longitude);

    bool IsInteger(double value);

    double lat, lon;
};

#endif // SUNPOSITION_H

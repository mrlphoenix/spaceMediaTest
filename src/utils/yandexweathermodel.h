#ifndef YANDEXWEATHERMODEL_H
#define YANDEXWEATHERMODEL_H
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QVector>
#include <QByteArray>

class YandexWeatherModel
{
public:
    YandexWeatherModel();
    static YandexWeatherModel fromXml(QByteArray data);

    QString city;
    QString country;
    int region;

    struct Temperature
    {
        QString color;
        QString plate;
        QString type;
        int value;
    };


    struct FactWeather
    {
        struct Station
        {
            QString lang;
            QString text;
            int distance;
        };
        QVector<Station> stations;
        QDateTime observation_time;
        Temperature temperature;
        QString weather_type;
        QString wind_direction;
        double wind_speed;
        int humidity;
        int pressure;
        int mslp_pressure;
    };
    FactWeather fact;
    FactWeather yesterday;
    struct Informer
    {
        QVector<Temperature> temperatures;
    };
    Informer informer;
    struct Day
    {
        QDate date;
        QString sunrise;
        QString sunset;

        struct MoonPhase
        {
            QString code;
            int value;
        };
        MoonPhase moonPhase;
        QString moonrise;
        QString moonset;

        struct dayPart
        {
            int type_id;
            QString type;
            int temperature_from;
            int temperature_to;
            QString weather_type;
            QString wind_direction;
            double wind_speed;
            int humidity;
            int pressure;
            int mslp_pressure;
        };
        QVector<dayPart> dayParts;
    };
    QVector<Day> days;
};

#endif // YANDEXWEATHERMODEL_H

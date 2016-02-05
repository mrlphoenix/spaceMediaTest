#include "yandexweathermodel.h"
#include <QXmlAttributes>
#include <QXmlStreamReader>
#include <QDebug>

YandexWeatherModel::YandexWeatherModel()
{

}

YandexWeatherModel YandexWeatherModel::fromXml(QByteArray data)
{
    QXmlStreamReader xml(data);
    YandexWeatherModel result;
    QString node = "";
    int dayIndex = -1;
    int dayPartIndex = -1;

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            if (xml.name().toString() == "forecast")
            {
                foreach (const QXmlStreamAttribute& atr, xml.attributes())
                {
                    if (atr.name().toString() == "city")
                        result.city = atr.value().toString();
                    else if (atr.name().toString() == "country")
                        result.country = atr.value().toString();
                    else if (atr.name().toString() == "region")
                        result.region = atr.value().toInt();
                }
            }
            else if (xml.name().toString() == "fact")
                node = "fact";
            else if (xml.name().toString() == "yesterday")
                node = "yesterday";
            else if (xml.name().toString() == "informer")
                node = "informer";
            else if (xml.name().toString() == "station")
            {
                YandexWeatherModel::FactWeather::Station station;
                foreach (const QXmlStreamAttribute& atr, xml.attributes())
                {
                    if (atr.name().toString() == "lang")
                        station.lang = atr.value().toString();
                    else if (atr.name().toString() == "distance")
                        station.distance = atr.value().toInt();
                }
                station.text = xml.readElementText();
                if (node == "fact")
                    result.fact.stations.append(station);
                else
                    result.yesterday.stations.append(station);
            }
            else if (xml.name().toString() == "observation_time")
            {
                QDateTime obsTime = QDateTime::fromString(xml.readElementText(), "yyyy-MM-ddTHH:mm:ss");
                if (node == "fact")
                    result.fact.observation_time = obsTime;
                else
                    result.yesterday.observation_time = obsTime;
            }
            else if (xml.name().toString() == "temperature")
            {
                YandexWeatherModel::Temperature temperature;
                foreach (const QXmlStreamAttribute& atr, xml.attributes())
                {
                    if (atr.name().toString() == "color")
                        temperature.color = atr.value().toString();
                    else if (atr.name().toString() == "type")
                        temperature.type = atr.value().toString();
                    else if (atr.name().toString() == "plate")
                        temperature.plate = atr.value().toString();
                }
                temperature.value = xml.readElementText().toInt();
                if (node == "fact")
                    result.fact.temperature = temperature;
                else if (node == "yesterday")
                    result.yesterday.temperature = temperature;
                else if (node == "informer")
                    result.informer.temperatures.append(temperature);
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].temperature = temperature.value;
            }
            else if (xml.name().toString() == "weather_type")
            {
                if (node == "fact")
                    result.fact.weather_type = xml.readElementText();
                else if (node == "yesterday")
                    result.yesterday.weather_type = xml.readElementText();
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].weather_type = xml.readElementText();
            }
            else if (xml.name().toString() == "wind_direction")
            {
                if (node == "fact")
                    result.fact.wind_direction = xml.readElementText();
                else if (node == "yesterday")
                    result.yesterday.wind_direction = xml.readElementText();
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].wind_direction = xml.readElementText();
            }
            else if (xml.name().toString() == "wind_speed")
            {
                if (node == "fact")
                    result.fact.wind_speed = xml.readElementText().toDouble();
                else if (node == "yesterday")
                    result.yesterday.wind_speed = xml.readElementText().toDouble();
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].wind_speed = xml.readElementText().toDouble();
            }
            else if (xml.name().toString() == "humidity")
            {
                if (node == "fact")
                    result.fact.humidity = xml.readElementText().toInt();
                else if (node == "yesterday")
                    result.yesterday.humidity = xml.readElementText().toInt();
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].humidity = xml.readElementText().toInt();
            }
            else if (xml.name().toString() == "pressure")
            {
                if (node == "fact")
                    result.fact.pressure = xml.readElementText().toInt();
                else if (node == "yesterday")
                    result.yesterday.pressure = xml.readElementText().toInt();
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].pressure = xml.readElementText().toInt();
            }
            else if (xml.name().toString() == "mslp_pressure")
            {
                if (node == "fact")
                    result.fact.mslp_pressure = xml.readElementText().toInt();
                else if (node == "yesterday")
                    result.yesterday.mslp_pressure = xml.readElementText().toInt();
                else if (node == "day_part")
                    result.days[dayIndex].dayParts[dayPartIndex].mslp_pressure = xml.readElementText().toInt();
            }
            else if (xml.name().toString() == "day")
            {
                YandexWeatherModel::Day day;
                dayIndex++;
                dayPartIndex = -1;
                node = "day";
                foreach (const QXmlStreamAttribute& atr, xml.attributes())
                    if (atr.name().toString() == "date")
                        day.date = QDate::fromString(atr.value().toString(), "yyyy-MM-dd");
                result.days.append(day);
            }
            else if (xml.name().toString() == "sunrise")
            {
                result.days[dayIndex].sunrise = xml.readElementText();
            }
            else if (xml.name().toString() == "sunset")
            {
                result.days[dayIndex].sunset = xml.readElementText();
            }
            else if (xml.name().toString() == "moon_phase")
            {
                YandexWeatherModel::Day::MoonPhase moonPhase;
                foreach (const QXmlStreamAttribute& atr, xml.attributes())
                    if (atr.name().toString() == "code")
                        moonPhase.code = atr.value().toString();
                moonPhase.value = xml.readElementText().toInt();
                result.days[dayIndex].moonPhase = moonPhase;
            }
            else if (xml.name().toString() == "moonrise")
            {
                result.days[dayIndex].moonrise = xml.readElementText();
            }
            else if (xml.name().toString() == "moonset")
            {
                result.days[dayIndex].moonset = xml.readElementText();
            }
            else if (xml.name().toString() == "day_part")
            {
                YandexWeatherModel::Day::dayPart dayPart;
                dayPartIndex++;
                node = "day_part";
                foreach (const QXmlStreamAttribute& atr, xml.attributes())
                {
                    if (atr.name().toString() == "type_id")
                        dayPart.type_id = atr.value().toInt();
                    else if (atr.name().toString() == "type")
                        dayPart.type = atr.value().toString();
                }
                result.days[dayIndex].dayParts.append(dayPart);
            }
            else if (xml.name().toString() == "temperature_from")
            {
                result.days[dayIndex].dayParts[dayPartIndex].temperature_from = xml.readElementText().toInt();
            }
            else if (xml.name().toString() == "temperature_to")
            {
                result.days[dayIndex].dayParts[dayPartIndex].temperature_to = xml.readElementText().toInt();
            }
        }
        else if (xml.hasError())
        {
            qDebug() << "XML error: " << xml.errorString() << "\n";
        }
        else if (xml.atEnd())
        {
            qDebug() << "Reached end, done" << "\n";
        }
    }
    return result;
}


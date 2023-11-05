#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QMap>


class DeviceParameters
{
private:
    QMap<QString, QString> parameters;
public:
    DeviceParameters();
};

#ifndef DEVICEDATAFILES_H
#define DEVICEDATAFILES_H


#include <QMap>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "serviceParemeters.h"

class DeviceDataFile
{

private:
    ServiceParemeters *servicesData;

    QString path = "service_data_v2.json";

    void jsonFileWrite();
    void jsonFileRead();

public:
    DeviceDataFile();
//    ~DeviceDataFile();

    void setBuffer(ServiceParemeters *servicesData);

    void saveServicesData();
    void readServicesData();
    void clearServicesData();

};

#endif // DEVICEDATAFILES_H

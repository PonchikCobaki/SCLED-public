#include "serviceParemeters.h"

ServiceParemeters::ServiceParemeters()
{

}

ServiceParemeters::ServiceParemeters(QMap<QString, QHostAddress> devicesMap, QMap<QString, QString> nameMap)
{
    qDebug() << "Service_paremeters constructor";

    this->devicesMap = devicesMap;
    this->nameMap = nameMap;
    qDebug() << devicesMap;
    qDebug() << nameMap;

    for (const auto& pair : this->nameMap.toStdMap()) {
        this->reverseNameMap.insert(pair.first, pair.second);
    }
    qDebug() << reverseNameMap;

    for (auto service : this->nameMap.keys()){
        availableMap.insert(service, 0);
    }
    qDebug() << availableMap;
}

ServiceParemeters &ServiceParemeters::operator=(const ServiceParemeters &other)
{
    if (this != &other) {
        this->devicesMap = other.devicesMap;
        this->nameMap = other.nameMap;
        this->reverseNameMap = other.reverseNameMap;
        this->availableMap = other.availableMap;
    }
    qDebug() << "Error self - assignment";
    return *this;
}

const QMap<QString, QHostAddress>& ServiceParemeters::getServices()
{
    return devicesMap;
}

const QMap<QString, QString>& ServiceParemeters::getUName()
{
    return nameMap;
}

const QMap<QString, QString>& ServiceParemeters::getRevUName()
{
    return reverseNameMap;
}

bool ServiceParemeters::getAvailableStatus(const QString service)
{
    return this->availableMap.value(service);
}

void ServiceParemeters::insertService(const QString service, const QHostAddress IP)
{
    this->devicesMap.insert(service, IP);
}

void ServiceParemeters::insertUName(const QString service, const QString name)
{
    this->nameMap.insert(service, name);
    this->reverseNameMap.insert(name, service);
}

void ServiceParemeters::changeUName(const QString service, const QString oldName, const QString newName)
{
    this->reverseNameMap.take(oldName);
    insertUName(service, newName);
}

void ServiceParemeters::updateAvailable(const QString service, const bool state)
{
    availableMap.insert(service, state);
}



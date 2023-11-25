#include "deviceDataFiles.h"


DeviceDataFile::DeviceDataFile()
{

}

void DeviceDataFile::setBuffer(ServiceParemeters *servicesData)
{
    this->servicesData = servicesData;
}

void DeviceDataFile::saveServicesData()
{
    QJsonObject jsonServiceName;

    for (const auto& pair : servicesData->getUName().toStdMap()){
        jsonServiceName.insert(pair.first, pair.second);
    }


//    auto nameId = servicesData.getName().begin();
//    while (true) {
//        if (ipId == servicesData.getDevices().end())
//            break;
//        QJsonArray  jsonServiceInfo;
//        jsonServiceInfo.append( ipId.value().toString() );
//        jsonServiceInfo.append( nameId.value() );
//        jsonServiceName.insert(ipId.key(), jsonServiceInfo);

//        ++ipId; ++userNameId;
//    }

    QJsonDocument jsonDoc(jsonServiceName);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void DeviceDataFile::readServicesData()
{
    qDebug() << "read Service data ";

    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray rawJsonData = file.readAll();
        file.close();

        QJsonObject jsonServiceName = QJsonDocument::fromJson(rawJsonData).object();


        for (auto it = jsonServiceName.begin(); it != jsonServiceName.end(); ++it) {
            QString serviceName = it.key();
            QString userName = it.value().toString();

//            servicesData->insertDevice(serviceName, QHostAddress());
            servicesData->insertUName(serviceName, userName);

        }
    }

}

void DeviceDataFile::clearServicesData()
{
    QFile file(path);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.resize(0);
        file.close();
    }
}

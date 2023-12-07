#ifndef SERVICEPAREMETERS_H
#define SERVICEPAREMETERS_H

#include <QNetworkAccessManager>
#include <QMap>

class ServiceParemeters
{
private:
    QMap<QString, QHostAddress> devicesMap; // Service name, device IP
    QMap<QString, QString> nameMap; // Service name, User device name
    QMap<QString, QString> reverseNameMap; // User device name,  Service

    QMap<QString, bool> availableMap; // Service name, available flag

public:
    ServiceParemeters();
    ServiceParemeters(QMap<QString, QHostAddress> devicesMap, QMap<QString, QString> nameMap);
//    ~ServiceParemeters();

    ServiceParemeters& operator=(const ServiceParemeters& other);

    const QMap<QString, QHostAddress>& getServices();       // return service name, device IP
    const QMap<QString, QString>& getUName();               // return service name, user device name
    const QMap<QString, QString>& getRevUName();            // return user device name, service name
    const QMap<QString, QString>& getaAvailable();          // return service name name, available flag

    bool getAvailableStatus(const QString service);
    
    void insertService(const QString service, const QHostAddress IP);
    void insertUName(const QString service, const QString name);    // update value of key or add new key
    void changeUName(const QString service, const QString oldName, const QString newName);    // change key and value
    void deliteService(const QString service);

    void updateAvailable(const QString service, const bool state);
};

#endif // SERVICEPAREMETERS_H

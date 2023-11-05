#include "deviceparameters.h"

DeviceParameters::DeviceParameters()
{



// Создание объекта QMap<QString, QString>
    QMap<QString, QString> myMap;
    myMap.insert("key1", "value1");
    myMap.insert("key2", "value2");

    // Создание JSON-объекта
    QJsonObject jsonObject;

    for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        jsonObject.insert(it.key(), it.value());
    }

    // Создание JSON-документа
    QJsonDocument jsonDocument(jsonObject);

    // Сохранение JSON-документа в файл
    QFile file("output.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDocument.toJson());
        file.close();
    }
}

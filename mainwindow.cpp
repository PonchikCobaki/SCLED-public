#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readServiceInfoInFile();
    serviceSerachRestart();
    currentDeviceComboUpdate();
    chekState();

    manager = new QNetworkAccessManager(this);
    connect(manager,
            &QNetworkAccessManager::finished,
            [=] (QNetworkReply *reply) {
                if (reply->error()) {
                    qDebug() << "Error: " << reply->errorString();
                    ui->plainTextEdit->appendPlainText(reply->errorString());
                    return;
                }

                QString answer = reply->readAll();
                if (!answer.isEmpty()){
                    jsonParse(answer);
                    chekState();
                    ui->plainTextEdit->clear();
//                    ui->plainTextEdit->appendPlainText(getCurrentDeviceAddress());
                    ui->plainTextEdit->appendPlainText(answer);
                }

            });


}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::serviceSerachRestart()
{
    if (cache != nullptr)
        delete cache;


    if (browser != nullptr)
        delete browser;

    cache = new QMdnsEngine::Cache();
    browser = new QMdnsEngine::Browser(&server, "_http._tcp.local.", cache);
    QObject::connect(browser,
                     &QMdnsEngine::Browser::serviceAdded,
                     [=](const QMdnsEngine::Service &service) {
                         qDebug() << "Service added:" << service.name();
//                         qDebug() << service.name().contains("CLED") << "discovered!";
//                         ui->plainTextEdit->appendPlainText(service.name());
                         //                        foreach (const QByteArray& key, service.attributes().keys()) {
                         //                            qDebug() << "param: " + key + ", Value: " + service.attributes().value(key) + "\n";
                         //                        }
                         //                        ui->plainTextEdit->appendPlainText(service.attributes()
                         //                        ui->plainTextEdit->appendPlainText(service.port());
                         serviceResolver(service);
                     });


    QObject::connect(browser,
                     &QMdnsEngine::Browser::serviceUpdated,
                     [=](const QMdnsEngine::Service &service) {
                         qDebug() << "Service updated:" << service.name();
                         ui->plainTextEdit->appendPlainText(service.name() + " updated");
                         serviceResolver(service);
                     });

    QObject::connect(browser,
                     &QMdnsEngine::Browser::serviceRemoved,
                     [=](const QMdnsEngine::Service &service) {
                         qDebug() << "Service removed:" << service.name();
                         ui->plainTextEdit->appendPlainText(service.name() + " removed");
                     });
}

void MainWindow::serviceResolver(const QMdnsEngine::Service &service)
{
    QMdnsEngine::Resolver *resolver = new QMdnsEngine::Resolver(&server, service.hostname(), cache);
    QObject::connect(resolver, &QMdnsEngine::Resolver::resolved,
                     [=](const QHostAddress &address) {
                         qDebug() << "resolved to" << address;
                         devicesMap.insert(service.name(), address);
                         if (!nameMap.contains(service.name())){
                             QString name = QString("Device " + QString::number(nameMap.size() + 1));
                             nameMap.insert(service.name(), name);
                             reverseNameMap.insert(name, service.name());
                             currentDeviceComboUpdate();
//                             saveServiceInfoInFile();

                         }

                         if (jsonDocDeviceParameters.isEmpty()){
                             requestParamsFromDevice();
                         }

                         resolver->deleteLater();
                     });

}

void MainWindow::requestParamsFromDevice()
{
    // Get
    QNetworkRequest request;
    request.setUrl(QUrl::fromUserInput(getCurrentDeviceAddress()));

    manager->get(request);
}

void MainWindow::updateParamsOnDevice()
{
    //    QUrlQuery postData;
    //    postData.addQueryItem("state", "run");
    //    postData.addQueryItem("hsv", getHexHSVColor());

    //    QNetworkRequest request(QUrl::fromUserInput(getCurrentDeviceAddress() + "/api"));
    ////    request.setHeader(QNetworkRequest::ContentTypeHeader,
    ////                      "application/x-www-form-urlencoded");

    //    manager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());


    ////    QUrl url = QUrl::fromUserInput(getCurrentDeviceAddress() + "/api");

    ////    url.setQuery(query.query());
    ////    request.setUrl(url);
    ///
    ///


    QNetworkRequest request;

    QUrl url = QUrl::fromUserInput(getCurrentDeviceAddress() + "/api");
    QUrlQuery query;
    query.addQueryItem("state", state);
    query.addQueryItem("hsv", getHexHSVColor());
    url.setQuery(query.query());

    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
//    manager->get(request);
    manager->post(request, QByteArray());


}

QString MainWindow::getCurrentDeviceAddress()
{
    if (devicesMap.isEmpty() || ui->selectedDeviceComboBox->currentIndex() == -1)
        return "";

    QString serviceName = reverseNameMap.value(ui->selectedDeviceComboBox->currentText());

    QHostAddress address = devicesMap.value(serviceName);
    qDebug() << "address" << address;

    return address.toString();
}

QString MainWindow::getHexHSVColor()
{
    int h = hsv[0].toInt();
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();
    QString hexColor = QString("%1%2%3").arg(h , 2, 16).arg(s, 2, 16).arg(v, 2, 16);
    if (hexColor.contains(" "))
        hexColor.replace(" ", "0");
    qDebug() << hexColor;
    return hexColor;
}


void MainWindow::jsonParse(QString jsonString)
{
    jsonDocDeviceParameters = QJsonDocument::fromJson(jsonString.toUtf8());
    jsonDeviceParameters = jsonDocDeviceParameters.object();

    hsv = jsonDeviceParameters.value("hsv").toArray();

    updateColorItemUi();

    //    // Пример вывода всех ключей и значений в отладочное окно
    //        for (auto it = jsonDeviceParameters.begin(); it != jsonDeviceParameters.end(); ++it) {
    //        QString key = it.key();
    //        QJsonValue value = it.value();

    //        qDebug().noquote() << key << ": ";
    //        if (value.isDouble()) {
    //            qDebug().noquote() << QString::number(value.toInt());
    //        } else if (value.isString()) {
    //            qDebug().noquote() << value.toString();
    //        } else if(value.isArray()) {
    //            QJsonArray jsonArray = value.toArray();

    //            // Выводим каждый элемент массива
    //            for (const QJsonValue& value : jsonArray) {
    //                // Выводим значение элемента в отладочное окно
    //                qDebug() << "Array value:" << value.toString();
    //            }
    //        } else {
    //            qDebug() << "Неизвестный тип данных";
    //        }
    //    }
}

void MainWindow::resolveJsonParse()
{
    jsonDeviceParameters = jsonDocDeviceParameters.object();
    hsv = jsonDeviceParameters.value("hsv").toArray();

    QColor color = QColor();
    int h = hsv[0].toInt() / 255.0 * 360.0;
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();

    color.setHsv(h, s, v);
    qDebug() << color;

    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, color.rgb());
    ui->colorPushButton->setAutoFillBackground(true);
    ui->colorPushButton->setPalette(Pal);
}

void MainWindow::jsonFileWrite()
{
    QFile file("service_data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDocDeviceParameters.toJson());
        file.close();
    }
}

void MainWindow::jsonFileRead()
{
    QFile file("service_data.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        jsonDocDeviceParameters = QJsonDocument::fromJson(jsonData);
        file.close();
        resolveJsonParse();
    }
}

void MainWindow::saveServiceInfoInFile()
{
    QJsonObject jsonServiceName;

    auto ipId = devicesMap.begin();
    auto userNameId = nameMap.begin();
    while (true) {
        if (ipId == devicesMap.end())
            break;
        QJsonArray  jsonServiceInfo;
        jsonServiceInfo.append( ipId.value().toString() );
        jsonServiceInfo.append( userNameId.value() );
        jsonServiceName.insert(ipId.key(), jsonServiceInfo);

        ++ipId; ++userNameId;
    }

    QJsonDocument jsonDoc(jsonServiceName);

    QFile file("service_data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void MainWindow::readServiceInfoInFile()
{
    QFile file("service_data.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray rawJsonData = file.readAll();
        file.close();

        QJsonObject jsonServiceName = QJsonDocument::fromJson(rawJsonData).object();

        for (auto it = jsonServiceName.begin(); it != jsonServiceName.end(); ++it) {
            QString serviceName = it.key();
            QString deviceIp = it.value().toArray().at(0).toString();
            QString userName = it.value().toArray().at(1).toString();

            devicesMap.insert(serviceName, QHostAddress(deviceIp));
            nameMap.insert(serviceName, userName);
            reverseNameMap.insert(userName, serviceName);

        }

        qDebug() << "read Service data " ;
        qDebug() << devicesMap;
        qDebug() << nameMap;
        qDebug() << reverseNameMap;
    }
}

void MainWindow::clearServiceInfoInFile()
{
    QFile file("service_data.json");

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.resize(0);
        file.close();
    }
}

void MainWindow::onOnColorChanged(const QColor &color)
{
    int h, s, v;
    color.getHsv(&h, &s, &v);
    hsv[0] = int((h / 360.0 * 255.0));
    hsv[1] = s;
    hsv[2] = v;


    updateColorItemUi();
    updateParamsOnDevice();
    // todo add set spinboxes or sliders
}

void MainWindow::chekState()
{

    QString s = jsonDeviceParameters.value("state").toString();
    state = s;
    if (s == "run"){
        ui->onButton->setChecked(true);
    }
    else if (s == "off"){
        ui->onButton->setChecked(false);
    }
}

void MainWindow::currentDeviceComboUpdate()
{
    ui->selectedDeviceComboBox->clear();
    for (const QString &userDeviceName : reverseNameMap.keys()) {
        ui->selectedDeviceComboBox->addItem(userDeviceName);
    }

}

void MainWindow::updateColorItemUi()
{
//    hsv = jsonDeviceParameters.value("hsv").toArray();


    int h = hsv[0].toInt() / 255.0 * 360.0;
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();

    QColor color = QColor();
    color.setHsv(h, s, v);

    qDebug() << "update color" << color;

    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, color.rgb());
    ui->colorPushButton->setAutoFillBackground(true);
    ui->colorPushButton->setPalette(Pal);



    ui->horizontalSliderHue->setSliderPosition(h);
    ui->horizontalSliderSaturation->setSliderPosition(s / 255.0 * 100.0);
    ui->horizontalSliderValue->setSliderPosition(v / 255.0 * 100.0);

    ui->spinBoxHue->setValue(h);
    ui->spinBoxSaturation->setValue(s / 255.0 * 100.0);
    ui->spinBoxValue->setValue(v / 255.0 * 100.0);


}



void MainWindow::on_colorPushButton_clicked()
{

    if (colorDialog != nullptr){
        delete colorDialog;
        colorDialog = nullptr;
    }


    colorDialog = new QColorDialog(this);
    connect(colorDialog, &QColorDialog::colorSelected, [=](const QColor &color){
        onOnColorChanged(color);
    });
    //    connect(colorDialog, &QColorDialog::currentColorChanged, [=](const QColor &color){
    //        onOnColorChanged(color);
    //    });



    QColor color = QColor();

    int h = hsv[0].toInt() / 255.0 * 360.0;
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();
//    qDebug() << h << s << v;

    color.setHsv(h, s, v);

    colorDialog->setCurrentColor(color);
    colorDialog->show();

//    updateColorItemUi();
//    QPalette Pal(palette());
//    Pal.setColor(QPalette::Button, color.rgb());
//    ui->colorPushButton->setPalette(Pal);
//    ui->colorPushButton->setAutoFillBackground(true);


}


void MainWindow::on_selectedDeviceComboBox_currentTextChanged(const QString &arg1)
{
    qDebug() << "currentTextChanged: " << arg1;
    qDebug() << "combo count: " << ui->selectedDeviceComboBox->count();
    qDebug() << "device count: " << devicesMap.size();

    if (ui->selectedDeviceComboBox->count() == devicesMap.size()){
        requestParamsFromDevice();
    }
}

//void MainWindow::on_selectedDeviceComboBox_textActivated(const QString &arg1)
//{
//    qDebug() << "textActivated: " << arg1;
//    qDebug() << "combo count: " << ui->selectedDeviceComboBox->count();
//    qDebug() << "device count: " << devicesMap.size();

//    if (ui->selectedDeviceComboBox->count() == devicesMap.size()){
//        requestParamsFromDevice();
//    }
//}


void MainWindow::on_deviceSearch_clicked()
{
    qDebug("mDNS update");
    ui->plainTextEdit->clear();
    serviceSerachRestart();
}




void MainWindow::on_updateParameters_clicked()
{
    requestParamsFromDevice();
}


void MainWindow::on_jsonSaveButton_clicked()
{
    saveServiceInfoInFile();
    qDebug() << "write Service Info \n" << jsonDeviceParameters;
}


void MainWindow::on_jsonReadButton_clicked()
{
    readServiceInfoInFile();
    qDebug() << "read Service Info \n" << devicesMap;
    qDebug() << "read Service Info \n" << nameMap;
    currentDeviceComboUpdate();
    requestParamsFromDevice();
}

void MainWindow::on_fileClearButton_clicked()
{
    clearServiceInfoInFile();
    devicesMap.clear();
    nameMap.clear();
    reverseNameMap.clear();
    currentDeviceComboUpdate();
}


void MainWindow::on_horizontalSliderHue_sliderReleased()
{
    int value = ui->horizontalSliderHue->value();
    hsv[0] = int((value / 360.0 * 255.0));
    updateColorItemUi();
    updateParamsOnDevice();
}


void MainWindow::on_horizontalSliderSaturation_sliderReleased()
{
    int value = ui->horizontalSliderSaturation->value();
    hsv[1] = int((value / 100.0 * 255.0));
    updateColorItemUi();
    updateParamsOnDevice();
}


void MainWindow::on_horizontalSliderValue_sliderReleased()
{
    int value = ui->horizontalSliderValue->value();
    hsv[2] = int((value / 100.0 * 255.0));
    updateColorItemUi();
    updateParamsOnDevice();
}


void MainWindow::on_onButton_clicked(bool checked)
{
    if (checked){
        state = "run";
    } else {
        state = "off";
    }
    updateParamsOnDevice();
}








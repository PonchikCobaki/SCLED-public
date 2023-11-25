#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gradientPaletts.cpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gradientNameComboBox->addItems(gradients);

//    ui->colorPushButton->setStyleSheet("background-color: white;");



    deactivateColorControls();

//    ui->ColorSettingsGroupBox->setEnabled(0);

    servicesFile.setBuffer(&servicesData);
    servicesFile.readServicesData();

    serviceSerachRestart();

    currentDeviceComboUpdate();
    checkState();

    setEnabledSolidMode();

//    checkMode();

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
                    checkState();
                    checkMode();
                    ui->plainTextEdit->clear();
//                    ui->plainTextEdit->appendPlainText(getCurrentDeviceAddress());
                    ui->plainTextEdit->appendPlainText(answer);
                }

            });


}


MainWindow::~MainWindow()
{
    delete cache;
    delete browser;

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
//    qDebug() << "service.hostname() " << service.hostname();
    QMdnsEngine::Resolver *resolver = new QMdnsEngine::Resolver(&server, service.hostname(), cache);
    QObject::connect(resolver, &QMdnsEngine::Resolver::resolved,
                     [=](const QHostAddress &address) {
                         qDebug() << "resolved to" << address;
        servicesData.insertDevice(service.name(), address);
                         if (!servicesData.getUName().contains(service.name())){
                             QString name = QString("Device " + QString::number(servicesData.getUName().size() + 1));
                             servicesData.insertUName(service.name(), name);
                             currentDeviceComboUpdate();
//                             saveServiceInfoInFile();

                         }

                         if (jsonDocDeviceParameters.isEmpty()){
                             requestParamsFromDevice();
                         }

                         resolver->deleteLater();
                     });

}

//void MainWindow::serviceResolver(const QString &serviceName)
//{

//    QMdnsEngine::Server server2;
//    QMdnsEngine::Resolver resolver2(&server2, serviceName.toUtf8());

//    QString host = "local.";
//    QMdnsEngine::Service service2;
//    service2.setName(serviceName.toUtf8());
//    service2.setHostname(".local.");
//    service2.setType("_http._tcp");

//    QObject::connect(&resolver2, &QMdnsEngine::Resolver::resolved,
//                     [=](const QMdnsEngine::Service &serviceD) {
//            qDebug() << "IP Address:" << serviceD.attributes();
//    });

//    resolver2.resolved(servicesData.getServices().value(serviceName));

//    QByteArray hostname = (serviceName + ".local.").toLocal8Bit();
//    QMdnsEngine::Resolver *resolver = new QMdnsEngine::Resolver(&server, hostname, cache);
//    QObject::connect(resolver, &QMdnsEngine::Resolver::resolved,
//                     [=](const QHostAddress &address) {
//                         qDebug() << "resolved to" << address;
//                         servicesData.insertDevice(serviceName, address);
//                         if (!servicesData.getUName().contains(serviceName)){
//                             QString name = QString("Device " + QString::number(servicesData.getUName().size() + 1));
//                             servicesData.insertUName(serviceName, name);
//                             currentDeviceComboUpdate();
//                             //                             saveServiceInfoInFile();

//                         }

//                         if (jsonDocDeviceParameters.isEmpty()){
//                             requestParamsFromDevice();
//                         }

//                         resolver->deleteLater();
//                     });
//}

void MainWindow::requestParamsFromDevice()
{
    QString addres = getCurrentDeviceAddress();

    if (addres != ""){
        // Get
        QNetworkRequest request;
        request.setUrl(QUrl::fromUserInput(addres));

        manager->get(request);
    }
}

void MainWindow::updateParamsOnDevice(QUrlQuery query)
{  
    QString curAddress = getCurrentDeviceAddress();

    if (curAddress != ""){
        QNetworkRequest request;

        QUrl url = QUrl::fromUserInput(curAddress + "/api");
//        QUrlQuery query;
//        query.addQueryItem("state", state);
//        query.addQueryItem("hsv", getHexHSVColor());

        url.setQuery(query.query());

        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        manager->post(request, QByteArray());
    }

}

QString MainWindow::getCurrentDeviceAddress()
{

    if (ui->selectedDeviceComboBox->currentIndex() == -1)
    {
        qDebug() << "Error missing services";
        return "";
    }

    QString curUName = ui->selectedDeviceComboBox->currentText();

    if (servicesData.getUName().contains(curUName))
    {
        qDebug() << "Error non-existent device" << curUName;
        return "";
    }

    // TO DO !!! если существует Uname и сервер доступен (были данные на счет его API) то отдаем адрес

    QString curService = servicesData.getRevUName().value(curUName);

    if (servicesData.getServices().contains(curService)){

        QHostAddress address = servicesData.getServices().value(curService);
        qDebug() << "address" << address;

        return address.toString();

    } else {
        qDebug() << "Error missing address";
        serviceSerachRestart();

        return "";
    }

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
//    ui->colorPushButton->setAutoFillBackground(true);
//    ui->colorPushButton->setPalette(Pal);

    QString qss = QString("QPushButton {background-color: %1;"
                          "border-radius: 10px;"
                          "}").arg(color.name());
    ui->colorPushButton->setStyleSheet(qss);

}


void MainWindow::onOnColorChanged(const QColor &color)
{
    int h, s, v;
    color.getHsv(&h, &s, &v);
    hsv[0] = int((h / 360.0 * 255.0));
    hsv[1] = s;
    hsv[2] = v;


    updateColorItemUi();

    QUrlQuery query;
    query.addQueryItem("state", state);
    query.addQueryItem("hsv", getHexHSVColor());

    updateParamsOnDevice(query);
    // todo add set spinboxes or sliders
}

void MainWindow::checkState()
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

void MainWindow::checkMode()
{
    QString mode = jsonDeviceParameters.value("program-type").toString();
    mode = mode[0].toUpper() + mode.mid(1).toLower();;
    ui->modeComboBox->setCurrentText(mode);
    ui->modeComboBox->activated(ui->modeComboBox->currentIndex());
    qDebug() << "mode " << mode;
}

void MainWindow::currentDeviceComboUpdate()
{
    ui->selectedDeviceComboBox->clear();
    for (const QString &userDeviceName : servicesData.getRevUName().keys()) {
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

    QString qss = QString("QPushButton {background-color: %1;"
                          "border-radius: 10px;"
                          "}").arg(color.name());
    ui->colorPushButton->setStyleSheet(qss);


    ui->horizontalSliderHue->setSliderPosition(h);
    ui->horizontalSliderSaturation->setSliderPosition(s / 255.0 * 100.0);
    ui->horizontalSliderValue->setSliderPosition(v / 255.0 * 100.0);

//    ui->spinBoxHue->setValue(h);
//    ui->spinBoxSaturation->setValue(s / 255.0 * 100.0);
//    ui->spinBoxValue->setValue(v / 255.0 * 100.0);


}

void MainWindow::deactivateColorControls()
{
    ui->horizontalSliderHue->setEnabled(false);
    ui->horizontalSliderSaturation->setEnabled(false);
    ui->horizontalSliderValue->setEnabled(false);

    ui->spinBoxHue->setEnabled(false);
    ui->spinBoxSaturation->setEnabled(false);
    ui->spinBoxValue->setEnabled(false);

//    ui->modeComboBox->setEnabled(false);
    ui->colorPushButton->setEnabled(false);
    ui->gradientNameComboBox->setEnabled(false);

    ui->horizontalSliderSpeed->setEnabled(false);
    ui->spinBoxSpeed->setEnabled(false);

    ui->horizontalSliderScale->setEnabled(false);
    ui->spinBoxScale->setEnabled(false);
}

void MainWindow::setEnabledSolidMode()
{
    ui->horizontalSliderHue->setEnabled(true);
    ui->horizontalSliderSaturation->setEnabled(true);
    ui->horizontalSliderValue->setEnabled(true);

    ui->spinBoxHue->setEnabled(true);
    ui->spinBoxSaturation->setEnabled(true);
    ui->spinBoxValue->setEnabled(true);

    //    ui->modeComboBox->setEnabled(false);
    ui->colorPushButton->setEnabled(true);


}

void MainWindow::setEnabledGradientMode()
{
//    ui->modeComboBox->setEnabled(true);
    ui->horizontalSliderValue->setEnabled(true);
    ui->spinBoxValue->setEnabled(true);

    ui->gradientNameComboBox->setEnabled(true);

    ui->horizontalSliderSpeed->setEnabled(true);
    ui->spinBoxSpeed->setEnabled(true);

    ui->horizontalSliderScale->setEnabled(true);
    ui->spinBoxScale->setEnabled(true);
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
    qDebug() << "device count: " << servicesData.getServices().size();

    if (ui->selectedDeviceComboBox->count() == servicesData.getServices().size()){
        requestParamsFromDevice();
    }
}

//void MainWindow::on_selectedDeviceComboBox_textActivated(const QString &arg1)
//{
//    qDebug() << "textActivated: " << arg1;
//    qDebug() << "combo count: " << ui->selectedDeviceComboBox->count();
//    qDebug() << "device count: " << servicesData.getDevices().size();

//    if (ui->selectedDeviceComboBox->count() == servicesData.getDevices().size()){
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
    servicesFile.saveServicesData();
//    saveServiceInfoInFile();
    qDebug() << "write Service Info";
}


void MainWindow::on_jsonReadButton_clicked()
{
    servicesFile.readServicesData();
    qDebug() << "read Service Info \n" << servicesData.getServices();
    qDebug() << "read Service Info \n" << servicesData.getUName();
    currentDeviceComboUpdate();
    requestParamsFromDevice();
}

void MainWindow::on_fileClearButton_clicked()
{
//    servicesFile.clearServicesData();
//    servicesData.getServices().clear();
//    servicesData.getUName().clear();
//    servicesData.getRevUName().clear();
//    currentDeviceComboUpdate();
}


//void MainWindow::on_horizontalSliderHue_valueChanged(int value)
//{
//    hsv[0] = int((value / 360.0 * 255.0));
//    updateColorItemUi();

//    QUrlQuery query;
//    query.addQueryItem("hsv", getHexHSVColor());
////    updateParamsOnDevice(query);
//}

//void MainWindow::on_horizontalSliderSaturation_valueChanged(int value)
//{
//    hsv[1] = int((value / 100.0 * 255.0));
//    updateColorItemUi();

//    QUrlQuery query;
//    query.addQueryItem("hsv", getHexHSVColor());
////    updateParamsOnDevice(query);
//}


void MainWindow::on_horizontalSliderValue_valueChanged(int value)
{
    hsv[2] = int((value / 100.0 * 255.0));
//    updateColorItemUi();

    QUrlQuery query;
    query.addQueryItem("hsv", getHexHSVColor());
    updateParamsOnDevice(query);
}


void MainWindow::on_onButton_clicked(bool checked)
{
    if (checked){
        state = "run";
    } else {
        state = "off";
    }

    QUrlQuery query;
    query.addQueryItem("state", state);
    updateParamsOnDevice(query);

}


void MainWindow::on_modeComboBox_activated(int index)
{
    qDebug() << "mode combo index " << index;
    QUrlQuery query;

    switch (index) {
    case 0: // Solid
        deactivateColorControls();
        setEnabledSolidMode();

        query.addQueryItem("program-type", "solid");
        updateParamsOnDevice(query);

        break;

    case 1: // Blink

        query.addQueryItem("program-type", "blink");
        updateParamsOnDevice(query);

        break;

    case 2: // Gradient
    {
        deactivateColorControls();
        setEnabledGradientMode();

        query.addQueryItem("program-type", "gradient");

        QString curGradient = ui->gradientNameComboBox->currentText();
            if (gradients.contains(curGradient)){
                int curGradientInd = gradients.indexOf(curGradient);
                query.addQueryItem("gradient-number", QString::number(curGradientInd));
                qDebug() << QString::number(curGradientInd);
            }

        updateParamsOnDevice(query);

        break;
    }
    case 3: // Sunrise


        break;

    default:
        deactivateColorControls();
        break;
    }
}


void MainWindow::on_gradientNameComboBox_activated(int index)
{
    QUrlQuery query;
    query.addQueryItem("gradient-number", QString::number(index));
    updateParamsOnDevice(query);
}


void MainWindow::on_horizontalSliderScale_valueChanged(int value)
{
    qDebug() << "gradient-scale " << value;
    QUrlQuery query;
    query.addQueryItem("gradient-scale", QString::number(value));
    updateParamsOnDevice(query);

}


void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    qDebug() << "speed " << value;
    QUrlQuery query;
    query.addQueryItem("speed", QString::number(value));
    updateParamsOnDevice(query);
}





#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serviceSerachRestart();
    currentDeviceComboUpdate();

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
                    ui->plainTextEdit->clear();
                }
                //              qDebug() << "Answer: " << answer;

                ui->plainTextEdit->appendPlainText(getCurrentDeviceAddress());
                ui->plainTextEdit->appendPlainText(answer);
            });

    //    postManager = new QNetworkAccessManager(this);

    //    connect(postManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
    //        if (reply->error() == QNetworkReply::NoError) {
    //            QByteArray responseData = reply->readAll();
    //            // Обработка ответа сервера
    //        } else {
    //            qDebug() << "Error update data on device";
    //        }
    //        reply->deleteLater();
    //    });

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
                         qDebug() << service.name().contains("CLED") << "discovered!";
                         ui->plainTextEdit->appendPlainText(service.name());
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
                         }

                         currentDeviceComboUpdate();

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
    query.addQueryItem("state", "run");
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

    QColor color = QColor();
    int h = hsv[0].toInt() / 255.0 * 360.0;
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();
    qDebug() << h << s << v;
    color.setHsv(h, s, v);
    qDebug() << color;

    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, color.rgb());
    ui->colorPushButton->setAutoFillBackground(true);
    ui->colorPushButton->setPalette(Pal);


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

void MainWindow::onOnColorChanged(const QColor &color)
{
    int h, s, v;
    color.getHsv(&h, &s, &v);
    hsv[0] = int((h / 360.0 * 255.0));
    hsv[1] = s;
    hsv[2] = v;
    qDebug() << "h: " << h << "  s: " << s << "  v: " << v;


    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, color.rgb());
    ui->colorPushButton->setAutoFillBackground(true);
    ui->colorPushButton->setPalette(Pal);

    // todo add set spinboxes or sliders



    updateParamsOnDevice();
}

void MainWindow::currentDeviceComboUpdate()
{
    ui->selectedDeviceComboBox->clear();
    foreach (QString deviceName, nameMap) {
        ui->selectedDeviceComboBox->addItem(deviceName);
    }
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


    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, color.rgb());
    ui->colorPushButton->setAutoFillBackground(true);
    ui->colorPushButton->setPalette(Pal);

    colorDialog->setCurrentColor(color);
    colorDialog->show();
}


void MainWindow::on_selectedDeviceComboBox_currentTextChanged(const QString &arg1)
{
    qDebug() << "currentTextChanged: " << arg1;
    requestParamsFromDevice();


}


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


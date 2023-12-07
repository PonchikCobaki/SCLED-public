#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gradientPaletts.cpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->gradientNameComboBox->addItems(gradients);
    setEnabledColorControls(false);


    servicesFile.setBuffer(&servicesData);
    servicesFile.readServicesData();

    currentDeviceComboUpdate();

    serviceSerachRestart();

    setEnabledSolidMode();

    // avaibale check timer
    chekAvaibelTimer = new QTimer(this);
    connect(chekAvaibelTimer, &QTimer::timeout, [=](){
        requestParamsFromDevice();
    });
    chekAvaibelTimer->start(5000);

    manager = new QNetworkAccessManager(this);
    connect(manager,
            &QNetworkAccessManager::finished,
            [=] (QNetworkReply *reply) {
                if (reply->error()) {
                    qDebug() << "Error: " << reply->errorString();            
                    connectionAbsent();
                    return;
                }
                QString answer = reply->readAll();
                if (!answer.isEmpty()){
                    ui->modeComboBox->activated(ui->modeComboBox->currentIndex());
                    jsonParse(answer);
                    checkState();
                    updateColorItemUi();
                    checkProgramMode();
                    checkGradientNum();
                    checkBlend();
                    checkScale();
                    checkSpeed();
                    checkSunrise();
                    ui->plainTextEdit->clear();
//                    ui->plainTextEdit->appendPlainText(getCurrentDeviceAddress());
                    ui->plainTextEdit->appendPlainText(answer);
                }


            });


}


MainWindow::~MainWindow()
{
//    delete cache;
//    delete browser;

    delete ui;
}


/*------------------------------PRIVATE SLOTS-----------------------------------*/

void MainWindow::onTimerTimeout()
{

}

void MainWindow::on_onButton_clicked(bool checked)
{
    QString state;
    if (checked){
        state = "run";
    } else {
        state = "off";
    }

    QUrlQuery query;
    query.addQueryItem("state", state);
    updateParamsOnDevice(query);

}

void MainWindow::on_renameService_clicked()
{

    //    connect(renameWinwdow, &QDialog::currentColorChanged, [=](const QColor &color){
    //        onOnColorChanged(color);
    //    });

    renameDialog *rw = new renameDialog(this);


    rw->setDataPtr(&servicesData);
    rw->setCurrentService(getCurrentService());
    rw->show();
    rw->exec();

    currentDeviceComboUpdate();
    servicesFile.saveServicesData();

}

void MainWindow::on_deleteService_clicked()
{
    QString curName = ui->selectedDeviceComboBox->currentText();
    servicesData.deliteService(servicesData.getRevUName().value(curName));
    servicesFile.saveServicesData();
    currentDeviceComboUpdate();
}

void MainWindow::on_deviceSearch_clicked()
{
    qDebug("mDNS search restart");
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
    // to do
    servicesFile.clearServicesData();
    //    servicesData.getServices().clear();
    //    servicesData.getUName().clear();
    //    servicesData.getRevUName().clear();
    //    currentDeviceComboUpdate();
}

void MainWindow::on_selectedDeviceComboBox_textActivated(const QString &arg1)
{
    qDebug() << "current dev activated: " << arg1;
    qDebug() << "combo count: " << ui->selectedDeviceComboBox->count();
    qDebug() << "device count: " << servicesData.getServices().size();

    if (servicesData.getRevUName().contains(arg1)){
        ui->selectedDeviceComboBox->setCurrentText(arg1);
        requestParamsFromDevice();
    }
}

void MainWindow::on_horizontalSliderHue_valueChanged(int value)
{
    hsv[0] = std::round(value / 360.0 * 255.0);
    QUrlQuery query;
    query.addQueryItem("hsv", getHexHSVColor());
    updateParamsOnDevice(query);
    updateColorPickerButton();
}

void MainWindow::on_horizontalSliderSaturation_valueChanged(int value)
{
    hsv[1] = int(value * 2.55);
    QUrlQuery query;
    query.addQueryItem("hsv", getHexHSVColor());
    updateParamsOnDevice(query);
    updateColorPickerButton();
}


void MainWindow::on_horizontalSliderValue_valueChanged(int value)
{
    hsv[2] = int(value * 2.55);
    QUrlQuery query;
    query.addQueryItem("hsv", getHexHSVColor());
    updateParamsOnDevice(query);
    updateColorPickerButton();
}

void MainWindow::on_colorPushButton_clicked()
{

    if (colorDialog != nullptr){
        delete colorDialog;
        colorDialog = nullptr;
    }


    colorDialog = new QColorDialog(this);
    //    connect(colorDialog, &QColorDialog::colorSelected, [=](const QColor &color){
    //        onOnColorChanged(color);
    //    });
    connect(colorDialog, &QColorDialog::currentColorChanged, [=](const QColor &color){
        onOnColorChanged(color);
    });



    QColor color = QColor();

    int h = std::round(hsv[0].toInt() / 255.0 * 360.0);
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();
    //    qDebug() << h << s << v;

    color.setHsv(h, s, v);

    colorDialog->setCurrentColor(color);
    colorDialog->show();
}

void MainWindow::onOnColorChanged(const QColor &color)
{
    int h, s, v;
    color.getHsv(&h, &s, &v);
    hsv[0] = std::round(h / 360.0 * 255.0);
    hsv[1] = s;
    hsv[2] = v;

    QUrlQuery query;
    query.addQueryItem("hsv", getHexHSVColor());
    updateParamsOnDevice(query);

    updateColorItemUi();
}

void MainWindow::on_modeComboBox_activated(int index)
{
    qDebug() << "mode combo index " << index;
    QUrlQuery query;

    switch (index) {
    case 0: // Solid
        setEnabledColorControls(false);
        setEnabledSolidMode();

        query.addQueryItem("program-type", "solid");
        updateParamsOnDevice(query);

        break;

    case 1: // Blink
        setEnabledColorControls(false);
        setEnabledBlinkMode();

        query.addQueryItem("program-type", "blink");
        updateParamsOnDevice(query);

        break;

    case 2: // Gradient
    {
        setEnabledColorControls(false);
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
        setEnabledColorControls(false);
        setEnabledSunriseMode();

        query.addQueryItem("program-type", "sunrise");
        updateParamsOnDevice(query);

        break;

    default:
        setEnabledColorControls(false);
        break;
    }
}

void MainWindow::on_gradientNameComboBox_activated(int index)
{
    QUrlQuery query;
    query.addQueryItem("gradient-number", QString::number(index));
    updateParamsOnDevice(query);
}

void MainWindow::on_blendComboBox_activated(int index)
{

    QUrlQuery query;

    if (index == 0){
        query.addQueryItem("blend-type", "linearblend");
    }
    else if (index == 1){
        query.addQueryItem("blend-type", "linearblend-nowarp");
    }
    else {
        query.addQueryItem("blend-type", "noblend");
    }

    updateParamsOnDevice(query);
}

void MainWindow::on_horizontalSliderScale_valueChanged(int value)
{
    qDebug() << "scale " << value;
    QUrlQuery query;
    query.addQueryItem("scale", QString::number(std::round(value / 100. * 255)));
    //    query.addQueryItem("scale", QString::number(value));
    updateParamsOnDevice(query);

}

void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    qDebug() << "speed " << value;
    QUrlQuery query;
    query.addQueryItem("speed", QString::number(std::round(value / 100. * 255)));
    //    query.addQueryItem("speed", QString::number(value));
    updateParamsOnDevice(query);
}

void MainWindow::on_sunriseStartButton_clicked()
{
    setEnabledSunriseControls(ui->sunriseTimeEdit->isEnabled());

    QTime timeDelay = ui->sunriseTimeEdit->time();
    qDebug() << "timeDelay: " << timeDelay;

    QUrlQuery query;
    query.addQueryItem("sunrise-delay", QString::number(timeDelay.minute() + timeDelay.hour() * 60));
    query.addQueryItem("sunrise-state", QString::number(1));
    updateParamsOnDevice(query);
}



/*------------------------------PRIVATE FUNC-----------------------------------*/



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
                        // update ip in data map
                        servicesData.insertService(service.name(), address);

                        // add new servse in data map
                        if (!servicesData.getUName().contains(service.name())){
                            QString name = QString("Device " + QString::number(servicesData.getUName().size() + 1));
                            servicesData.insertUName(service.name(), name);
                            currentDeviceComboUpdate();
                            servicesFile.saveServicesData();
                         }

                        // first start
                        if (jsonDocDeviceParameters.isEmpty()){
                            ui->selectedDeviceComboBox->textActivated(servicesData.getUName().value(service.name()));
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

void MainWindow::jsonParse(QString jsonString)
{
    jsonDocDeviceParameters = QJsonDocument::fromJson(jsonString.toUtf8());
    jsonDeviceParameters = jsonDocDeviceParameters.object();

    hsv = jsonDeviceParameters.value("hsv").toArray();


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

//void MainWindow::resolveJsonParse()
//{
//    jsonDeviceParameters = jsonDocDeviceParameters.object();
//    hsv = jsonDeviceParameters.value("hsv").toArray();

//    QColor color = QColor();
//    int h = std::round(hsv[0].toInt() / 255.0 * 360.0);
//    int s = hsv[1].toInt();
//    int v = hsv[2].toInt();

//    color.setHsv(h, s, v);
//    qDebug() << color;

//    QPalette Pal(palette());
//    Pal.setColor(QPalette::Button, color.rgb());
//    //    ui->colorPushButton->setAutoFillBackground(true);
//    //    ui->colorPushButton->setPalette(Pal);

//    QString qss = QString("QPushButton {background-color: %1;"
//                          "border-radius: 10px;"
//                          "}").arg(color.name());
//    ui->colorPushButton->setStyleSheet(qss);

//}

void MainWindow::requestParamsFromDevice()
{
    if (!timerRequest.isValid() || timerRequest.hasExpired(timeoutRequest)){
        QString addres = getCurrentDeviceAddress();

        if (addres != ""){
            // Get
            qDebug() << "Get request";
            QNetworkRequest request;
            request.setUrl(QUrl::fromUserInput(addres));

            manager->get(request);
        }
        timerRequest.start();
    }
}

void MainWindow::requestParamsFromDevice(const QHostAddress &address)
{
    if (!timerRequest.isValid() || timerRequest.hasExpired(timeoutRequest)){
        QString addres = address.toString();

        if (addres != ""){
            // Get
            QNetworkRequest request;
            request.setUrl(QUrl::fromUserInput(addres));

            manager->get(request);
        }
        timerRequest.start();
    }
}

void MainWindow::updateParamsOnDevice(QUrlQuery query)
{

    if (!timerUpdate.isValid() || timerUpdate.hasExpired(timeoutRequest)){
        QString curAddress = getCurrentDeviceAddress();

        if (curAddress != ""){
            QNetworkRequest request;

            QUrl url = QUrl::fromUserInput(curAddress + "/api");
    //        QUrlQuery query;
    //        query.addQueryItem("state", state);
    //        query.addQueryItem("hsv", getHexHSVColor());

            qDebug() << "Post request: " << query.query();
            url.setQuery(query.query());

            request.setUrl(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");            
            manager->post(request, QByteArray());

        }
        timerUpdate.start();
    }
}

void MainWindow::connectionAbsent()
{
    setEnabledColorControls(false);
    jsonParse("state=0");
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("ERROR: there is no connection to the device");
}

QString MainWindow::getCurrentDeviceAddress()
{

    QString curService = getCurrentService();

    if (servicesData.getServices().contains(curService)){

        QHostAddress address = servicesData.getServices().value(curService);
        qDebug() << "address" << address;

        return address.toString();

    }

    // missing address
    qDebug() << "Error missing address";
    serviceSerachRestart();
    connectionAbsent();

    return "";

}

QString MainWindow::getCurrentService()
{
    //  if ui combo with services list is empty
    if (ui->selectedDeviceComboBox->currentIndex() == -1)
    {
        qDebug() << "Error missing services";
        return "";
    }

    QString curUName = ui->selectedDeviceComboBox->currentText();

    //  if the newly entered in combo box user name is missing
    if (servicesData.getUName().contains(curUName))
    {
        qDebug() << "Error non-existent device" << curUName;
        return "";
    }

    // TO DO !!! если существует Uname и сервер доступен (были данные на счет его API) то отдаем адрес

    return servicesData.getRevUName().value(curUName);
}

QString MainWindow::getHexHSVColor()
{
    int h = hsv[0].toInt();
    int s = hsv[1].toInt();
    int v = hsv[2].toInt();
    QString hexColor = QString("%1%2%3").arg(h , 2, 16).arg(s, 2, 16).arg(v, 2, 16);
    if (hexColor.contains(" "))
        hexColor.replace(" ", "0");
//    qDebug() << hexColor;
    return hexColor;
}


/*------------------------------GUI-----------------------------------*/



void MainWindow::currentDeviceComboUpdate()
{
    ui->selectedDeviceComboBox->clear();
    for (const QString &userDeviceName : servicesData.getRevUName().keys()) {
        ui->selectedDeviceComboBox->addItem(userDeviceName);
    }
}

void MainWindow::updateColorItemUi()
{
    checkHSVSliders();
    updateColorPickerButton();
}

void MainWindow::updateColorPickerButton()
{
    int h = std::round(hsv[0].toInt() / 255.0 * 360.0);
    int s = hsv[1].toInt();
    int v = std::round(hsv[2].toInt() / 255.0 * 128) + 127;

    QColor color = QColor();
    color.setHsv(h, s, v);

    QString qss = QString("QPushButton {background-color: %1;"
                          "border-radius: 10px;"
                          "}").arg(color.name());
    ui->colorPushButton->setStyleSheet(qss);
}

void MainWindow::checkHSVSliders()
{
    int h = std::round(hsv[0].toInt() / 255.0 * 360.0);
    int s = std::round(hsv[1].toInt() / 255.0 * 100.0);
    int v = std::round(hsv[2].toInt() / 255.0 * 100.0);

    ui->spinBoxHue->setValue(h);
    ui->spinBoxSaturation->setValue(s);
    ui->spinBoxValue->setValue(v);
}

void MainWindow::checkState()
{
    QString s = jsonDeviceParameters.value("state").toString();
//    state = s;
    if (s == "run"){
        ui->onButton->setChecked(true);
    }
    else if (s == "off"){
        ui->onButton->setChecked(false);
    }
}

void MainWindow::checkProgramMode()
{
    QString mode = jsonDeviceParameters.value("program-type").toString();
    mode = mode[0].toUpper() + mode.mid(1).toLower();
    if (mode != ui->modeComboBox->currentText()){
        ui->modeComboBox->setCurrentText(mode);
        ui->modeComboBox->activated(ui->modeComboBox->currentIndex());  // update enabled elements
    }

    qDebug() << "mode " << mode;
}

void MainWindow::checkGradientNum()
{
    int gradientNum = jsonDeviceParameters.value("gradient-number").toInt();
    if (gradientNum != ui->gradientNameComboBox->currentIndex()){
        ui->gradientNameComboBox->setCurrentIndex(gradientNum);
        qDebug() << "GradientNum " << gradientNum;
    }
}

void MainWindow::checkBlend()
{
    QString blend = jsonDeviceParameters.value("blend-type").toString();
    if (blend == "linearblend"){
        ui->blendComboBox->setCurrentIndex(0);
    }
    else if (blend == "linearblend-nowarp"){
        ui->blendComboBox->setCurrentIndex(1);
    }
    else if (blend == "noblend"){
        ui->blendComboBox->setCurrentIndex(2);
    }
}

void MainWindow::checkSunrise()
{
    int dalayToStartMin = jsonDeviceParameters.value("sunrise-delay").toInt();
    int timePassedMin = 0;
    bool sunriseState = jsonDeviceParameters.value("sunrise-state").toInt();
    if (sunriseState)
        timePassedMin = jsonDeviceParameters.value("time-passed").toInt();

    setEnabledSunriseControls(sunriseState);

    QTime dalayToStart(dalayToStartMin / 60, dalayToStartMin % 60);
    QTime timePassed(timePassedMin / 60, timePassedMin % 60);

    ui->sunriseTimeEdit->setTime(dalayToStart);
    ui->sunriseElapsetTimeEdit->setTime(timePassed);


}

void MainWindow::checkScale()
{
    int scale = jsonDeviceParameters.value("scale").toInt();
    ui->spinBoxScale->setValue(std::round(scale / 255.0 * 100.0));
    qDebug() << "scale " << scale;
}

void MainWindow::checkSpeed()
{
    int speed = jsonDeviceParameters.value("speed").toInt();
    ui->spinBoxSpeed->setValue(std::round(speed / 255.0 * 100.0));
    qDebug() << "speed " << speed;
}


void MainWindow::setEnabledColorControls(bool state)
{
    ui->horizontalSliderHue->setEnabled(state);
    ui->horizontalSliderSaturation->setEnabled(state);
    ui->horizontalSliderValue->setEnabled(state);

    ui->spinBoxHue->setEnabled(state);
    ui->spinBoxSaturation->setEnabled(state);
    ui->spinBoxValue->setEnabled(state);

//    ui->modeComboBox->setEnabled(state);
    ui->colorPushButton->setEnabled(state);
    ui->gradientNameComboBox->setEnabled(state);
    ui->blendComboBox->setEnabled(state);

    ui->horizontalSliderSpeed->setEnabled(state);
    ui->spinBoxSpeed->setEnabled(state);

    ui->horizontalSliderScale->setEnabled(state);
    ui->spinBoxScale->setEnabled(state);

    ui->sunriseTimeEdit->setEnabled(state);
    ui->sunriseStartButton->setEnabled(state);
    ui->sunriseElapsetTimeEdit->setEnabled(state);
}

void MainWindow::setEnabledSolidMode()
{
    //    ui->modeComboBox->setEnabled(true);

    ui->horizontalSliderHue->setEnabled(true);
    ui->horizontalSliderSaturation->setEnabled(true);
    ui->horizontalSliderValue->setEnabled(true);

    ui->spinBoxHue->setEnabled(true);
    ui->spinBoxSaturation->setEnabled(true);
    ui->spinBoxValue->setEnabled(true);


    ui->colorPushButton->setEnabled(true);


}

void MainWindow::setEnabledBlinkMode()
{
    //    ui->modeComboBox->setEnabled(true);

    ui->horizontalSliderHue->setEnabled(true);
    ui->horizontalSliderSaturation->setEnabled(true);
    ui->horizontalSliderValue->setEnabled(true);

    ui->spinBoxHue->setEnabled(true);
    ui->spinBoxSaturation->setEnabled(true);
    ui->spinBoxValue->setEnabled(true);

    ui->colorPushButton->setEnabled(true);

    ui->horizontalSliderSpeed->setEnabled(true);
    ui->spinBoxSpeed->setEnabled(true);

    ui->horizontalSliderScale->setEnabled(true);
    ui->spinBoxScale->setEnabled(true);
}

void MainWindow::setEnabledGradientMode()
{
//    ui->modeComboBox->setEnabled(true);
    ui->horizontalSliderValue->setEnabled(true);
    ui->spinBoxValue->setEnabled(true);

    ui->gradientNameComboBox->setEnabled(true);
    ui->blendComboBox->setEnabled(true);

    ui->horizontalSliderSpeed->setEnabled(true);
    ui->spinBoxSpeed->setEnabled(true);

    ui->horizontalSliderScale->setEnabled(true);
    ui->spinBoxScale->setEnabled(true);
}

void MainWindow::setEnabledSunriseMode()
{
    ui->horizontalSliderValue->setEnabled(true);
    ui->spinBoxValue->setEnabled(true);

    ui->sunriseTimeEdit->setEnabled(true);
    ui->sunriseStartButton->setEnabled(true);
    ui->sunriseElapsetTimeEdit->setEnabled(true);
}

void MainWindow::setEnabledSunriseControls(bool state)
{
    if (state){
        ui->sunriseTimeEdit->setEnabled(false);
        ui->sunriseStartButton->setText("Stop");
    } else {
        ui->sunriseTimeEdit->setEnabled(true);
        ui->sunriseStartButton->setText("Start");
    }
}


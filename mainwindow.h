#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QColorDialog>
#include <QColor>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include <mdns.h>
#include <service.h>
#include <server.h>
#include <cache.h>
#include <browser.h>
#include <resolver.h>

#include "serviceParemeters.h"
#include "deviceDataFiles.h"


Q_DECLARE_METATYPE(QMdnsEngine::Service)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QMdnsEngine::Server   server;
    QMdnsEngine::Cache   *cache = nullptr;
    QMdnsEngine::Browser *browser = nullptr;

    ServiceParemeters servicesData;
    DeviceDataFile servicesFile;

//    QMap<QString, QHostAddress> devicesMap; // Service name, device IP
//    QMap<QString, QString> nameMap; // Service name, User device name
//    QMap<QString, QString> reverseNameMap; // User device name,  Service name

    QNetworkAccessManager *manager = nullptr;
    QNetworkAccessManager *postManager = nullptr;
    QColorDialog          *colorDialog = nullptr;

private slots:
    void on_colorPushButton_clicked();

    void on_selectedDeviceComboBox_currentTextChanged(const QString &arg1);

    void on_deviceSearch_clicked();

    void on_updateParameters_clicked();

    void on_jsonSaveButton_clicked();

    void on_jsonReadButton_clicked();

    void on_fileClearButton_clicked();

//    void on_horizontalSliderHue_valueChanged(int value);
//    void on_horizontalSliderSaturation_valueChanged(int value);
    void on_horizontalSliderValue_valueChanged(int value);

    void on_onButton_clicked(bool checked);

    void on_modeComboBox_activated(int index);

    void on_gradientNameComboBox_activated(int index);

    void on_horizontalSliderScale_valueChanged(int value);

    void on_horizontalSliderSpeed_valueChanged(int value);



private:

    Ui::MainWindow *ui;
    QJsonDocument jsonDocDeviceParameters;
    QJsonObject jsonDeviceParameters;
    QJsonArray hsv;
    QString state;

    void serviceSerachRestart();
    void serviceResolver(const QMdnsEngine::Service &service);
//    void serviceResolver(const QString &serviceName);

    void jsonParse(QString jsonString);
    void resolveJsonParse();

    void requestParamsFromDevice();
    void updateParamsOnDevice(QUrlQuery query);


    QString getCurrentDeviceAddress();
    QString getHexHSVColor();


    void onOnColorChanged(const QColor &color);


    void currentDeviceComboUpdate();
    void updateColorItemUi();

    void checkState();
    void checkMode();

    void deactivateColorControls();
    void setEnabledSolidMode();
    void setEnabledGradientMode();

};
#endif // MAINWINDOW_H

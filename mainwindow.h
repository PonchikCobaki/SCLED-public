#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QColorDialog>
#include <QColor>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>

#include <QMap>
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

#include <cmath>

#include <QTimer>
#include <QElapsedTimer>


#include "renameDialog.h"

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

    int timeoutRequest = 100;
    QElapsedTimer timerUpdate;
    QElapsedTimer timerRequest;

    QTimer *chekAvaibelTimer;


//    QMap<QString, QHostAddress> devicesMap; // Service name, device IP
//    QMap<QString, QString> nameMap; // Service name, User device name
//    QMap<QString, QString> reverseNameMap; // User device name,  Service name

    QNetworkAccessManager *manager = nullptr;
    QNetworkAccessManager *postManager = nullptr;
    QColorDialog          *colorDialog = nullptr;
    QDialog               *renameWinwdow = nullptr;

private slots:
    void onTimerTimeout();

    void on_onButton_clicked(bool checked);
    void on_renameService_clicked();
    void on_deleteService_clicked();
    void on_deviceSearch_clicked();
    void on_updateParameters_clicked();

    void on_jsonSaveButton_clicked();
    void on_jsonReadButton_clicked();
    void on_fileClearButton_clicked();

    void on_selectedDeviceComboBox_textActivated(const QString &arg1);


    void on_horizontalSliderHue_valueChanged(int value);
    void on_horizontalSliderSaturation_valueChanged(int value);
    void on_horizontalSliderValue_valueChanged(int value);

    void on_colorPushButton_clicked();
    void onOnColorChanged(const QColor &color);

    void on_modeComboBox_activated(int index);

    void on_gradientNameComboBox_activated(int index);
    void on_blendComboBox_activated(int index);

    void on_horizontalSliderScale_valueChanged(int value);
    void on_horizontalSliderSpeed_valueChanged(int value);

    void on_sunriseStartButton_clicked();



private:
    Ui::MainWindow *ui;
    QJsonDocument jsonDocDeviceParameters;
    QJsonObject jsonDeviceParameters;
    QJsonArray hsv;
//    QString state;



    void serviceSerachRestart();
    void serviceResolver(const QMdnsEngine::Service &service);
//    void serviceResolver(const QString &serviceName);

    void jsonParse(QString jsonString);
//    void resolveJsonParse();

    void requestParamsFromDevice();
    void requestParamsFromDevice(const QHostAddress &address);
    void updateParamsOnDevice(QUrlQuery query);
    void connectionAbsent();

    QString getCurrentDeviceAddress();
    QString getCurrentService();
    QString getHexHSVColor();


    void currentDeviceComboUpdate();

    void updateColorItemUi();
    void updateColorPickerButton();
    void checkHSVSliders();

    void checkState();
    void checkProgramMode();

    void checkGradientNum();
    void checkBlend();
    void checkSunrise();

    void checkScale();
    void checkSpeed();


    void setEnabledColorControls(bool state);
    void setEnabledSolidMode();
    void setEnabledBlinkMode();
    void setEnabledGradientMode();
    void setEnabledSunriseMode();

    void setEnabledSunriseControls(bool state);




};
#endif // MAINWINDOW_H

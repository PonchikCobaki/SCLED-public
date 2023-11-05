#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QColorDialog>
#include <QColor>

#include <mdns.h>
#include <service.h>
#include <server.h>
#include <cache.h>
#include <browser.h>
#include <resolver.h>

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

    QMdnsEngine::Server server;
    QMdnsEngine::Cache *cache;
    QMdnsEngine::Browser *browser;

    QMap<QString, QHostAddress> devicesMap; // Service name, device IP
    QMap<QString, QString> nameMap; // Service name, User device name
    QMap<QString, QString> reverseNameMap; // User device name,  Service name

    QNetworkAccessManager *manager;
    QColorDialog          *colorDialog;
private slots:

    void on_mDNSupdate_pressed();

private:
    Ui::MainWindow *ui;

    void createBrowser();
    void resolve(const QMdnsEngine::Service &service);

};
#endif // MAINWINDOW_H

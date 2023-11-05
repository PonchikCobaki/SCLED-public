#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(parent);
    connect(manager,
            &QNetworkAccessManager::finished,
            [=] (QNetworkReply *reply) {
                if (reply->error()) {
                    qDebug() << "Error: " << reply->errorString();
                    ui->plainTextEdit->appendPlainText(reply->errorString());
                    return;
                }

                QString answer = reply->readAll();

                qDebug() << "Answer: " << answer;
                ui->plainTextEdit->appendPlainText(answer);
            });

    createBrowser();



    colorDialog = new QColorDialog(this);
    connect(colorDialog, &QColorDialog::colorSelected, [=](const QColor &color){
        int h, s, v;
        color.getHsv(&h, &s, &v);
        qDebug() << "h: " << h << "  s: " << s << "  v: " << v;
    });
    connect(colorDialog, &QColorDialog::currentColorChanged, [=](const QColor &color){
        int h, s, v;
        color.getHsv(&h, &s, &v);
        qDebug() << "h: " << h << "  s: " << s << "  v: " << v;
    });
    QColor color = QColor();
    color.setHsv(100, 255, 100);
    colorDialog->setCurrentColor(color);
    colorDialog->show();
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::createBrowser()
{
    cache = new QMdnsEngine::Cache();
    browser = new QMdnsEngine::Browser(&server, "_http._tcp.local.", cache);
    QObject::connect(browser,
                     &QMdnsEngine::Browser::serviceAdded,
                     [=](const QMdnsEngine::Service &service) {
                         qDebug() << "Добавлен сервис:" << service.name();
                             qDebug() << service.name().contains("CLED") << "discovered!";
                         ui->plainTextEdit->appendPlainText(service.name());
                         ui->plainTextEdit->appendPlainText(service.name());
                         //                        foreach (const QByteArray& key, service.attributes().keys()) {
                         //                            qDebug() << "param: " + key + ", Value: " + service.attributes().value(key) + "\n";
                         //                        }
                         //                        ui->plainTextEdit->appendPlainText(service.attributes()
                         //                        ui->plainTextEdit->appendPlainText(service.port());
                         resolve(service);
                     });


    QObject::connect(browser,
                     &QMdnsEngine::Browser::serviceUpdated,
                     [=](const QMdnsEngine::Service &service) {
                         qDebug() << "Service updated:" << service.name();
                         ui->plainTextEdit->appendPlainText(service.name() + " updated");
                         resolve(service);
                     });

    QObject::connect(browser,
                     &QMdnsEngine::Browser::serviceRemoved,
                     [=](const QMdnsEngine::Service &service) {
                         qDebug() << "Service removed:" << service.name();
                         ui->plainTextEdit->appendPlainText(service.name() + " removed");
                     });
}

void MainWindow::resolve(const QMdnsEngine::Service &service)
{
    QMdnsEngine::Resolver *resolver = new QMdnsEngine::Resolver(&server, service.hostname(), cache);
    QObject::connect(resolver, &QMdnsEngine::Resolver::resolved,
                     [=](const QHostAddress &address) {
                         qDebug() << "resolved to" << address;
                         devicesMap.insert(service.name(), address);
                         nameMap.insert(service.name(), "user " );
                         ui->plainTextEdit->appendPlainText(address.toString());

                         // Get
                         QNetworkRequest request;
                         request.setUrl(QUrl::fromUserInput(address.toString()));
                         manager->get(request);

                         resolver->deleteLater();
                     }
                     );

}



void MainWindow::on_mDNSupdate_pressed()
{
    qDebug("mDNS update");
    // Освобождение ресурсов


    delete cache;
    delete browser;
    createBrowser();
}



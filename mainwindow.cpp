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

void MainWindow::onOnColorChanged(const QColor &color)
{
    int h, s, v;
    color.getHsv(&h, &s, &v);
    qDebug() << "h: " << h << "  s: " << s << "  v: " << v;

    QString hexColor = QString("%1%2%3").arg((int)((h / 360.0) * 255), 2, 16).arg(s, 2, 16).arg(v, 2, 16);
    qDebug() << "hexColor: " << hexColor;

    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, color.rgb());
    ui->colorPushButton->setAutoFillBackground(true);
    ui->colorPushButton->setPalette(Pal);


    if (!devicesMap.isEmpty()){
        QHostAddress address = devicesMap.first();

        QNetworkRequest request;

        QUrl url = QUrl::fromUserInput(address.toString() + "/api");

        QUrlQuery query;
        query.addQueryItem("state", "run");
        query.addQueryItem("hsv", hexColor);
        url.setQuery(query.query());

        request.setUrl(url);
        manager->get(request);
        //manager->post(request);
    }
}



void MainWindow::on_mDNSupdate_pressed()
{
    qDebug("mDNS update");
    // Освобождение ресурсов


    delete cache;
    delete browser;
    createBrowser();
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
    color.setHsv(100, 255, 100);
    colorDialog->setCurrentColor(color);
    colorDialog->show();
}


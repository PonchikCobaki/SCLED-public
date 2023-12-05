#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>
#include <QString>

#include "serviceParemeters.h"


namespace Ui {
class renameDialog;
}

class renameDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::renameDialog *ui;

    ServiceParemeters *servicesData = nullptr;
    QString curService;
    QString oldName;

public:
    explicit renameDialog(QWidget *parent = nullptr);
    ~renameDialog();

    void setDataPtr(ServiceParemeters *servicesData);
    void setCurrentService(const QString curService);


    Q_SIGNALS:

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

public:


};

#endif // RENAMEDIALOG_H

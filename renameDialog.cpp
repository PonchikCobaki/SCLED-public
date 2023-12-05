#include "renameDialog.h"
#include "ui_renameDialog.h"

renameDialog::renameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renameDialog)
{
    ui->setupUi(this);
    this->setModal(true);


}

renameDialog::~renameDialog()
{
    delete ui;
}

void renameDialog::setDataPtr(ServiceParemeters *servicesData)
{
    this->servicesData = servicesData;
}

void renameDialog::setCurrentService(const QString curService)
{
    this->curService = curService;
    oldName = servicesData->getUName().value(curService);
    ui->lineEdit->setText(oldName);
}

void renameDialog::on_buttonBox_accepted()
{
    QString newName = ui->lineEdit->text();
    if (newName != ""){
        if (!servicesData->getRevUName().contains(newName))
            servicesData->changeUName(curService, oldName, newName);
    }

    this->close();
}


void renameDialog::on_buttonBox_rejected()
{
    this->close();
}


#include "renameDialog.h"
#include "ui_renameDialog.h"

renameDialog::renameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renameDialog)
{
    ui->setupUi(this);
}

renameDialog::~renameDialog()
{
    delete ui;
}

bool renameDialog::takingDecision()
{
    return true;
}

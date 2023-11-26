#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class renameDialog;
}

class renameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit renameDialog(QWidget *parent = nullptr);
    ~renameDialog();

private:
    Ui::renameDialog *ui;

Q_SIGNALS:
public:
    void takingDecision();
};

#endif // RENAMEDIALOG_H

#ifndef FIRSTSETTINGSDIALOG_H
#define FIRSTSETTINGSDIALOG_H

#include <QDialog>
#include "defs.h"

class QSettings;

namespace Ui {
class FirstSettingsDialog;
}

class FirstSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstSettingsDialog(QWidget *parent = 0);
    ~FirstSettingsDialog();

    void closeEvent(QCloseEvent *);
private slots:
    void on_choosePathButton_clicked();

    void on_acceptButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::FirstSettingsDialog *ui;
    QSettings *settings;
};

#endif // FIRSTSETTINGSDIALOG_H

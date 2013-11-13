#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QList>
#include <QSettings>

#include "defs.h"

struct Login{
    QString name;
    QString passwd;
};

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_quitButton_clicked();

    void on_loginButton_clicked();

private:
    Ui::LoginDialog *ui;

    QSettings *settings;
    void readSettings();
    QList<Login> Logins;

    void initUserPass();

};

#endif // LOGINDIALOG_H

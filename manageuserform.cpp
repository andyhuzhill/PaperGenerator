#include "manageuserform.h"
#include "ui_manageuserform.h"

#include <QSettings>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDebug>

manageUserForm::manageUserForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::manageUserForm)
{
    ui->setupUi(this);

    setWindowTitle(tr("用户管理窗口"));

    settings = new QSettings(OrgName, AppName);

    int size = settings->beginReadArray("logins");

    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Login login;
        login.name = settings->value("name").toString();
        login.passwd = settings->value("pass").toString();
        Logins.append(login);

        ui->UserLists->addItem(login.name);
    }
    settings->endArray();
}

manageUserForm::~manageUserForm()
{
    delete ui;
}

void manageUserForm::on_exitButton_clicked()
{
    close();
}

void manageUserForm::on_addUserButton_clicked()
{
    QString name = ui->EditUserName->text().trimmed();
    QString passwd = ui->EditPasswd->text().trimmed();
    QString cpasswd = ui->ConfirmPasswd->text().trimmed();

    if (name.isEmpty() || passwd.isEmpty() || cpasswd.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("用户名或密码不能为空！"), QMessageBox::Ok);

        ui->EditPasswd->clear();
        ui->EditUserName->clear();
        ui->ConfirmPasswd->clear();
        ui->EditUserName->setFocus();

        return ;
    }

    if (passwd != cpasswd) {
        QMessageBox::warning(this, tr("警告"), tr("两次输入的密码不一致"),QMessageBox::Ok);

        ui->EditPasswd->clear();
        ui->EditUserName->clear();
        ui->ConfirmPasswd->clear();
        ui->EditUserName->setFocus();

        return ;
    }

    Login log;
    log.name = name;

    QByteArray bp = QCryptographicHash::hash(passwd.toAscii(), QCryptographicHash::Md5);
    QString pwd2md5 = bp.toHex();

    log.passwd = pwd2md5;

    ui->UserLists->clear();

    int size = Logins.size();

    settings = new QSettings(OrgName, AppName);

    bool same;

    settings->beginWriteArray("logins");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        if (log.name == Logins[i].name) {
            Logins[i].passwd = log.passwd;
            same = true;
        }
        settings->setValue("name", Logins.at(i).name);
        settings->setValue("pass", Logins.at(i).passwd);
        ui->UserLists->addItem(Logins.at(i).name);
    }
    settings->endArray();

    if (!same) {
        Logins.append(log);
        ui->UserLists->addItem(log.name);
    }
}



void manageUserForm::on_UserLists_itemClicked(QListWidgetItem *item)
{
    ui->EditUserName->setText(item->text());
}

void manageUserForm::on_deleteUserButton_clicked()
{
    if (ui->UserLists->count() == 1) {
        QMessageBox::warning(this, tr("警告"), tr("这已经是最后一个用户了，不可以删除！"), QMessageBox::Ok);
        return ;
    }
    QString name = ui->EditUserName->text().trimmed();
    QString passwd = ui->EditPasswd->text().trimmed();
    QString cpasswd = ui->ConfirmPasswd->text().trimmed();

    settings = new QSettings(OrgName, AppName);

    if (name.isEmpty() || passwd.isEmpty() || cpasswd.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("删除用户前请输入确认密码！"), QMessageBox::Ok);

        ui->EditPasswd->clear();
        ui->EditUserName->clear();
        ui->ConfirmPasswd->clear();
        ui->EditUserName->setFocus();

        return ;
    }

    if (passwd != cpasswd) {
        QMessageBox::warning(this, tr("警告"), tr("两次输入的密码不一致"),QMessageBox::Ok);

        ui->EditPasswd->clear();
        ui->EditUserName->clear();
        ui->ConfirmPasswd->clear();
        ui->EditUserName->setFocus();

        return ;
    }

    QByteArray bp = QCryptographicHash::hash(passwd.toAscii(), QCryptographicHash::Md5);
    QString pwd2md5 = bp.toHex();

    int size = Logins.size();
    bool find=false;
    int i;

    for (i = 0; i < size; ++i) {
        if (Logins[i].name == name) {
            find = true;
            break;
        }
    }

    if (!find) {
        QMessageBox::warning(this, tr("错误"), tr("没有该用户%1").arg(name), QMessageBox::Ok);
        return ;
    }

    if (pwd2md5 != Logins[i].passwd) {
        QMessageBox::warning(this, tr("警告"),tr("密码错误，无法删除"),QMessageBox::Ok);
        return ;
    }else{
        settings->remove(QString("logins/%1").arg(name));
        Logins.removeAt(i);
        QMessageBox::information(this, tr("通知"), tr("删除成功"), QMessageBox::Ok);
        size --;
    }

    ui->UserLists->clear();

    settings->beginWriteArray("logins");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        settings->setValue("name", Logins.at(i).name);
        settings->setValue("pass", Logins.at(i).passwd);
        ui->UserLists->addItem(Logins.at(i).name);
    }
    settings->endArray();
}

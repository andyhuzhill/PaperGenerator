#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QCryptographicHash>   //加密函数库

#include "firstsettingsdialog.h"
#include "QTextCodec"
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    readSettings();     //读取保存的用户名和密码

    QSettings settings("app.ini", QSettings::IniFormat);

    settings.setIniCodec(QTextCodec::codecForName("GB2312"));

    QString strOrgName = settings.value("orgName").toString();
    ui->orgLabel->setText(strOrgName);


    int strLength = strOrgName.length()*20;
    ui->orgLabel->setMaximumWidth(strLength);
    ui->orgLabel->setFont(QFont(tr("楷体"), 16));

    //设置密码输入框显示*
    ui->passEdit->setEchoMode(QLineEdit::Password);
    //将焦点设为用户名输入框
    ui->userLabel->setFocus();

    setWindowIcon(QIcon(":/images/computer.png"));
    setWindowTitle(tr("试卷自动化生成工具"));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_quitButton_clicked()
{
    //拒绝登陆
    reject();
}

void LoginDialog::on_loginButton_clicked()
{
    int size = Logins.size();

    QString username = ui->nameEdit->text().trimmed();
    QString password = ui->passEdit->text().trimmed();

    QByteArray bp = QCryptographicHash::hash(password.toAscii(), QCryptographicHash::Md5);
    QString pwd2md5 = bp.toHex();

    for (int i = 0; i < size; ++i) {
        if (username == Logins.at(i).name &&
                pwd2md5 == Logins.at(i).passwd) {
            accept();
            return ;
        }
    }

    QMessageBox::warning(this, tr("警告"),tr("用户名或密码错误！"), QMessageBox::Ok);
    ui->passEdit->clear();
    ui->nameEdit->clear();
    ui->nameEdit->setFocus(); //将密码框和用户名框清空 并设置用户名框为焦点
}

void LoginDialog::readSettings()
{
    settings = new QSettings(OrgName, AppName);

    int size = settings->beginReadArray("logins");
    if (size == 0) {
        initUserPass();
        size ++;
    }

    Logins.clear();

    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Login login;
        login.name = settings->value("name").toString();
        login.passwd = settings->value("pass").toString();
        Logins.append(login);
    }

    settings->endArray();
}

void LoginDialog::initUserPass()
{
    FirstSettingsDialog firstDialog;

    if (firstDialog.exec() != QDialog::Accepted) {
        reject();
    }
}

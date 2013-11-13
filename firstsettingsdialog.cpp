#include "firstsettingsdialog.h"
#include "ui_firstsettingsdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCryptographicHash>

FirstSettingsDialog::FirstSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstSettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("初次使用设置"));
}

FirstSettingsDialog::~FirstSettingsDialog()
{
    delete ui;
}

void FirstSettingsDialog::closeEvent(QCloseEvent *)
{
    qApp->quit();
}


void FirstSettingsDialog::on_choosePathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("请选择保存试题库的文件夹"));

    dir.replace("/", "\\");

    if (!dir.isEmpty()) {
        ui->pathEdit->setText(dir);
    }
}

void FirstSettingsDialog::on_acceptButton_clicked()
{
    QString name = ui->usrEdit->text().trimmed();
    QString pass = ui->passEdit->text().trimmed();
    QString comfirm = ui->comfirmEdit->text().trimmed();
    QString path = ui->pathEdit->text().trimmed().replace("/","\\");

    if (name.isEmpty() || pass.isEmpty() || comfirm.isEmpty()) {
        QMessageBox::warning(this, tr("警告"),tr("用户名或密码不能为空！"), QMessageBox::Ok);

        ui->usrEdit->setFocus();
        return ;
    }

    if (pass != comfirm) {
        QMessageBox::warning(this, tr("警告"), tr("两次输入的密码不一致！"), QMessageBox::Ok);
        ui->passEdit->setFocus();
        return ;
    }

    if (path.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("试题库保存的地址不能为空！"), QMessageBox::Ok);
        ui->pathEdit->setFocus();
        return;
    }

    QByteArray bp = QCryptographicHash::hash(pass.toAscii(), QCryptographicHash::Md5);
    QString pwd2md5 = bp.toHex();

    settings = new QSettings(OrgName, AppName);

    settings->beginWriteArray("logins");
    settings->setArrayIndex(0);
    settings->setValue("name", name);
    settings->setValue("pass", pwd2md5);
    settings->endArray();

    settings->setValue("paperPath", path);

    accept();
}


void FirstSettingsDialog::on_pushButton_clicked()
{
    qApp->quit();
}

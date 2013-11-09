#include <QtGui/QApplication>
#include "mainwindow.h"
#include "logindialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QTextCodec>
#include <QTranslator>
#include <QWizard>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include <iostream>

bool connect2Database()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("./papergen.db");

    if (!db.open()) {
        std::cout <<  "错误:无法连接到数据库！" << std::endl;
        return false;
    }

    return true;
}

QWizardPage *IntroPage()
{
    QWizardPage *page = new QWizardPage;

    page->setTitle(QObject::tr("欢迎使用 试题生成系统 "));

    page->setSubTitle(QObject::tr("系统检测到您是第一次运行本软件，需要做一些设置"));

    QLabel *label = new QLabel(QObject::tr("<h3>    第一次运行，需要您告诉我：<br>"
                                           "你是谁？<br>"
                                           "你要设置什么样的密码？<br>"
                                           "还有你想将试题库保存到哪？"
                                           "</h3>"
                                           ));

    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(label);

    page->setLayout(layout);

    return page;
}

QWizardPage *SettingPage()
{
    QWizardPage *page = new QWizardPage;

    page->setWindowTitle(QObject::tr("用户信息"));

    page->setTitle(QObject::tr("设置用户信息"));

    page->setSubTitle(QObject::tr("请在下面文本框设置您的用户名和密码："));

    QLabel *nameLabel = new QLabel(QObject::tr("用户名"));

    QLineEdit *nameEdit  = new QLineEdit();

    QLabel *passLabel = new QLabel(QObject::tr("密码："));

    QLineEdit *passEdit = new QLineEdit();

    passEdit->setEchoMode(QLineEdit::Password);

    QLabel *comfirmLabel = new QLabel(QObject::tr("确认密码："));

    QLineEdit *comfirmEdit = new QLineEdit();

    comfirmEdit->setEchoMode(QLineEdit::Password);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
    layout->addWidget(passLabel);
    layout->addWidget(passEdit);
    layout->addWidget(comfirmLabel);
    layout->addWidget(comfirmEdit);

    page->setLayout(layout);

    return page;
}


int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));  //解决中文编码问题
    QTranslator translator;
    translator.load(":/lang/qt_zh_CN.qm");      //载入对话框的汉化文件

    QApplication app(argc, argv);
    app.installTranslator(&translator);           //汉化对话框

    QApplication::addLibraryPath("./plugins");

    QWizard wizard;
    wizard.addPage(IntroPage());
    wizard.addPage(SettingPage());
    wizard.setWindowTitle(QObject::tr("试题生成系统初次设置"));
    wizard.setWizardStyle(QWizard::ModernStyle);
    wizard.setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

//    wizard.show();


    LoginDialog login;        //登陆窗口

    if (!connect2Database()) {
        return 0;
    }
#if 0
    if (login.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();           //只有在登陆验证通过后才显示主窗口

        return app.exec();
    }

    return 0;
#else
//    if (wizard.exec() == QWizard::Accepted) {
//        MainWindow w;
//        w.show();

//        return app.exec();
//    }

//    return 0;
    MainWindow w;
    w.show();

    return app.exec();
#endif
}

#include <QtGui/QApplication>
#include "mainwindow.h"
#include "logindialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QTextCodec>
#include <QTranslator>
#include <iostream>

bool connect2Database(void)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("./papergen.db");

    if (!db.open()) {
        std::cout <<  "错误:无法连接到数据库！" << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));  //解决中文编码问题
    QTranslator translator;
    translator.load(":/lang/qt_zh_CN.qm");      //载入对话框的汉化文件

    QApplication a(argc, argv);
    a.installTranslator(&translator);           //汉化对话框

    QApplication::addLibraryPath("./plugins");

    LoginDialog login;        //登陆窗口

    if (!connect2Database()) {
        return 0;
    }
#if 0
    if (login.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();           //只有在登陆验证通过后才显示主窗口

        return a.exec();
    }

    return 0;
#else
    MainWindow w;
    w.show();

    return a.exec();
#endif
}

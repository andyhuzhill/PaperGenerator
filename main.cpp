#include <QtGui/QApplication>
#include "mainwindow.h"
#include "logindialog.h"

#include <QSqlDatabase>
#include <QTextCodec>
#include <QTranslator>
#include <autonewpaper.h>

#include <iostream>

/** @brief 连接数据库
 *
 *  @return 返回 true 成功连接到数据库 false 没有连接到数据库
 */
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /** 载入对话框的汉化文件 */
    QTranslator translator;
    translator.load(":/lang/qt_zh_CN.qm");

    /** 汉化对话框 */
    app.installTranslator(&translator);

    QApplication::addLibraryPath("./plugins");

    /** 解决中文编码问题 */
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));

    if (!connect2Database()) {
        return -1;
    }

    /** 登陆窗口 */
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        MainWindow w;

        /** 禁用窗口最大化按钮 */
        w.setWindowFlags(w.windowFlags() & ~Qt::WindowMaximizeButtonHint);

        /** 只有在登陆验证通过后才显示主窗口 */
        w.show();

        return app.exec();
    }
    return -1;
}

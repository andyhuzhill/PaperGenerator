#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QAction>
#include <QSqlQuery>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QDesktopServices>

#include <newsubjectform.h>
#include <newtestform.h>
#include <manageuserform.h>

#include <QSslConfiguration>
#include <QFile>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/images/computer.png"));
    setWindowTitle(tr("试卷自动生成系统"));

    createActions();
    createMenus();
    //    createToolBars();

    ui->subjectsView->setColumnCount(3);
    ui->subjectsView->setHeaderLabels(QStringList() << tr("课程名") << tr("题型") << tr("题目数量"));
    ui->subjectsView->setIconSize(QSize(60,60));

    textViewRefresh();
    connect(ui->newTestButton, SIGNAL(clicked()), this, SLOT(newTest()));
    connect(ui->manageQuestionButton, SIGNAL(clicked()), this, SLOT(manageSubject()));

    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onCheckUpdate()));

    checkUpdate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/** @brief 新建一套试卷 */
void MainWindow::newTest()
{
    testForm = new newTestForm();
    testForm->setWindowFlags(testForm->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    testForm->show();
    connect(testForm, SIGNAL(contentChanged()), this, SLOT(textViewRefresh()));
}

/** @brief 管理试题库 */
void MainWindow::manageSubject()
{
    subjectForm = new newSubjectForm();
    subjectForm->setWindowFlags(subjectForm->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    subjectForm->show();
    connect(subjectForm, SIGNAL(contentChanged()), this, SLOT(textViewRefresh()));
}

/** @brief  管理用户及密码 */
void MainWindow::manageUser()
{
    userForm = new manageUserForm();

    userForm->show();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("试卷自动生成系统"),
                       tr("<h1> 题库管理与试卷自动生成系统 </h1><br>"
                          " 作者： Andy Scout <br>"
                          " 联系方式： andyhuzhill@gmail.com <br>"
                          "个人主页： <a href=\"http://andyhuzhill.github.io\">http://andyhuzhill.github.io</a><br>"
                          "<a href='http://me.alipay.com/huzhill'>http://me.alipay.com/huzhill</a> <br>"
                          "以上是我的支付宝收款主页<br>如果你觉得我的程序还不错可以向我捐赠XD~")
                       );
}

void MainWindow::focusInEvent(QFocusEvent *)
{
    textViewRefresh();
}

void MainWindow::help()
{
    QDir dir(".");

    QDesktopServices::openUrl(QUrl::fromLocalFile(QString("%1/help.doc").arg(dir.absolutePath())));
}

void MainWindow::textViewRefresh()
{
    ui->subjectsView->clear();
    ui->listWidget->clear();

    QSqlQuery query;
    query.exec("SELECT subjectName FROM subjects");
    QTreeWidgetItem *item = NULL;
    QList<QTreeWidgetItem*> itemList;

    while(query.next()){
        item = new QTreeWidgetItem(ui->subjectsView);
        item->setText(0, query.value(0).toString());
        itemList.append(item);
    }

    foreach (QTreeWidgetItem *it, itemList) {
        query.exec(QString("SELECT questionTypes, numOfQuestions FROM '%1'").arg(it->text(0)));
        while(query.next()){
            QTreeWidgetItem *childItem = new QTreeWidgetItem(it);
            childItem->setText(1, query.value(0).toString());
            childItem->setText(2, query.value(1).toString());
        }
    }

    query.exec("SELECT paperName FROM savedPaper ");
    while (query.next()) {
        QString paperName = query.value(0).toString();
        QListWidgetItem *item;
        if (paperName == "autoSavedPaper") {
            item = new QListWidgetItem(tr("自动保存试卷"));
        }else{
            item = new QListWidgetItem(query.value(0).toString());
        }
        ui->listWidget->addItem(item);
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

/** @brief 创建一系列操作以及设置操作的快捷键 */
void MainWindow::createActions()
{
    newTestAction = new QAction(QIcon(":/images/new.png"),tr("新建试卷"), this);
    newTestAction->setShortcut(QKeySequence::New);
    newTestAction->setStatusTip(tr("根据已建的试题库创建一份试卷"));
    connect(newTestAction, SIGNAL(triggered()), this, SLOT(newTest()));

    manageSubjectAction = new QAction(QIcon(":/images/window-new.png"), tr("管理题库"), this);
    manageSubjectAction->setStatusTip(tr("管理试题库"));
    manageSubjectAction->setShortcut(tr("Ctrl+M"));
    connect(manageSubjectAction, SIGNAL(triggered()), this, SLOT(manageSubject()));

    manageUserAction = new QAction(QIcon(":/images/add.png"), tr("用户管理"), this);
    manageUserAction->setStatusTip(tr("添加用户或修改密码"));
    manageUserAction->setShortcut(tr("Ctrl+A"));
    connect(manageUserAction, SIGNAL(triggered()), this, SLOT(manageUser()));

    quitAction = new QAction(QIcon(":/images/quit.png"), tr("退出系统"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    helpAction = new QAction(tr("帮助文档"), this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    aboutQtAction = new QAction(tr("关于Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

/** @brief 创建主菜单  */
void MainWindow::createMenus()
{
    fileMenu = ui->menuBar->addMenu(tr("开始工作"));
    fileMenu->addAction(newTestAction);
    fileMenu->addAction(manageSubjectAction);
    fileMenu->addAction(manageUserAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    helpMenu = ui->menuBar->addMenu(tr("帮助信息"));
    helpMenu->addAction(helpAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutQtAction);
}

/** @brief 创建工具栏  */
void MainWindow::createToolBars()
{
//    ui->mainToolBar->addAction(newTestAction);
    //    ui->mainToolBar->addAction(manageSubjectAction);
}

void MainWindow::checkUpdate()
{
    QNetworkRequest request;

    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    request.setUrl(QUrl("https://raw.githubusercontent.com/dqyxxgcxy/PaperGenerator/master/VERSION"));

    reply = manager->get(request);

    reply->ignoreSslErrors();
}

/** @brief 双击试卷列表时 跳到该试卷 */
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    testForm = new newTestForm();
    testForm->setWindowFlags(testForm->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    if (item->text() == tr("自动保存的试卷")) {
        testForm->setPaperName("autoSavedPaper");
    }else{
        testForm->setPaperName(item->text());
    }

    testForm->show();

    connect(testForm, SIGNAL(contentChanged()), this, SLOT(textViewRefresh()));
}

/** @brief 删除选定的试卷  */
void MainWindow::on_deleteSelectPaper_clicked()
{
    QSqlQuery query;
    QList<QListWidgetItem *> selectedPaper = ui->listWidget->selectedItems();
    if (selectedPaper.length() == 0) {
        QMessageBox::warning(this, tr("警告"), tr("请先选中要删除的试卷名称!"), QMessageBox::Ok);
        return ;
    }

    foreach (QListWidgetItem *item, selectedPaper) {
        if (item->text() == tr("自动保存的试卷")) {
            QMessageBox::information(this, tr("通知"), tr("自动保存的试卷无法删除"), QMessageBox::Ok);
            continue;
        }

        bool status1 = query.exec(QString("DROP TABLE '%1_Paper'").arg(item->text()));
        bool status2 = query.exec(QString("DELETE FROM savedPaper where paperName = '%1'").arg(item->text()));

        if (!status1 || !status2) {
            QMessageBox::warning(this, tr("警告"), tr("无法删除试卷<%1>").arg(item->text()));
            return;
        }
    }

    textViewRefresh();
}


void MainWindow::showUpdate()
{
     QMessageBox::information(this, tr("有更新可用"), tr("您好,本程序有更新的版本可用。请到如下地址下载最新版本安装：<br><a href='https://raw.githubusercontent.com/dqyxxgcxy/PaperGenerator/master/PaperGeneratorSetup.exe'>https://raw.githubusercontent.com/dqyxxgcxy/PaperGenerator/master/PaperGeneratorSetup.exe</a>"));

}

void MainWindow::onCheckUpdate()
{
    QString Version = QString::fromUtf8(reply->readAll());

    int Major = Version.split(".").at(0).toInt();

    int Minor = Version.split(".").at(1).toInt();

    int Min   = Version.split(".").at(2).toInt();

    if ((curMajor < Major)
            ||(curMinor < Minor)
            ||(curMin  < Min)
            ){
        showUpdate();
    }

}




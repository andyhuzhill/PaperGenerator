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
    createToolBars();

    textViewRefresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newTest()
{
    testForm = new newTestForm();
    testForm->setWindowFlags(testForm->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    testForm->show();
    connect(testForm, SIGNAL(contentChanged()), this, SLOT(textViewRefresh()));
}

void MainWindow::manageSubject()
{
    subjectForm = new newSubjectForm();
    subjectForm->setWindowFlags(subjectForm->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    subjectForm->show();
    connect(subjectForm, SIGNAL(contentChanged()), this, SLOT(textViewRefresh()));
}

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
    ui->textBrowser->clear();
    ui->listWidget->clear();

    ui->textBrowser->insertPlainText(tr("课程名称       题目类型        题目数量\n"));
    ui->textBrowser->insertPlainText("==========================================\n");

    QSqlQuery query;
    query.exec("SELECT subjectName FROM subjects");
    QStringList subjectNames;
    while(query.next()){
        subjectNames.append(query.value(0).toString());
    }

    foreach (QString subject, subjectNames) {
        ui->textBrowser->insertPlainText(tr("%1\n").arg(subject));
        query.exec(QString("SELECT questionTypes, numOfQuestions FROM '%1'").arg(subject));
        while(query.next()){
            ui->textBrowser->insertPlainText(tr("              %1             %2\n").arg(query.value(0).toString()).arg(query.value(1).toInt()));
        }
    }

    query.exec("SELECT paperName FROM savedPaper ");
    while (query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());
        ui->listWidget->addItem(item);
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

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

    manageUserAction = new QAction(QIcon(":/images/add.png"), tr("添加用户"), this);
    manageUserAction->setStatusTip(tr("添加一位用户"));
    manageUserAction->setShortcut(tr("Ctrl+A"));
    connect(manageUserAction, SIGNAL(triggered()), this, SLOT(manageUser()));

    quitAction = new QAction(QIcon(":/images/quit.png"), tr("退出系统"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    //    aboutAction = new QAction(tr("关于本程序"), this);
    //    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    helpAction = new QAction(tr("帮助文档"), this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    aboutQtAction = new QAction(tr("关于Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

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
    //    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    ui->mainToolBar->addAction(newTestAction);
    ui->mainToolBar->addAction(manageSubjectAction);
}



void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    testForm = new newTestForm();
    testForm->setWindowFlags(testForm->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    testForm->setPaperName(item->text());
    testForm->show();
    connect(testForm, SIGNAL(contentChanged()), this, SLOT(textViewRefresh()));
}

void MainWindow::on_deleteSelectPaper_clicked()
{
    QSqlQuery query;
    QList<QListWidgetItem *> selectedPaper = ui->listWidget->selectedItems();
    if (selectedPaper.length() == 0) {
        QMessageBox::warning(this, tr("警告"), tr("请先选中要删除的试卷名称!"), QMessageBox::Ok);
        return ;
    }
    foreach (QListWidgetItem *item, selectedPaper) {
        if (item->text() == "autoSavedPaper") {
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

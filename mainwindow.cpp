#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <QAction>
#include <QMenu>
#include <QSqlQuery>
#include <ActiveQt/QAxWidget>
#include <ActiveQt/QAxObject>

#include <QDebug>
#include <QTextCodec>
#include <QDir>
#include <QSettings>
#include "defs.h"

#include <newsubjectform.h>
#include <newtestform.h>
#include <manageuserform.h>

#include <docreadwriter.h>
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

    QSettings settings(OrgName, AppName);

    paperPath = settings.value("paperPath").toString();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), tr("."), tr("*.doc"));

    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("没有提供文件名"), QMessageBox::Ok);
        return ;
    }

    word = new QAxWidget("Word.Application");

    if (!word) {
        QMessageBox::warning(this, tr("错误"),tr("无法找到Word程序，请安装！"),QMessageBox::Ok);
        return;
    }
    word->setProperty("Visible", true);

    DocReadWriter *docRDWR = new DocReadWriter(this, fileName, paperPath);

    QAxObject *docs = word->querySubObject("Documents");

    docRDWR->setDocuemnt(docs);

    ui->statusBar->setStatusTip(tr("正在处理,请稍侯"));
    if(!docRDWR->convert()){
        QMessageBox::warning(this, tr("错误"), tr("文件信息提取错误！"), QMessageBox::Ok);
        return ;
    }
    ui->statusBar->setStatusTip(tr("处理完成"));

    QString Question = docRDWR->getQuestion();
    QString Answer = docRDWR->getAnswer();

    ui->textBrowser->insertPlainText(tr("题目如下\n"));
    ui->textBrowser->insertHtml(Question);
    ui->textBrowser->insertPlainText(tr("答案如下\n"));
    ui->textBrowser->insertHtml(Answer);

    delete docRDWR;

    word->dynamicCall("Quit(boolean)", true);

    delete word;
}

void MainWindow::newTest()
{
    testForm = new newTestForm();

    testForm->show();
}

void MainWindow::manageSubject()
{
    subjectForm = new newSubjectForm();

    subjectForm->show();
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
                          "个人主页： <a href=\"http://andyhuzhill.github.io\">http://andyhuzhill.github.io</a>")
                       );
}

void MainWindow::createActions()
{
    newTestAction = new QAction(QIcon(":/images/new.png"),tr("新建试卷"), this);
    newTestAction->setShortcut(QKeySequence::New);
    newTestAction->setStatusTip(tr("根据已建的试题库创建一份试卷"));
    connect(newTestAction, SIGNAL(triggered()), this, SLOT(newTest()));

    manageSubjectAction = new QAction(QIcon(":/images/window-new.png"), tr("管理题库"), this);
    manageSubjectAction->setStatusTip(tr("管理试题库"));
    connect(manageSubjectAction, SIGNAL(triggered()), this, SLOT(manageSubject()));

    manageUserAction = new QAction(QIcon(":/images/add.png"), tr("添加用户"), this);
    manageUserAction->setStatusTip(tr("添加一位用户"));
    connect(manageUserAction, SIGNAL(triggered()), this, SLOT(manageUser()));

    quitAction = new QAction(QIcon(":/images/quit.png"), tr("退出系统"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    aboutAction = new QAction(tr("关于本程序"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

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
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    ui->mainToolBar->addAction(newTestAction);
    ui->mainToolBar->addAction(manageSubjectAction);
}



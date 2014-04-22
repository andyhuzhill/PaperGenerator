/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file subjectmanager.cpp
** @brief
**
** @version 1.0
** @author 胡震宇 <andyhuzhill@gmail.com>
**
** @date 2014/4/12
**
** @revision： 最初版本
*******************************************************************************
*/

#include "subjectmanager.h"
#include "ui_subjectmanager.h"
#include <QMessageBox>

SubjectManager::SubjectManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubjectManager)
{
    ui->setupUi(this);

    NewSubject  *subject = new NewSubject(this);
    NewQuestion *question = new NewQuestion(this);
    ModifyQuestion *modifyQuestion = new ModifyQuestion(this);

    connect(subject, SIGNAL(dataChanged()), question, SLOT(onDataChanged()));
    connect(subject, SIGNAL(dataChanged()), modifyQuestion, SLOT(onDataChanged()));

    connect(question, SIGNAL(dataChanged()), modifyQuestion, SLOT(onDataChanged()));
    connect(modifyQuestion, SIGNAL(dataChanged()), question, SLOT(onDataChanged()));

    ui->stackedWidget->addWidget(subject);
    ui->stackedWidget->addWidget(question);
    ui->stackedWidget->addWidget(modifyQuestion);
    ui->listWidget->addItem(tr("新建课程与题型"));
    ui->listWidget->addItem(tr("录入新题"));
    ui->listWidget->addItem(tr("查看或修改题目"));

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));

    setWindowTitle(tr("试题库管理"));
}

SubjectManager::~SubjectManager()
{
    delete ui;
}

void SubjectManager::closeEvent(QCloseEvent *)
{
    emit contentChanged();
}


void SubjectManager::on_returnButton_clicked()
{
    emit contentChanged();
    close();
}

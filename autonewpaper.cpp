/**
*******************************************************************************
** Copyright (c) 2014
** All rights reserved.
**
** @file autonewpaper.cpp
** @brief
**
** @version 1.0
** @author 胡震宇 <andyhuzhill@gmail.com>
**
** @date 2014/4/22
**
** @revision： 最初版本
*******************************************************************************
*/

#include "autonewpaper.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSpinBox>

#include <QSqlQuery>

#include <QDebug>

AutoNewPaper::AutoNewPaper(QWidget *parent)
    : QWizard(parent)
{
    addPage(new SubjectSetup);
    addPage(new QuestionTypes);
    addPage(new PointSetup);
}

SubjectSetup::SubjectSetup(QWidget *parent)
    :QWizardPage(parent)
{
    this->setTitle(tr("<h1>本向导将引导您设置自动抽卷的约束条件。</h1>"));
    setSubTitle(tr("<h3>请选择您要组卷的科目:</h3>"));

    QComboBox *subjectBox = new QComboBox(this);

    QSqlQuery query;

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        subjectBox->addItem(query.value(0).toString());
    }

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(subjectBox);

    setLayout(layout);

    registerField("subjectName", subjectBox, "currentText");
}


QuestionTypes::QuestionTypes(QWidget *parent)
    : QWizardPage(parent)
{

}

void QuestionTypes::initializePage()
{
    if (this->layout()){
        delete this->layout();
    }

    QString subjectName = field("subjectName").toString();

    setSubTitle(tr("<h3>请选择各个题型的题目数量：</h3>"));

    QSqlQuery query;



    query.exec(QString("SELECT questionTypes FROM '%1'").arg(subjectName));
    QStringList questionTypeList;

    while (query.next()) {
        questionTypeList.append(query.value(0).toString());
    }

    QVBoxLayout *layout = new QVBoxLayout();

    foreach (QString questionType, questionTypeList) {

        QLabel *label = new QLabel(questionType);
        QSpinBox *numsOfQuestion = new QSpinBox();
        label->setBuddy(numsOfQuestion);
        numsOfQuestion->setMinimum(0);

        query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'" ).arg(subjectName).arg(questionType));
        query.next();
        numsOfQuestion->setMaximum(query.value(0).toInt());

        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->addWidget(label);
        hLayout->addWidget(numsOfQuestion);
        layout->addLayout(hLayout);
        registerField(QString("%1_%2").arg(subjectName).arg(questionType), numsOfQuestion);
    }

    setLayout(layout);
}


PointSetup::PointSetup(QWidget *parent)
    : QWizardPage(parent)
{
    TypeCount = 0;
    QuestionNumCount = 0;
}

void PointSetup::initializePage()
{
    if (this->layout()) {
        delete this->layout();
    }

    QString subjectName = field("subjectName").toString();
    qDebug() << subjectName;

    setSubTitle(tr("<h3>请设置各个题目的知识点</h3>"));

    QSqlQuery query;

    query.exec(QString("SELECT questionTypes FROM '%1'").arg(subjectName));

    QStringList questionTypeList;
    while (query.next()) {
        questionTypeList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();
    }

    foreach (QString questionType, questionTypeList) {

    }

    QuestionNumCount = field(QString("%1_%2").arg(subjectName).arg(questionTypeList.at(TypeCount))).toInt();

    QVBoxLayout *layout = new QVBoxLayout;
    for (int i = 1; i < QuestionNumCount+1; ++i) {
        QHBoxLayout *hlayout = new QHBoxLayout();

        QLabel *label = new QLabel(tr("题目%1").arg(i));

        QComboBox *pointCombo = new QComboBox;

        pointCombo->addItem(tr("任意知识点"));

        QStringList points = getPoints(subjectName, questionTypeList.at(TypeCount));

        foreach (QString point, points) {
            pointCombo->addItem(point);
        }
        hlayout->addWidget(label);
        hlayout->addWidget(pointCombo);

        layout->addLayout(hlayout);
    }

    setLayout(layout);
}

QStringList PointSetup::getPoints(QString subjectName, QString questionTypeName)
{
    QStringList Points;
    QSqlQuery query;
    query.exec(QString("SELECT Point FROM '%1_%2'").arg(subjectName).arg(questionTypeName));
    Points.clear();
    while (query.next()) {
        QString point = query.value(0).toString();
        if (Points.contains(point) || point == "") {
            continue;
        }
        Points.append(point);
    }
    return Points;
}

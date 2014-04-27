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
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>

#include <QSqlQuery>
#include <QVariant>

#include <QDebug>

AutoNewPaper::AutoNewPaper(QWidget *parent)
    : QWizard(parent)
{
    addPage(new SubjectSetup);
    addPage(new QuestionTypes);
    addPage(new PointSetup);

    setPixmap(QWizard::BackgroundPixmap, QPixmap("/images/background.png"));
    setWindowTitle(tr("自动生成新试卷向导"));
}

SubjectSetup::SubjectSetup(QWidget *parent)
    :QWizardPage(parent)
{
    this->setTitle(tr("<h1>本向导将引导您设置自动抽卷的约束条件。</h1>"));
    setSubTitle(tr("<h2>请选择您要组卷的科目:</h2>"));

    QLabel *label = new QLabel(tr("题库中现有科目:"));
    QComboBox *subjectBox = new QComboBox(this);

    QSqlQuery query;

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        subjectBox->addItem(query.value(0).toString());
    }

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(label);
    layout->addWidget(subjectBox);

    setLayout(layout);

    if (field("subjectName").isNull()) {
        registerField("subjectName", subjectBox, "currentText");
    } else {
        setField("subjectName", subjectBox->currentText());
        connect(subjectBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSubjectChanged(QString)));
    }
}

void SubjectSetup::onSubjectChanged(QString subjectName)
{
    setField("subjectName", subjectName);

    qDebug() << "subjectName changed to:" << field("subjectName").toString();
}


QuestionTypes::QuestionTypes(QWidget *parent)
    : QWizardPage(parent)
{
    typeLabel = NULL;
    numOfQuestionSpinBox = NULL;
    numsLabel = NULL;
    questionTypeComboBox = NULL;
    degreeLabel = NULL;
    degreeOfQuestionType = NULL;
    addButton = NULL;
}

void QuestionTypes::initializePage()
{
    if (this->layout()){
        delete this->layout();
    }

    subjectName = field("subjectName").toString();

    setTitle(tr("<h1>科目:%1</h1>").arg(subjectName));

    setSubTitle(tr("<h2>请设置各个题型的题目数量与总分值：</h2>"));

    QSqlQuery query;

    query.exec(QString("SELECT questionTypes FROM '%1'").arg(subjectName));

    questionTypeList.clear();

    while (query.next()) {
        questionTypeList.append(query.value(0).toString());
    }

    QVBoxLayout *layout = new QVBoxLayout();

    tableWidget = new QTableWidget(questionTypeList.length() , 3);

    tableWidget->setEditTriggers(QTableView::NoEditTriggers);

    tableWidget->setHorizontalHeaderLabels(QStringList() << tr("题目类型") << tr("题目数量") <<  tr("题型总分"));

//    int row = 0;
//    foreach (QString questionType, questionTypeList) {
//        tableWidget->setItem(row, 0, new QTableWidgetItem(questionType));

//        QSpinBox *numOfQuestionSpinBox = new QSpinBox;
//        numOfQuestionSpinBox->setMinimum(0);

//        query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes == '%2'").arg(subjectName).arg(questionType));
//        query.next();
//        numOfQuestionSpinBox->setMaximum(query.value(0).toInt());

//        tableWidget->setCellWidget(row, 1, numOfQuestionSpinBox);

//        QSpinBox *degree = new QSpinBox;
//        degree->setMinimum(0);

//        tableWidget->setCellWidget(row, 2, degree);

//        QString numOfQuestField = tr("%1_%2").arg(subjectName).arg(questionType);
//        if (field(numOfQuestField).isNull()) {
//            registerField(numOfQuestField, numOfQuestionSpinBox);
//            connect(numOfQuestionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onNumOfQuestionChanged(int)));
//            connect(numOfQuestionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(printValue(int)));
//        }

//        this->questionType = questionType;
//        row ++;
//    }

////    connect(tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(onTableCellEnter(int,int)));
////    connect(tableWidget, SIGNAL(cellActivated(int,int)), this, SLOT(onTableCellEnter(int,int)));
//    connect(tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(onTableCellEnter(int,int)));
////    connect(tableWidget, SIGNAL(cellEntered(int,int)), this, SLOT(onTableCellEnter(int, int)));
////    connect(tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(onTableCellChanged(int,int,int,int)));
///

    if (typeLabel == NULL) {
        typeLabel = new QLabel(tr("题目类型:"));
    }

    if (questionTypeComboBox == NULL) {
        questionTypeComboBox = new QComboBox;
    }

    foreach(QString type, questionTypeList){
        questionTypeComboBox->addItem(type);
    }

    connect(questionTypeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onQuestionTypeChanged(QString)));
    query.exec(tr("SELECT numOfQuestion FROM '%1' WHERE questionType == '%2'").arg(subjectName).arg(questionTypeComboBox->currentText()));

    query.next();

    if (numsLabel == NULL) {
        numsLabel = new QLabel(tr("题目数量:"));
    }
    if (numOfQuestionSpinBox == NULL) {
        numOfQuestionSpinBox = new QSpinBox;
    }
    numOfQuestionSpinBox->setMinimum(0);
    numOfQuestionSpinBox->setMaximum(query.value(0).toInt());

    if (degreeLabel == NULL) {
        degreeLabel = new QLabel(tr("题型总分:"));
    }
    if (degreeOfQuestionType == NULL) {
        degreeOfQuestionType = new QSpinBox;
    }
    degreeOfQuestionType->setMinimum(0);

    if (addButton == NULL) {
        addButton = new QPushButton(tr("添加题型"));
    }
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));

    QHBoxLayout *hLayout = new QHBoxLayout();

    hLayout->addWidget(typeLabel);
    hLayout->addWidget(questionTypeComboBox);
    hLayout->addWidget(numsLabel);
    hLayout->addWidget(numOfQuestionSpinBox);
    hLayout->addWidget(degreeLabel);
    hLayout->addWidget(degreeOfQuestionType);
    hLayout->addWidget(addButton);

    layout->addLayout(hLayout);
    layout->addWidget(tableWidget);

    setLayout(layout);
}

void QuestionTypes::onNumOfQuestionChanged(int value)
{
    QString numOfQuestField = tr("%1_%2").arg(subjectName).arg(questionType);
    setField(numOfQuestField, value);
}

void QuestionTypes::onTableCellChanged(int curRow, int, int, int)
{
    questionType = questionTypeList.at(curRow);
    qDebug() << "questionType = " << questionType;
}

void QuestionTypes::onTableCellEnter(int curRow, int)
{
    questionType = questionTypeList.at(curRow);
    qDebug() << "questionType = " << questionType;
}

void QuestionTypes::onQuestionTypeChanged(QString questionType)
{
    QSqlQuery query;
    query.exec(tr("SELECT numOfQuestion FROM '%1' WHERE questionType == '%2'").arg(subjectName).arg(questionType));
    query.next();
    numOfQuestionSpinBox->setMaximum(query.value(0).toInt());
}

void QuestionTypes::printValue(int)
{
    foreach(QString type, questionTypeList) {
        qDebug() << tr("%1_%2 value=").arg(subjectName).arg(questionType) << field(tr("%1_%2").arg(subjectName).arg(questionType)).toString();
    }
}

void QuestionTypes::onAddClicked()
{
    QString typeName = questionTypeComboBox->currentText();
    int numOfType = numOfQuestionSpinBox->value();
    int degree = degreeOfQuestionType->value();

    tableWidget->setRowCount(tableWidget->rowCount());

    tableWidget->setItem(tableWidget->rowCount(), 0, new QTableWidgetItem(typeName));

    tableWidget->setItem(tableWidget->rowCount(), 1, new QTableWidgetItem(QString("%1").arg(numOfType)));

    tableWidget->setItem(tableWidget->rowCount(), 2, new QTableWidgetItem(QString("%1").arg(degree)));

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

    //    QTableWidget *tableWidget = (QTableWidget *) field("questionsTypeTables");

    //    subjectName = field("subjectName").toString();

    //    setTitle(tr("<h1>科目:%1 </h1>").arg(subjectName));

    //    setSubTitle(tr("<h2>请设置各个题目的知识点</h2>"));

    //    QSqlQuery query;

    //    query.exec(QString("SELECT questionTypes FROM '%1'").arg(subjectName));

    //    QStringList questionTypeList;
    //    while (query.next()) {
    //        questionTypeList.append(query.value(0).toString());
    //    }

    ////    foreach (QString questionType, questionTypeList) {

    ////    }

    //    QuestionNumCount = field(QString("%1_%2").arg(subjectName).arg(questionTypeList.at(TypeCount))).toInt();

    //    QVBoxLayout *layout = new QVBoxLayout;
    //    for (int i = 1; i < QuestionNumCount+1; ++i) {
    //        QHBoxLayout *hlayout = new QHBoxLayout();

    //        QLabel *label = new QLabel(tr("%1%2").arg(questionTypeList.at(TypeCount)).arg(i));

    //        QComboBox *pointCombo = new QComboBox;

    //        pointCombo->addItem(tr("任意知识点"));

    //        QStringList points = getPoints(subjectName, questionTypeList.at(TypeCount));

    //        foreach (QString point, points) {
    //            pointCombo->addItem(point);
    //        }
    //        hlayout->addWidget(label);
    //        hlayout->addWidget(pointCombo);

    //        layout->addLayout(hlayout);
    //    }

    //    setLayout(layout);
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

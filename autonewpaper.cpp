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
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

static QStandardItemModel *tableModel = NULL;
static QStandardItemModel *pointsModel = NULL;

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
        connect(subjectBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSubjectChanged(QString)));
    }
}

void SubjectSetup::onSubjectChanged(QString subjectName)
{
    setField("subjectName", subjectName);
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

    tableModel = new QStandardItemModel(questionTypeList.length(), 3);

    tableModel->setHorizontalHeaderLabels(QStringList() << tr("题目类型") << tr("题目数量") << tr("题型总分"));

    int row = 0;
    foreach (QString type, questionTypeList) {
        tableModel->setItem(row, 0, new QStandardItem(type));
        tableModel->item(row, 0)->setEditable(false);
        tableModel->setItem(row, 1, new QStandardItem("0"));
        tableModel->setItem(row, 2, new QStandardItem("0"));

        row++;
    }

    QTableView *view = new QTableView;

    view->setModel(tableModel);

    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(onCellClicked(QModelIndex)));

    if (typeLabel == NULL) {
        typeLabel = new QLabel(tr("题目类型:"));
    }

    if (questionTypeComboBox == NULL) {
        questionTypeComboBox = new QComboBox;
    } else {
        questionTypeComboBox->clear();
    }

    foreach(QString type, questionTypeList){
        questionTypeComboBox->addItem(type);
    }

    connect(questionTypeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onQuestionTypeChanged(QString)));

    query.exec(tr("SELECT numOfQuestions FROM '%1' WHERE questionTypes == '%2'").arg(subjectName).arg(questionTypeComboBox->currentText()));

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
        addButton = new QPushButton(tr("修改数量与分数"));
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

    layout->addWidget(view);

    layout->addLayout(hLayout);

    setLayout(layout);
}

void QuestionTypes::onCellClicked(QModelIndex index)
{
    int row =  index.row();
    questionTypeComboBox->setCurrentIndex(row);
    numOfQuestionSpinBox->setValue(tableModel->item(row, 1)->text().toInt());
    degreeOfQuestionType->setValue(tableModel->item(row, 2)->text().toInt());
}

void QuestionTypes::onQuestionTypeChanged(QString questionType)
{
    QSqlQuery query;

    query.exec(tr("SELECT numOfQuestions FROM '%1' WHERE questionTypes == '%2'").arg(subjectName).arg(questionType));
    query.next();
    numOfQuestionSpinBox->setMaximum(query.value(0).toInt());

    int row = questionTypeComboBox->currentIndex();

    if (row != -1) {
        numOfQuestionSpinBox->setValue(tableModel->item(row, 1)->text().toInt());
        degreeOfQuestionType->setValue(tableModel->item(row, 2)->text().toInt());
    }
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

    for (int row = 0; row < tableModel->rowCount(); ++row) {
        if (tableModel->item(row, 0)->text() == typeName) {
            tableModel->setItem(row, 1, new QStandardItem(QString("%1").arg(numOfType)));
            tableModel->setItem(row, 2, new QStandardItem(QString("%1").arg(degree)));
        }
    }
}

PointSetup::PointSetup(QWidget *parent)
    : QWizardPage(parent)
{
    TypeCount = 0;
    QuestionNumCount = 0;
    questionTypeList.clear();
    questionTypeComboBox  = NULL;
    numComboBox = NULL;
    pointComboBox = NULL;
    setButton = NULL;
}

void PointSetup::initializePage()
{
    if (this->layout()) {
        delete this->layout();
    }

    subjectName = field("subjectName").toString();

    setTitle(tr("<h1>科目:%1 </h1>").arg(subjectName));

    setSubTitle(tr("<h2>请设置各个题目的知识点和分数</h2>"));

    if (questionTypeComboBox == NULL) {
        questionTypeComboBox = new QComboBox;
    } else {
        questionTypeComboBox->clear();
    }

    QSqlQuery query;

    query.exec(tr("SELECT questionTypes FROM '%1'").arg(subjectName));

    while(query.next()) {
        questionTypeComboBox->addItem(query.value(0).toString());
    }

    questionTypeComboBox->setCurrentIndex(0);

    connect(questionTypeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onQuestionTypeChanged(QString)));

    questionType = questionTypeComboBox->currentText();

    pointsModel = new QStandardItemModel(tableModel->item(0,1)->text().toInt(), 3);

    pointsModel->setHorizontalHeaderLabels(QStringList() << tr("题号") << tr("知识点") << tr("分数"));

    for (int row = 0; row < tableModel->item(0,1)->text().toInt(); ++row) {
        pointsModel->setItem(row, 0, new QStandardItem(QString("%1").arg(row+1)));
        pointsModel->setItem(row, 1, new QStandardItem(tr("任意知识点")));
        pointsModel->setItem(row, 2, new QStandardItem(tr("根据总分值计算")));
    }

    QTableView *pointsView = new QTableView;

    pointsView->setModel(pointsModel);

    connect(pointsView, SIGNAL(clicked(QModelIndex)), this, SLOT(onPointsViewClicked(QModelIndex)));

    QLabel *numLabel = new QLabel(tr("题号:"));

    if (numComboBox == NULL) {
        numComboBox = new QComboBox;
    } else {
        numComboBox->clear();
    }

    for (int row = 0; row < tableModel->item(0,1)->text().toInt(); ++row) {
        numComboBox->addItem(QString("%1").arg(row+1));
    }

    QLabel *pointLabel = new QLabel(tr("知识点"));

    if (pointComboBox == NULL) {
        pointComboBox = new QComboBox;
    } else {
        pointComboBox->clear();
    }

    QStringList points = getPoints(subjectName, questionType);

    pointComboBox->addItem(tr("任意知识点"));
    foreach (QString point, points) {
        pointComboBox->addItem(point);
    }

    QLabel *degreeLabel = new QLabel(tr("分数"));

    QSpinBox *degreeSpinBox = new QSpinBox;

    degreeSpinBox->setMinimum(0);

    if (setButton == NULL) {
        setButton = new QPushButton(tr("设置该题"));
    }

    QHBoxLayout *hLayout = new QHBoxLayout;

    hLayout->addWidget(numLabel);
    hLayout->addWidget(numComboBox);
    hLayout->addWidget(pointLabel);
    hLayout->addWidget(pointComboBox);
    hLayout->addWidget(degreeLabel);
    hLayout->addWidget(degreeSpinBox);
    hLayout->addWidget(setButton);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(questionTypeComboBox);

    layout->addWidget(pointsView);

    layout->addLayout(hLayout);

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
        qDebug() << point;
    }
    return Points;
}

void PointSetup::onPointsViewClicked(QModelIndex index)
{
    int row = index.row();

    numComboBox->setCurrentIndex(row);

    //    pointComboBox->setCurrentIndex();
}

void PointSetup::onQuestionTypeChanged(QString type)
{

}

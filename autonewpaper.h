/**
*******************************************************************************
** Copyright (c) 2014
** All rights reserved.
**
** @file autonewpaper.h
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

#ifndef AUTONEWPAPER_H
#define AUTONEWPAPER_H

#include <QWizard>
#include <QString>
#include <QModelIndex>
#include <QMap>
#include <QVariantMap>
#include <QList>
#include <question.h>

class QTableView;
class QComboBox;
class QSpinBox;
class QTableWidget;
class QLabel;
class QPushButton;

class AutoNewPaper : public QWizard
{
    Q_OBJECT
public:
    AutoNewPaper(QWidget *parent = 0);

    void generatePaper();
    QList<Question> getQuestionList(QString subjectName, QString questionTypeName, int num);

    void getCommonDegree();

private slots:

    void onFinished(int i);

private:
    QVariantMap pointsMap;
    QVariantMap questionNumMap;
    QVariantMap commonDegree;

    int getPointsCnt(QString subjectName, QString questionTypeName);
};


class SubjectSetup : public QWizardPage
{
    Q_OBJECT
public:
    SubjectSetup(QWidget *parent=0);
private slots:
    void onSubjectChanged(QString subjectName);
};


class QuestionTypes : public QWizardPage
{
    Q_OBJECT
public:
    QuestionTypes(QWidget *parent = 0);

    void initializePage();

private slots:
    void onCellClicked(QModelIndex index);

    void onQuestionTypeChanged(QString questionType);

    void onAddClicked();

private:
    QLabel *typeLabel;
    QLabel *numsLabel;
    QLabel *degreeLabel;

    QComboBox *questionTypeComboBox;
    QSpinBox  *numOfQuestionSpinBox;
    QSpinBox  *degreeOfQuestionType;

    QStringList questionTypeList;
    QString questionType;
    QString subjectName;

    QPushButton *addButton;
};

class PointSetup : public QWizardPage
{
    Q_OBJECT
public:
    PointSetup(QWidget *parent = 0);

    void initializePage();

    QStringList getPoints(QString subjectName, QString questionTypeName);

    bool generatePaper();

private slots:
    void onPointsViewClicked(QModelIndex index);
    void onQuestionTypeChanged(int index);

    void onSetButtonClicked();

    void appPointModelList(int index);

private:
    int typeCount;
    int questionNumCount;
    QString subjectName;
    QString questionType;
    QStringList questionTypeList;
    QComboBox *questionTypeComboBox;
    QTableView * pointsView;
    QSpinBox *degreeSpinBox;
    QLabel *numLabel;
    QLabel *degreeLabel;
    QLabel *pointLabel;
    QLabel *diffLabel;

    QPushButton *setButton;
    QComboBox *numComboBox;
    QComboBox *pointComboBox;
    QComboBox *diffComboBox;

};

#endif // AUTONEWPAPER_H

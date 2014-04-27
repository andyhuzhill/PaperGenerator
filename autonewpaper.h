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
#include <QMap>
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
    void onNumOfQuestionChanged(int value);
    void onTableCellChanged(int curRow, int , int , int );
    void onTableCellEnter(int curRow, int);

    void onQuestionTypeChanged(QString questionType);
    void printValue(int);

    void onAddClicked();

private:
    QComboBox *questionTypeComboBox;
    QSpinBox  *numOfQuestionSpinBox;
    QSpinBox  *degreeOfQuestionType;
    QTableWidget *tableWidget;
    QStringList questionTypeList;
    QString questionType;
    QString subjectName;

    QLabel *typeLabel;
    QLabel *numsLabel;
    QLabel *degreeLabel;
    QPushButton *addButton;
};

class PointSetup : public QWizardPage
{
    Q_OBJECT
public:
    PointSetup(QWidget *parent = 0);

    void initializePage();

    QStringList getPoints(QString subjectName, QString questionTypeName);

private:
    int TypeCount;
    int QuestionNumCount;
    QString subjectName;
    QString questionType;
};

#endif // AUTONEWPAPER_H

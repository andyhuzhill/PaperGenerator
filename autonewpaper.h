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

class AutoNewPaper : public QWizard
{
    Q_OBJECT
public:
    AutoNewPaper(QWidget *parent = 0);

};


class SubjectSetup : public QWizardPage
{
public:
    SubjectSetup(QWidget *parent=0);
};


class QuestionTypes : public QWizardPage
{
public:
    QuestionTypes(QWidget *parent = 0);

    void initializePage();
};

class PointSetup : public QWizardPage
{
public:
    PointSetup(QWidget *parent = 0);

    void initializePage();

    QStringList getPoints(QString subjectName, QString questionTypeName);

private:
    int TypeCount;
    int QuestionNumCount;
};

#endif // AUTONEWPAPER_H

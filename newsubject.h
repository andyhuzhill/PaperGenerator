/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file newsubject.h
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

#ifndef NEWSUBJECT_H
#define NEWSUBJECT_H

#include <QWidget>
#include <QString>

namespace Ui {
class NewSubject;
}

class NewSubject : public QWidget
{
    Q_OBJECT

public:
    explicit NewSubject(QWidget *parent = 0);
    ~NewSubject();

signals:
    void dataChanged();

private slots:
    void on_newSubjectButton_clicked();

    void on_newTypeButton_clicked();

    void on_deleteSelectedSubject_clicked();

    void on_deleteSelectedType_clicked();

    void on_subjectsListsView_currentTextChanged(const QString &currentText);

    void questionTypeListRefresh(QString subject);

    void onDataChanged();

private:
    Ui::NewSubject *ui;


    void subjectListRefresh();

    QString questionLibraryPath;
};

#endif // NEWSUBJECT_H

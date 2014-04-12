/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file subjectmanager.h
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

#ifndef SUBJECTMANAGER_H
#define SUBJECTMANAGER_H

#include <QWidget>
#include "newsubject.h"
#include "newquestion.h"
#include "modifyquestion.h"

namespace Ui {
class SubjectManager;
}

class SubjectManager : public QWidget
{
    Q_OBJECT

public:
    explicit SubjectManager(QWidget *parent = 0);
    ~SubjectManager();

    void closeEvent(QCloseEvent *);

signals:
    void contentChanged();

private slots:

    void on_returnButton_clicked();

private:
    Ui::SubjectManager *ui;
};

#endif // SUBJECTMANAGER_H

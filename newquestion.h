/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file newquestion.h
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

#ifndef NEWQUESTION_H
#define NEWQUESTION_H

#include <QWidget>
#include <QString>

class QAxWidget;
class QPaintEvent;

namespace Ui {
class NewQuestion;
}

class NewQuestion : public QWidget
{
    Q_OBJECT

public:
    explicit NewQuestion(QWidget *parent = 0);
    ~NewQuestion();

    void extractImage(QString &html, QString QorA, QString outPath);

signals:
    void dataChanged();

private slots:
    void questionTypeListRefresh(QString subject);

    void on_inputQuestionButton_clicked();

    void on_inputAnswerButton_clicked();

    void on_inputButton_clicked();

    void on_chooseFile_clicked();

    void on_inputOne_clicked();

    void on_chooseInputDir_clicked();

    void on_inputMany_clicked();

    void onDataChanged();

private:
    Ui::NewQuestion *ui;
    QAxWidget *word;

    void subjectListRefresh();

    QString questionLibraryPath;

    void inputFileIsQuestion(bool isQuestion);
};

#endif // NEWQUESTION_H

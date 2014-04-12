/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file modifyquestion.h
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

#ifndef MODIFYQUESTION_H
#define MODIFYQUESTION_H

#include <QWidget>
#include <QString>

class QAxWidget;
class QPaintEvent;

namespace Ui {
class ModifyQuestion;
}

/** @enum 枚举 修改的是问题、答案或两者都有或其他 */
enum EnumModified{
    EMod_None,
    EMod_Question,
    EMod_Answer,
    EMod_QandA,
    EMod_Other
};

class ModifyQuestion : public QWidget
{
    Q_OBJECT

public:
    explicit ModifyQuestion(QWidget *parent = 0);
    ~ModifyQuestion();

    void extractImage(QString &html, QString QorA, QString outPath);

signals:
    void dataChanged();

private slots:
    void on_modifyQuestion_clicked();

    void on_modifyAnswer_clicked();

    void on_confirmChange_clicked();

    void on_deleteButton_clicked();

    void on_prevButton_clicked();

    void on_nextButton_clicked();

    void questionTypeListRefresh(QString subject);

    void onDataChanged();

    void modifyQA(QString QorA);

    void questionNumRefresh();

    void on_questionNumberCombox_valueChanged(int arg1);

private:
    Ui::ModifyQuestion *ui;

    void subjectListRefresh();

    QAxWidget *word;

    QString questionLibraryPath;


    EnumModified modifyWhich;    /**< 修改了问题、答案或者是两者都有被修改 */
};

#endif // MODIFYQUESTION_H

#ifndef NEWTESTFORM_H
#define NEWTESTFORM_H

#include <QWidget>
#include <QList>

#include "question.h"

namespace Ui {
class newTestForm;
}

class newTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit newTestForm(QWidget *parent = 0);
    ~newTestForm();

    void questionNumRefresh();

private slots:
    void on_exitButton_clicked();

    void on_SubjectNameCB_currentIndexChanged(const QString &arg1);

    void on_questionNum_valueChanged(int arg1);

    void on_questionTypeCB_currentIndexChanged(const QString &arg1);

    void on_chooseThisQuestion_clicked();

    void on_generatePaperButton_clicked();

    void onQuestionsrefresh();

    void on_deleteThisQuestion_clicked();

    void on_chooseMany_clicked();

    void on_tabWidget_currentChanged(QWidget *arg1);

    void on_alreadySelectQuestions_currentRowChanged(int currentRow);

    void on_autoChooseQuestions_clicked();

    void on_clearButton_clicked();

    void on_clearAlreadySelection_clicked();

private:
    Ui::newTestForm *ui;
    QList<Question> questions;
};

#endif // NEWTESTFORM_H

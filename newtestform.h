#ifndef NEWTESTFORM_H
#define NEWTESTFORM_H

#include <QWidget>
#include <QList>

#include "question.h"

class QAxObject;
class QAxWidget;
class QListWidgetItem;

namespace Ui {
class newTestForm;
}

class newTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit newTestForm(QWidget *parent = 0);
    ~newTestForm();

    void closeEvent(QCloseEvent *);

    void setPaperName(QString paperName);

    bool savePaper(QString paperName);

    bool checkSubjectQuestionType();

    QStringList getPoints(QString subjectName, QString questionTypeName);

    void setQuestionList(QList<Question> listOfQuestion);

    int getTotalGradeOfQuestionType(QString questionType);

signals:
    void contentChanged();

private slots:
    void questionNumRefresh();

    void on_exitButton_clicked();

    void on_SubjectNameCB_currentIndexChanged(const QString &arg1);

    void on_questionTypeCB_currentIndexChanged(const QString &arg1);

    void on_chooseThisQuestion_clicked();

    void on_generatePaperButton_clicked();

    void onQuestionsrefresh();

    void on_deleteThisQuestion_clicked();

    void on_tabWidget_currentChanged(QWidget *arg1);

    void on_clearAlreadySelection_clicked();

    void paperGenerate(QAxObject *docs, QList<Question> question, QStringList &itemList, QString outFileName);

    void on_alreadySelectQuestions_itemClicked(QListWidgetItem *item);

    void on_questionNumListWidget_itemClicked(QListWidgetItem *item);

    void on_questionNumListWidget_currentRowChanged(int currentRow);

    void on_saveQuestionButton_clicked();

    void on_autoChooseButton_clicked();

private:
    Ui::newTestForm *ui;
    QAxWidget *word;
    QList<Question> questions;
    QStringList points;
};

#endif // NEWTESTFORM_H

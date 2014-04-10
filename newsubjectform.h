#ifndef NEWSUBJECTFORM_H
#define NEWSUBJECTFORM_H

#include <QWidget>

class QAxWidget;

namespace Ui {
class newSubjectForm;
}
/** @enum 枚举 修改的是问题、答案或两者都有或其他 */
enum EnumModified{
    EMod_None,
    EMod_Question,
    EMod_Answer,
    EMod_QandA,
    EMod_Other
};

class newSubjectForm : public QWidget
{
    Q_OBJECT

public:
    explicit newSubjectForm(QWidget *parent = 0);
    ~newSubjectForm();

    void extractImage(QString &html, QString QorA, QString outPath);

    void closeEvent(QCloseEvent *);

signals:
    void contentChanged();

private slots:
    void on_newSubjectButton_clicked();

    void on_newTypeButton_clicked();


    void on_subjectsListsView_currentTextChanged(const QString &currentText);

    void on_questionTypeList_currentTextChanged(const QString &currentText);

    void on_deleteSelectedSubject_clicked();

    void on_deleteSelectedType_clicked();

    void on_subjectsCB_currentIndexChanged(const QString &arg1);

    void on_chooseFileName_clicked();

    void on_chooseInputFilePath_clicked();

    void on_startInput_clicked();

    void on_startInputMany_clicked();

    void on_subjectCB2_currentIndexChanged(const QString &arg1);

    void on_quesTypeCB2_currentIndexChanged(const QString &arg1);

    void on_changeButton_clicked();

    void on_questNumCB_valueChanged(int arg1);

    void on_deleteButton_clicked();

    void questionNumRefresh();

    void on_newQuestionButton_clicked();

    void on_modifyQuestion_clicked();

    void modifyQA(QString QorA);

    void on_modifyAnswer_clicked();

    void on_inputButton_clicked();

    void on_returnButton_clicked();

    void on_subjectsCB_activated(const QString &arg1);

    void on_subjectCB2_activated(const QString &arg1);

private:
    Ui::newSubjectForm *ui;

    void questionTypeListRefresh(QString subject);

    void subjectListRefresh();

    QAxWidget *word;

    EnumModified modifyWhich;    /**< 修改了问题、答案或者是两者都有被修改 */

    QString questionLibraryPath;
};

#endif // NEWSUBJECTFORM_H

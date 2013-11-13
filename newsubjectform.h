#ifndef NEWSUBJECTFORM_H
#define NEWSUBJECTFORM_H

#include <QWidget>

class QAxWidget;

namespace Ui {
class newSubjectForm;
}

class newSubjectForm : public QWidget
{
    Q_OBJECT

public:
    explicit newSubjectForm(QWidget *parent = 0);
    ~newSubjectForm();


private slots:
    void on_newSubjectButton_clicked();

    void on_newTypeButton_clicked();

    void on_SubjectNameEdit_textChanged(const QString &arg1);

    void on_subjectsListsView_currentTextChanged(const QString &currentText);

    void on_questionTypeList_currentTextChanged(const QString &currentText);

    void on_deleteSelectedSubject_clicked();

    void on_deleteSelectedType_clicked();

    void on_subjectsCB_currentIndexChanged(const QString &arg1);

    void on_chooseFileName_clicked();

    void on_chooseInputFilePath_clicked();

    void on_startInput_clicked();

    void on_startInputMany_clicked();

private:
    Ui::newSubjectForm *ui;

    void questionTypeListRefresh(QString subject);

    void subjectListRefresh();

    QAxWidget *word;

    QString questionLibraryPath;
};

#endif // NEWSUBJECTFORM_H

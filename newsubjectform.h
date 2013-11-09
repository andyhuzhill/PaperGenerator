#ifndef NEWSUBJECTFORM_H
#define NEWSUBJECTFORM_H

#include <QWidget>

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

private:
    Ui::newSubjectForm *ui;

    void questionTypeListRefresh();
};

#endif // NEWSUBJECTFORM_H

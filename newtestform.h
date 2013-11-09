#ifndef NEWTESTFORM_H
#define NEWTESTFORM_H

#include <QWidget>

namespace Ui {
class newTestForm;
}

class newTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit newTestForm(QWidget *parent = 0);
    ~newTestForm();

private slots:
    void on_exitButton_clicked();

    void on_SubjectNameCB_currentIndexChanged(const QString &arg1);

private:
    Ui::newTestForm *ui;
};

#endif // NEWTESTFORM_H

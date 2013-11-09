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
    
private:
    Ui::newSubjectForm *ui;
};

#endif // NEWSUBJECTFORM_H

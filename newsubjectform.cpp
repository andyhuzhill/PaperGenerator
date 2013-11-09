#include "newsubjectform.h"
#include "ui_newsubjectform.h"

newSubjectForm::newSubjectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newSubjectForm)
{
    ui->setupUi(this);

    setWindowTitle(tr("新建一个试题库"));
}

newSubjectForm::~newSubjectForm()
{
    delete ui;
}

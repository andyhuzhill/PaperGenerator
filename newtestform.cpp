#include "newtestform.h"
#include "ui_newtestform.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QStringList>

newTestForm::newTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newTestForm)
{
    ui->setupUi(this);

    setWindowTitle(tr("新建一份试卷"));

    QSqlQuery query;

    query.exec("SELECT * FROM subjects");

    while (query.next()) {
        ui->SubjectNameCB->addItem(query.value(query.record().indexOf("subjectName")).toString());
    }

}

newTestForm::~newTestForm()
{
    delete ui;
}

void newTestForm::on_exitButton_clicked()
{
    close();
}

void newTestForm::on_SubjectNameCB_currentIndexChanged(const QString &arg1)
{
    QSqlQuery query;

    query.exec(QString("SELECT questype FROM subject WHERE name = '%1'").arg(arg1));

    qDebug() << QString(tr("SELECT questype FROM subject WHERE name = %1")).arg(arg1);

    while (query.next()) {
        QStringList list = query.value(0).toString().split(",", QString::SkipEmptyParts);
        ui->listWidget->clear();
        foreach (QString str, list) {
            ui->listWidget->addItem(str);
        }

    }
}

#include "newsubjectform.h"
#include "ui_newsubjectform.h"

#include <QMessageBox>
#include <QString>
#include <QSqlQuery>
#include <QDir>

#include <QDebug>

const QString databasePath = "C:/out/";

newSubjectForm::newSubjectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newSubjectForm)
{
    ui->setupUi(this);

    setWindowTitle(tr("管理试题库"));
}

newSubjectForm::~newSubjectForm()
{
    delete ui;
}

void newSubjectForm::on_newSubjectButton_clicked()
{
    int i = 0;
    QString subjectName = ui->SubjectNameEdit->text().trimmed();
    if (subjectName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("课程名称不能为空！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;

    query.exec("SELECT SubjectName FROM subjects");

    while (query.next()) {
        if (query.value(0).toString() == subjectName) {
            QMessageBox::warning(this, tr("警告"), tr("数据库中已存在课程《%1》,请重新输入一个课程名！").arg(subjectName), QMessageBox::Ok);
            return ;
        }
        i++;
    }

    QString createSubjectCmd, insertSubjectCmd;
    createSubjectCmd = QString("CREATE TABLE %1("
                               "id integer ,"
                               "questionTypes,"
                               "numOfQuestions integer)"
                               ).arg(subjectName);
    qDebug() << "createSubjectcmd: \n" << createSubjectCmd;
    insertSubjectCmd = QString("INSERT INTO subjects VALUES("
                               "%1,"
                               "\"%2\","
                               "0)"
                               ).arg(i+1).arg(subjectName);
    qDebug() << "insertSubjectCmd:\n" << insertSubjectCmd;
    if (query.exec(createSubjectCmd) && query.exec(insertSubjectCmd)) {
        QMessageBox::information(this, tr("通知"), tr("课程数据库创建成功！"), QMessageBox::Ok);
        QDir dir(databasePath);
        dir.mkdir(subjectName);
    }else{
        QMessageBox::information(this, tr("警告"), tr("课程数据库创建失败！"), QMessageBox::Ok);
    }

}

void newSubjectForm::on_newTypeButton_clicked()
{
    QString questionType = ui->questionTypeEdit->text().trimmed();

    if (questionType.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("题目类型不得为空！"), QMessageBox::Ok);
        return ;
    }
    QString subjectName = ui->SubjectNameEdit->text().trimmed();
    if (subjectName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("课程名称不能为空！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;

    query.exec(QString("SELECT questionTypes FROM %1").arg(subjectName));

    int i = 0;
    while(query.next()){
        if (query.value(0).toString() == questionType) {
            QMessageBox::warning(this, tr("警告"), tr("该课程中已经包含“%1”类型的题目！").arg(questionType), QMessageBox::Ok);
        return ;
        }
        i++;
        qDebug() << query.value(0).toString();
    }

    QString insertQuestionTypeCmd;
    insertQuestionTypeCmd = QString("INSERT INTO %1 VALUES("
                                    "%2,"
                                    "\"%3\","
                                    "%4)"
                                    ).arg(subjectName).arg(i+1).arg(questionType).arg(0);

    qDebug() << insertQuestionTypeCmd;

    if (query.exec(insertQuestionTypeCmd)) {
        QMessageBox::information(this, tr("通知"), tr("题目类型插入完毕。"), QMessageBox::Ok);
        questionTypeListRefresh();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("题目类型插入失败！"), QMessageBox::Ok);
    }

}

void newSubjectForm::on_SubjectNameEdit_textChanged(const QString &arg1)
{
    questionTypeListRefresh();
//    QSqlQuery query;
//    QString subject = arg1.trimmed();

//    query.exec("SELECT subjectName FROM subjects");

//    while (query.next()) {
//        if (query.value(0).toString() == subject) {
//            break;
//        }
//    }

//    if (query.value(0).toString() == subject) {
//        query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(subject));
//        while (query.next()) {
//            ui->questionTypeList->addItem(query.value(0).toString());
//        }
//    }
}

void newSubjectForm::questionTypeListRefresh()
{
    QSqlQuery query;
    QString subject = ui->SubjectNameEdit->text().trimmed();

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        if (query.value(0).toString() == subject) {
            break;
        }
    }

    ui->questionTypeList->clear();
    if (query.value(0).toString() == subject) {
        query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(subject));
        while (query.next()) {
            ui->questionTypeList->addItem(query.value(0).toString());
        }
    }
}

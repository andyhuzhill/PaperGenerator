/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file newsubject.cpp
** @brief
**
** @version 1.0
** @author 胡震宇 <andyhuzhill@gmail.com>
**
** @date 2014/4/12
**
** @revision： 最初版本
*******************************************************************************
*/

#include "newsubject.h"
#include "ui_newsubject.h"
#include "defs.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QListWidgetItem>
#include <QDir>
#include <QDebug>
#include <QSettings>

NewSubject::NewSubject(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewSubject)
{
    ui->setupUi(this);

    QSqlQuery query;
    query.exec("SELECT subjectName FROM subjects");
    ui->subjectsListsView->clear();
    while (query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());

        ui->subjectsListsView->addItem(item);
    }

    connect(ui->subjectsListsView, SIGNAL(currentTextChanged(QString)), this, SLOT(questionTypeListRefresh(QString)));

    QSettings settings(OrgName, AppName);

    questionLibraryPath = settings.value("paperPath").toString();
    questionLibraryPath.replace("\\","/");
}

NewSubject::~NewSubject()
{
    delete ui;
}

void NewSubject::on_newSubjectButton_clicked()
{
    bool ok;

    QString subjectName = QInputDialog::getText(this, tr("请输入课程名称"), tr(""), QLineEdit::Normal, "", &ok);
    if(!ok){
        return ;
    }
    if (subjectName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("课程名称不能为空！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;

    query.exec("SELECT SubjectName FROM subjects");

    int i = 0;
    while (query.next()) {
        if (query.value(0).toString() == subjectName) {
            QMessageBox::warning(this, tr("警告"), tr("数据库中已存在课程《%1》,请重新输入一个课程名！").arg(subjectName), QMessageBox::Ok);
            return ;
        }
        qDebug() << query.value(0).toString();
        i++;
    }

    QString createSubjectCmd, insertSubjectCmd;
    createSubjectCmd = QString("CREATE TABLE %1("
                               "id integer ,"
                               "questionTypes,"
                               "numOfQuestions integer)"
                               ).arg(subjectName);
    insertSubjectCmd = QString("INSERT INTO subjects VALUES("
                               "%1,"
                               "\"%2\","
                               "0)"
                               ).arg(i+1).arg(subjectName);
    bool createSubjectResult = query.exec(createSubjectCmd);
    bool insertSubjectResult = query.exec(insertSubjectCmd);
    qDebug() << "creaet:" << createSubjectResult ;
    qDebug() << "insert:" << insertSubjectResult;

    if (createSubjectResult && insertSubjectResult) {
        QMessageBox::information(this, tr("通知"), tr("课程数据库创建成功！"), QMessageBox::Ok);
        QDir dir(questionLibraryPath);
        dir.mkdir(subjectName);
        subjectListRefresh();

        ui->questionTypesLabel->setText(tr("课程[%1]题型列表：").arg(subjectName));
        questionTypeListRefresh(subjectName);
        emit dataChanged();

    }else{
        QMessageBox::information(this, tr("警告"), tr("课程数据库创建失败！"), QMessageBox::Ok);
    }
}

void NewSubject::on_newTypeButton_clicked()
{
    bool ok;
    QString questionType = QInputDialog::getText(this, tr("请输入题目类型名称："), tr(""), QLineEdit::Normal, "", &ok);

    if (!ok) {
        return ;
    }
    if (questionType.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("题目类型不得为空！"), QMessageBox::Ok);
        return ;
    }

    QString questionLabel = ui->questionTypesLabel->text();
    QString  subjectName = questionLabel.mid(questionLabel.indexOf("[")+1, questionLabel.indexOf("]")-questionLabel.indexOf("[")-1);

    if (subjectName.isNull()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选中课程名！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;

    query.exec(QString("SELECT questionTypes FROM '%1'").arg(subjectName));

    int i = 0;
    while(query.next()){
        if (query.value(0).toString() == questionType) {
            QMessageBox::warning(this, tr("警告"), tr("该课程中已经包含“%1”类型的题目！").arg(questionType), QMessageBox::Ok);
            return ;
        }
        i++;
    }

    QString insertQuestionTypeCmd;
    insertQuestionTypeCmd = QString("INSERT INTO '%1' VALUES("
                                    "%2,"
                                    "'%3',"
                                    "%4)"
                                    )
            .arg(subjectName)
            .arg(i+1)
            .arg(questionType)
            .arg(0);

    QString insertQuestionTypeTableCmd;
    insertQuestionTypeTableCmd = QString("CREATE TABLE '%1_%2'("
                                         "id integer,"
                                         "Question,"
                                         "Answer,"
                                         "QuestionDocPath,"
                                         "AnswerDocPath,"
                                         "Point,"
                                         "Difficulty,"
                                         "degrade)").arg(subjectName).arg(questionType);

    bool insertQusTypeResult = query.exec(insertQuestionTypeCmd);
    bool insertQusTypeIntoTableResult = query.exec(insertQuestionTypeTableCmd);

    if (insertQusTypeResult && insertQusTypeIntoTableResult) {

        QDir dir(questionLibraryPath);
        dir.cd(subjectName);
        dir.mkdir(questionType);

        QMessageBox::information(this, tr("通知"), tr("题目类型插入完毕。"), QMessageBox::Ok);
        questionTypeListRefresh(subjectName);

        emit dataChanged();

    }else{
        QMessageBox::warning(this, tr("警告"), tr("题目类型插入失败！"), QMessageBox::Ok);
    }
}

void NewSubject::on_deleteSelectedSubject_clicked()
{
    if (QMessageBox::Yes == QMessageBox::warning(this, tr("警告"), tr("您确定要删除选定的科目？"), QMessageBox::Yes, QMessageBox::No)) {
        QList<QListWidgetItem*> selectedSubjects = ui->subjectsListsView->selectedItems();

        if (selectedSubjects.length() == 0) {
            QMessageBox::warning(this, tr("警告"), tr("没有选中要删除的课程名，请先选中"),QMessageBox::Ok);
            return ;
        }
        QSqlQuery query;

        foreach (QListWidgetItem *item, selectedSubjects) {
            if(!query.exec(QString("SELECT questionTypes FROM '%1'").arg(item->text()))){
                QMessageBox::warning(this, tr("警告"), tr("删除《%1》失败！").arg(item->text()), QMessageBox::Ok);
                return ;
            }
            QStringList types;

            while (query.next()) {
                types.append(query.value(0).toString());
            }

            foreach (QString str, types) {
                if(!query.exec(QString("DROP TABLE '%1_%2'").arg(item->text()).arg(str))){
                    QMessageBox::warning(this, tr("警告"), tr("删除《%1》%2题型失败！").arg(item->text()).arg(str), QMessageBox::Ok);
                    return;
                }
            }

            bool status1 = query.exec(QString("DELETE FROM subjects where subjectName = '%1'").arg(item->text()));

            bool status2 = query.exec(QString("DROP TABLE '%1'").arg(item->text()));

            if (!status1 || !status2) {
                QMessageBox::warning(this, tr("警告"), tr("课程《%1》删除失败").arg(item->text()), QMessageBox::Ok);
                subjectListRefresh();
                ui->questionTypeList->clear();
                return ;
            }
        }

        subjectListRefresh();

        emit dataChanged();

        QMessageBox::information(this, tr("通知"), tr("选择的科目已经删除"), QMessageBox::Ok);

    }
}

void NewSubject::on_deleteSelectedType_clicked()
{
    if (QMessageBox::Yes == QMessageBox::warning(this, tr("警告"), tr("您确定要删除选定的题目类型？"), QMessageBox::Yes, QMessageBox::No)) {

        QList<QListWidgetItem*> selectedQuestionTypes = ui->questionTypeList->selectedItems();

        if (selectedQuestionTypes.length() == 0) {
            QMessageBox::warning(this, tr("警告"), tr("没有选中要删除的题目类型，请先选中"),QMessageBox::Ok);
            return ;
        }

        QString labelQuestionType = ui->questionTypesLabel->text();
        QString subjectName = labelQuestionType.mid(labelQuestionType.indexOf("[")+1, labelQuestionType.indexOf("]")-labelQuestionType.indexOf("[")-1);
        if (subjectName.isEmpty()) {
            QMessageBox::warning(this, tr("警告"), tr("请先选中课程名称!"), QMessageBox::Ok);
            return ;
        }

        QSqlQuery query;

        foreach (QListWidgetItem *item, selectedQuestionTypes) {
            bool deleteQuestionTypeResult = query.exec(QString("DELETE FROM '%1' where questionTypes = '%2'").arg( subjectName ).arg(item->text()));
            bool dropTableResult = query.exec(QString("DROP TABLE '%1_%2'").arg(subjectName).arg(item->text()));

            if (!deleteQuestionTypeResult && !dropTableResult) {
                QMessageBox::warning(this, tr("警告"), tr("题目类型删除失败！"),QMessageBox::Ok);
                questionTypeListRefresh(subjectName);
                return ;
            }
        }

        QMessageBox::information(this, tr("信息"), tr("题目类型删除成功！"),QMessageBox::Ok);
        questionTypeListRefresh(subjectName);

        emit dataChanged();
    }
}

void NewSubject::questionTypeListRefresh(QString subject)
{
    QSqlQuery query;

    bool bOk = query.exec("SELECT subjectName FROM subjects");

    if (bOk) {
        while (query.next()) {
            if (query.value(0).toString() == subject) {
                break;
            }
        }

        ui->questionTypeList->clear();

        if (query.value(0).toString() == subject) {
            query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(subject));
            while (query.next()) {
                QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());
                ui->questionTypeList->addItem(item);
            }
        }
    }
}

void NewSubject::onDataChanged()
{
    QSqlQuery query;
    query.exec("SELECT subjectName FROM subjects");
    ui->subjectsListsView->clear();
    while (query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());

        ui->subjectsListsView->addItem(item);
    }
}

void NewSubject::subjectListRefresh()
{
    QSqlQuery query;
    query.exec("SELECT subjectName from subjects");

    ui->subjectsListsView->clear();
    while (query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());

        ui->subjectsListsView->addItem(item);
    }
}

void NewSubject::on_subjectsListsView_currentTextChanged(const QString &currentText)
{
    questionTypeListRefresh(currentText);
    ui->questionTypesLabel->setText(tr("课程[%1]题型列表：").arg(currentText));
}

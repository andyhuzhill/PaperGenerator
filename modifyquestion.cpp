/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file modifyquestion.cpp
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

#include "modifyquestion.h"
#include "ui_modifyquestion.h"
#include "defs.h"

#include <QMessageBox>
#include <QFile>
#include <QAxObject>
#include <QDir>
#include <QSettings>
#include <QSqlQuery>
#include <QClipboard>
#include <QAxWidget>
#include <QMimeData>

ModifyQuestion::ModifyQuestion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModifyQuestion)
{
    ui->setupUi(this);

    subjectListRefresh();
    questionTypeListRefresh(ui->subjectCombox->currentText());

    connect(ui->subjectCombox, SIGNAL(currentIndexChanged(QString)), this, SLOT(questionTypeListRefresh(QString)));

    QSettings settings(OrgName, AppName);

    questionLibraryPath = settings.value("paperPath").toString();
    questionLibraryPath.replace("\\","/");
}

ModifyQuestion::~ModifyQuestion()
{
    delete ui;
}

void ModifyQuestion::extractImage(QString &html, QString QorA, QString outPath)
{
    html.replace(QString("\\"), QString("/"));
    html.remove("file:///");

    QList<int> start;
    QList<int> end;

    int startidx = 0;
    int endidx = 0;

    /** 首先记录下每个公式 标签的开始和结束 */
    while((startidx = html.indexOf("<!--[if gte vml 1]>",startidx)) != -1){
        start.prepend(startidx);
        startidx ++;
        if((endidx = html.indexOf("<![endif]-->", startidx)) != -1){
            end.prepend(endidx);
        }
    }

    /** 然后删除公式 */
    for (int i = 0; i < start.length(); ++i) {
        html.remove(start.at(i), end.at(i)-start.at(i)+12);
    }

    start.clear();
    startidx = 0;

    /** 记录下图片的开始标签位置 */
    while((startidx = html.indexOf("<img", startidx)) != -1){
        start.prepend(startidx);
        startidx ++;
    }

    /** 将图片的原地址替换为新地址，并将图片保存到新地址 */
    for (int i = 0; i < start.length(); ++i) {
        int srcStart = html.indexOf("src=\"", start.at(i))+5;
        int srcEnd = html.indexOf("\"",srcStart);
        srcEnd = html.indexOf("\"", srcEnd);

        QString imgOriginalPath = html.mid(srcStart,srcEnd-srcStart);

        QFile imgfile(imgOriginalPath);
        QFileInfo imgFileInfo(imgOriginalPath);
        QString imgFinalPath = QString("%1/%2%3.%4").arg(outPath).arg(QorA).arg(i).arg(imgFileInfo.completeSuffix());
        imgfile.copy(imgFinalPath);

        html.replace(imgOriginalPath, imgFinalPath);
    }
}

void ModifyQuestion::on_modifyQuestion_clicked()
{
    modifyQA("Question");
}

void ModifyQuestion::on_modifyAnswer_clicked()
{
    modifyQA("Answer");
}

void ModifyQuestion::on_confirmChange_clicked()
{
    QSqlQuery query;

    QString subjectName = ui->subjectCombox->currentText();
    QString questionTypeName = ui->questionTypeCombox->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    if (modifyWhich!= EMod_None) {
        word = new QAxWidget(wordAppName);
        if (!word) {
            QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
            return ;
        }
        word->setProperty("Visible", false);    //隐藏word程序
        word->setProperty("DisplayAlerts", true);
        QAxObject *docs = word->querySubObject("Documents");
        if (!docs) {
            QMessageBox::warning(this, tr("警告"), tr("无法获得Documents对象"), QMessageBox::Ok);
            word->dynamicCall("Quit(boolean)", true);
            delete word;
            return;
        }

        QClipboard *clip = QApplication::clipboard();
        const QMimeData *dat;
        QAxObject *active_doc;
        switch(modifyWhich){
        case EMod_Question:
            query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3")
                       .arg(subjectName)
                       .arg(questionTypeName)
                       .arg(ui->questionNumberCombox->value()));
            query.next();
            docs->dynamicCall("Open(QString)", query.value(0).toString());
            active_doc = word->querySubObject("ActiveDocument");
            if (!active_doc) {
                QMessageBox::warning(this, tr("警告"), tr("无法获得该文档！"), QMessageBox::Ok);
                word->dynamicCall("Quit(Boolean)", true);
                delete word;
                return;
            }
            active_doc->dynamicCall("Select()");
            active_doc->querySubObject("Range()")->dynamicCall("Copy()");

            dat = clip->mimeData();

            if (dat->hasHtml()) {
                QString html = dat->html();
                query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3")
                           .arg(subjectName)
                           .arg(questionTypeName)
                           .arg(ui->questionNumberCombox->value()));
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Question", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Question = \"%3\" WHERE id = %4")
                           .arg(subjectName)
                           .arg(questionTypeName)
                           .arg(html)
                           .arg(ui->questionNumberCombox->value()));
            }
            clip->clear();
            break;
        case EMod_Answer:
            query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3")
                       .arg(subjectName)
                       .arg(questionTypeName)
                       .arg(ui->questionNumberCombox->value()));
            query.next();
            docs->dynamicCall("Open(QString)", query.value(0).toString());
            active_doc = word->querySubObject("ActiveDocument");
            if (!active_doc) {
                QMessageBox::warning(this, tr("警告"), tr("无法获得该文档！"), QMessageBox::Ok);
                word->dynamicCall("Quit(boolean)", true);
                delete word;
                return;
            }
            active_doc->dynamicCall("Select()");
            active_doc->querySubObject("Range()")->dynamicCall("Copy()");

            dat = clip->mimeData();

            if (dat->hasHtml()) {
                QString html = dat->html();
                query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));
                query.next();
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Answer", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Answer = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questionNumberCombox->value()));
            }
            clip->clear();
            break;
        default:
            query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));
            query.next();
            docs->dynamicCall("Open(QString)", query.value(0).toString());
            active_doc = word->querySubObject("ActiveDocument");
            if (!active_doc) {
                QMessageBox::warning(this, tr("警告"), tr("无法获得该文档！"), QMessageBox::Ok);
                word->dynamicCall("Quit(Boolean)", true);
                delete word;
                return;
            }
            active_doc->dynamicCall("Select()");
            active_doc->querySubObject("Range()")->dynamicCall("Copy()");

            dat = clip->mimeData();

            if (dat->hasHtml()) {
                QString html = dat->html();
                query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Question", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Question = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questionNumberCombox->value()));
            }
            clip->clear();

            query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));
            query.next();
            docs->dynamicCall("Open(QString)", query.value(0).toString());
            active_doc = word->querySubObject("ActiveDocument");
            if (!active_doc) {
                QMessageBox::warning(this, tr("警告"), tr("无法获得该文档！"), QMessageBox::Ok);
                word->dynamicCall("Quit(Boolean)", true);
                delete word;
                return;
            }
            active_doc->dynamicCall("Select()");
            active_doc->querySubObject("Range()")->dynamicCall("Copy()");

            dat = clip->mimeData();

            if (dat->hasHtml()) {
                QString html = dat->html();
                query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));
                query.next();
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Answer", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Answer = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questionNumberCombox->value()));
            }
            clip->clear();
            break;
        }
        word->dynamicCall("Quit(boolean)", true);
        delete word;
    }

    QString strDifficulty = QString("%1").arg(ui->diffSP->value());
    QString strPoint = ui->pointEdit->text().trimmed();

    bool updatePointResult = query.exec(QString("UPDATE '%1_%2' SET Point = '%3' WHERE id = %4")
                                        .arg(subjectName)
                                        .arg(questionTypeName)
                                        .arg(strPoint)
                                        .arg(ui->questionNumberCombox->value())
                                        );
    bool updateDifficultyResult = query.exec(QString("UPDATE '%1_%2' SET Difficulty = '%3' WHERE id = %4")
                                             .arg(subjectName)
                                             .arg(questionTypeName)
                                             .arg(strDifficulty)
                                             .arg(ui->questionNumberCombox->value())
                                             );

    if ( updatePointResult && updateDifficultyResult) {
        QMessageBox::information(this, tr("通知"), tr("修改试题成功！"), QMessageBox::Ok);

        on_questionNumberCombox_valueChanged(0);

        modifyWhich = EMod_None;
    }else{
        QMessageBox::warning(this, tr("警告"), tr("修改试题失败！"), QMessageBox::Ok);
    }
}

void ModifyQuestion::on_deleteButton_clicked()
{

    if (QMessageBox::Yes == QMessageBox::warning(this, tr("警告"), tr("您确定要删除该题目？"), QMessageBox::Yes, QMessageBox::No)) {
        QString subjectName = ui->subjectCombox->currentText();
        QString questionTypeName = ui->questionTypeCombox->currentText();

        if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
            QMessageBox::warning(this, tr("警告"), tr("请先选择课程和题型！"), QMessageBox::Ok);
            return ;
        }

        QSqlQuery query;

        bool status1 = query.exec(QString("DELETE FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));

        if(status1){
            QMessageBox::information(this, tr("通知"), tr("试题删除成功！"), QMessageBox::Ok);
            ui->questionBrowser->clear();
            ui->answerBrowser->clear();
            ui->diffSP->clear();
            ui->pointEdit->clear();
            on_questionNumberCombox_valueChanged(0);
        }else{
            QMessageBox::warning(this, tr("警告"), tr("试题删除失败！"), QMessageBox::Ok);
        }
    }
}

void ModifyQuestion::on_prevButton_clicked()
{
    if (ui->questionNumberCombox->value() > ui->questionNumberCombox->minimum()){
        ui->questionNumberCombox->setValue(ui->questionNumberCombox->value() - 1);
    }
}

void ModifyQuestion::on_nextButton_clicked()
{
    if (ui->questionNumberCombox->value() < ui->questionNumberCombox->maximum()) {
        ui->questionNumberCombox->setValue(ui->questionNumberCombox->value() + 1);
    }
}

void ModifyQuestion::questionTypeListRefresh(QString subject)
{
    QSqlQuery query;

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        if (query.value(0).toString() == subject) {
            break;
        }
    }

    ui->questionTypeCombox->clear();

    if (query.value(0).toString() == subject) {
        query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(subject));
        while (query.next()) {
            ui->questionTypeCombox->addItem(query.value(0).toString());
        }
    }

    questionNumRefresh();
}

void ModifyQuestion::onDataChanged()
{
    subjectListRefresh();
    questionTypeListRefresh(ui->subjectCombox->currentText());
}

void ModifyQuestion::modifyQA(QString QorA)
{
    word = new QAxWidget(wordAppName);
    word->setControl(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);    //隐藏word程序
    word->setProperty("DisplayAlerts", true);
    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获取Documents对象！"), QMessageBox::Ok);
        return;
    }

    QString subjectName = ui->subjectCombox->currentText();
    QString questionTypeName = ui->questionTypeCombox->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;
    if (QorA == "Question") {
        query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3")
                   .arg(subjectName)
                   .arg(questionTypeName)
                   .arg(ui->questionNumberCombox->value())
                   );
        if (modifyWhich == EMod_Answer) {
            modifyWhich = EMod_QandA;
        }else{
            modifyWhich = EMod_Question;
        }
    }else{
        query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questionNumberCombox->value()));
        if (modifyWhich == EMod_Question) {
            modifyWhich = EMod_QandA;
        }else{
            modifyWhich = EMod_Answer;
        }
    }
    query.next();
    QString docPath = query.value(0).toString();
    docs->dynamicCall("Open(QString)", docPath);
    word->setProperty("Visible", true);
}

void ModifyQuestion::questionNumRefresh()
{
    QSqlQuery query;

    if(query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'")
                  .arg(ui->subjectCombox->currentText())
                  .arg(ui->questionTypeCombox->currentText()))
            ){
        query.next();
        int maxNum = query.value(0).toInt();
        ui->questionNumberCombox->setMaximum(maxNum);
        ui->numOfQuestiongLabel->setText(tr("共 %1 题").arg(maxNum));
    }
}

void ModifyQuestion::subjectListRefresh()
{
    QSqlQuery query;
    query.exec("SELECT subjectName from subjects");

    ui->subjectCombox->clear();

    while (query.next()) {
        ui->subjectCombox->addItem(query.value(0).toString());
    }

}

void ModifyQuestion::on_questionNumberCombox_valueChanged(int arg1)
{
    (void)arg1;
    QString subjectName = ui->subjectCombox->currentText();
    QString questionTypeName = ui->questionTypeCombox->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        ui->answerBrowser->clear();
        ui->questionBrowser->clear();
        ui->diffSP->clear();
        ui->pointEdit->clear();
        return ;
    }

    QSqlQuery query;

    query.exec(QString("SELECT * FROM '%1_%2' WHERE id = %3")
               .arg(subjectName)
               .arg(questionTypeName)
               .arg(ui->questionNumberCombox->value()));
    query.next();
    QString Question = query.value(1).toString();
    QString Answer = query.value(2).toString();
    QString Point = query.value(5).toString();
    int Difficulty = query.value(6).toInt();

    ui->questionBrowser->clear();
    ui->questionBrowser->insertHtml(Question);
    ui->answerBrowser->clear();
    ui->answerBrowser->insertHtml(Answer);
    ui->pointEdit->setText(Point);
    ui->diffSP->setValue(Difficulty);
}


void ModifyQuestion::on_questionTypeCombox_currentIndexChanged(int index)
{
    on_questionNumberCombox_valueChanged(index);
}

void ModifyQuestion::on_subjectCombox_currentIndexChanged(int index)
{
    on_questionNumberCombox_valueChanged(index);
}

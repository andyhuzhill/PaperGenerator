/**
*******************************************************************************
** Copyright (c) 2014,
** All rights reserved.
**
** @file newquestion.cpp
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

#include "newquestion.h"
#include "ui_newquestion.h"

#include "defs.h"
#include "docreadwriter.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QSettings>
#include <QClipboard>
#include <QMimeData>

#include <QAxObject>

NewQuestion::NewQuestion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewQuestion)
{
    ui->setupUi(this);
    ui->inputManyProgressBar->hide();
    ui->inputManyProgressLabel->hide();

    subjectListRefresh();
    questionTypeListRefresh(ui->subjectCombox->currentText());

    connect(ui->subjectCombox, SIGNAL(currentIndexChanged(QString)), this, SLOT(questionTypeListRefresh(QString)));

    QSettings settings(OrgName, AppName);

    questionLibraryPath = settings.value("paperPath").toString();
    questionLibraryPath.replace("\\","/");

}

NewQuestion::~NewQuestion()
{
    delete ui;
}

void NewQuestion::extractImage(QString &html, QString QorA, QString saveToPath)
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
        QString imgFinalPath = QString("%1/%2%3.%4").arg(saveToPath).arg(QorA).arg(i).arg(imgFileInfo.completeSuffix());
        imgfile.copy(imgFinalPath);

        html.replace(imgOriginalPath, imgFinalPath);
    }
}

void NewQuestion::questionTypeListRefresh(QString subject)
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

    emit dataChanged();
}

void NewQuestion::subjectListRefresh()
{
    QSqlQuery query;
    query.exec("SELECT subjectName from subjects");

    ui->subjectCombox->clear();

    while (query.next()) {
        ui->subjectCombox->addItem(query.value(0).toString());
    }
}

void NewQuestion::inputFileIsQuestion(bool isQuestion)
{
    word = new QAxWidget(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }

    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("获取当前打开文档失败！"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return ;
    }

    QDir dir(".");
    QFile templa(QString("%1/Normal.dot").arg(dir.currentPath()));

    if (isQuestion) {
        templa.copy(QString("%1/OneQuestion.doc").arg(dir.currentPath()));

        docs->dynamicCall("Open(QString)",QString("%1/OneQuestion.doc").arg(dir.currentPath()));
    }else{
        templa.copy(QString("%1/TheAnswer.doc").arg(dir.currentPath()));

        docs->dynamicCall("Open(QString)",QString("%1/TheAnswer.doc").arg(dir.currentPath()));
    }

    word->setProperty("Visible", true);
    word = 0;
}

void NewQuestion::on_inputQuestionButton_clicked()
{
    inputFileIsQuestion(true);
}

void NewQuestion::on_inputAnswerButton_clicked()
{
    inputFileIsQuestion(false);
}

void NewQuestion::on_inputButton_clicked()
{
    QFile questionFile("OneQuestion.doc");
    QFile answerFile("TheAnswer.doc");

    if (!(questionFile.exists() && answerFile.exists())) {
        QMessageBox::information(this, tr("信息"), tr("请先点击“录入题目“和“录入答案“"),QMessageBox::Ok);
        return ;
    }

    word = new QAxWidget(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);    //隐藏word程序
    word->setProperty("DisplayAlerts", false); //不显示任何警告信息
    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得Documents对象"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return;
    }

    QString subjectName = ui->subjectCombox->currentText();
    QString questionTypeName = ui->questionTypeCombox->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return ;
    }

    QSqlQuery query;

    query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
    query.next();
    int numOfQuestions = query.value(0).toInt()+1;

    QClipboard *clip = QApplication::clipboard();
    const QMimeData *dat;
    QAxObject *active_doc;

    QDir currentDir(".");

    docs->dynamicCall("Open(QString)", QString("%1/OneQuestion.doc").arg(currentDir.currentPath()));
    active_doc = word->querySubObject("ActiveDocument");
    if (!active_doc) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得文档<OneQuestion.doc>！"), QMessageBox::Ok);
        word->dynamicCall("Quit(Boolean)", true);
        delete word;
        return;
    }
    active_doc->dynamicCall("Select()");
    active_doc->querySubObject("Range()")->dynamicCall("Copy()");

    dat = clip->mimeData();

    QString questionHtml;
    QString questionFilePath;

    if (dat->hasHtml()) {
        QString html = dat->html();

        QDir dir(".");

        QString outPath = QString("%1/%2/%3/%4")
                .arg(questionLibraryPath)
                .arg(subjectName)
                .arg(questionTypeName)
                .arg(numOfQuestions);
        dir.mkdir(outPath);
        extractImage(html, "Question", outPath);
        html.replace("\"","'");
        questionHtml = html;
        questionFilePath = outPath + "/question.doc";
        questionFile.copy(questionFilePath);
    }
    clip->clear();

    docs->dynamicCall("Open(QString)", QString("%1/TheAnswer.doc").arg(currentDir.currentPath()));
    active_doc = word->querySubObject("ActiveDocument");
    if (!active_doc) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得文档<TheAnswer.doc>！"), QMessageBox::Ok);
        word->dynamicCall("Quit(Boolean)", true);
        delete word;
        return;
    }
    active_doc->dynamicCall("Select()");
    active_doc->querySubObject("Range()")->dynamicCall("Copy()");

    dat = clip->mimeData();

    QString answerHtml;
    QString answerFilePath;

    if (dat->hasHtml()) {
        QString html = dat->html();
        QDir dir(".");

        QString outPath = QString("%1/%2/%3/%4")
                .arg(questionLibraryPath)
                .arg(subjectName)
                .arg(questionTypeName)
                .arg(numOfQuestions);

        dir.mkdir(outPath);
        extractImage(html, "Answer", outPath);
        html.replace("\"","'");
        answerHtml = html;
        answerFilePath = outPath + "/answer.doc";
        answerFile.copy(answerFilePath);
    }
    clip->clear();

    // TODO 录入题目与答案

    QString point = ui->pointEdit->text().trimmed();
    int     diff  = ui->diffSP->value();

    QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                    "%3,"           //id
                                    "\"%4\","       //Question
                                    "\"%5\","       //Answer
                                    "'%6',"         //QuestionDocPath
                                    "'%7',"         //AnswerDocPath
                                    "'%8',"         //Point
                                    "'%9',"         //Difficulty
                                    "'%10')"        //degrade
                                    )
            .arg(subjectName)
            .arg(questionTypeName)
            .arg(numOfQuestions)
            .arg(questionHtml)
            .arg(answerHtml)
            .arg(questionFilePath)
            .arg(answerFilePath)
            .arg(point)
            .arg(diff)
            .arg("0");

    bool status = query.exec(insertInfoCmd);
    if (status) {
        QMessageBox::information(this, tr("通知"),tr("试题录入成功！"),QMessageBox::Ok);
        query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));

        emit dataChanged();

    }else{
        QMessageBox::warning(this, tr("警告"), tr("试题录入失败!"), QMessageBox::Ok);
    }

    query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));

    questionFile.remove();
    answerFile.remove();
}

void NewQuestion::on_chooseFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开一个试题文件"),"",tr("Microsoft Word 97-2003文件(*.doc *.rtf);;Microsoft Word 2007-2010文件 (*.docx)"));

    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("文件名为空！"), QMessageBox::Ok);
        return ;
    }

    ui->inputOneFileNameEdit->setText(fileName);
}

void NewQuestion::on_inputOne_clicked()
{
    QString fileName = ui->inputOneFileNameEdit->text().trimmed();

    QString subjectName = ui->subjectCombox->currentText();
    QString questionTypeName = ui->questionTypeCombox->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QFileInfo inputFile(fileName);

    if (inputFile.suffix() != "doc" && inputFile.suffix() != "docx") {
        QMessageBox::warning(this, tr("警告"), tr("输入的文件不是Word文档！"), QMessageBox::Ok);
        return;
    }

    word = new QAxWidget(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);    //隐藏word程序
    word->setProperty("DisplayAlerts", false); //不显示任何警告信息

    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("获取当前打开文档失败！"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return ;
    }

    DocReadWriter *docread = new DocReadWriter(this);

    QSqlQuery query;

    query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
    query.next();
    int numOfQuestions = query.value(0).toInt()+1;

    QString outPath = QString("%1/%2/%3/%4")
            .arg(questionLibraryPath)
            .arg(subjectName)
            .arg(questionTypeName)
            .arg(numOfQuestions);
    QDir dir(".");
    dir.mkdir(outPath);

    docread->setSourceDest(fileName, outPath);
    docread->setDocuemnt(docs);

    if(!docread->readAndConvert()){
        QMessageBox::warning(this, tr("警告"), tr("文件书签信息提取失败！"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return ;
    }

    word->dynamicCall("Quit(boolean)", true);
    delete word;

    QString question = docread->getQuestion();
    question.replace("\"","'");

    QString answer = docread->getAnswer();
    answer.replace("\"","'");

    QString questionPath = docread->getQuestionDocPath();

    QString answerPath = docread->getAnswerDocPath();

    QString point = docread->getPoint();

    QString difficulty = docread->getDifficulty();

    QString degrade = docread->getDegrade();


    QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                    "%3,"           //id
                                    "\"%4\","       //Question
                                    "\"%5\","       //Answer
                                    "'%6',"         //QuestionDocPath
                                    "'%7',"         //AnswerDocPath
                                    "'%8',"         //Point
                                    "'%9',"         //Difficulty
                                    "'%10')"        //degrade
                                    )
            .arg(subjectName)
            .arg(questionTypeName)
            .arg(numOfQuestions)
            .arg(question)
            .arg(answer)
            .arg(questionPath)
            .arg(answerPath)
            .arg(point)
            .arg(difficulty)
            .arg(degrade);

    bool status = query.exec(insertInfoCmd);
    if (status) {
        query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));

        QMessageBox::information(this, tr("通知"),tr("试题录入成功！"),QMessageBox::Ok);

        emit dataChanged();

    }else{
        QMessageBox::warning(this, tr("警告"), tr("试题录入失败!"), QMessageBox::Ok);
    }

    query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));
    delete docread;
}

void NewQuestion::on_chooseInputDir_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, tr("选择试题文件保存的文件夹"),"");
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("未提供文件夹名！"), QMessageBox::Ok);
        return ;
    }

    ui->inputManyDirEdit->setText(filePath);
}

void NewQuestion::on_inputMany_clicked()
{
    QString inputManyPath = ui->inputManyDirEdit->text().trimmed();

    if (inputManyPath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择试题所在的文件夹"),QMessageBox::Ok);
        return ;
    }

    QString subjectName = ui->subjectCombox->currentText();
    QString questionTypeName = ui->questionTypeCombox->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QMessageBox::about(this, tr("说明"), tr("批量录入文件可能需要较长时间，请耐心等待进度条走完即可！"));

    QDir dir(inputManyPath);
    QStringList fileList = dir.entryList(QDir::Files);

    QSqlQuery query;

    query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
    query.next();
    int numOfQuestions = query.value(0).toInt();

    ui->inputManyProgressBar->setMaximum(fileList.length());
    ui->inputManyProgressBar->setValue(0);
    ui->inputManyProgressLabel->show();
    ui->inputManyProgressBar->show();

    word = new QAxWidget(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        ui->inputManyProgressBar->hide();
        ui->inputManyProgressLabel->hide();
        return ;
    }
    word->setProperty("Visible", false);        //隐藏word程序
    word->setProperty("DisplayAlerts", false);  //不显示任何警告信息

    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得Documents对象"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        ui->inputManyProgressBar->hide();
        ui->inputManyProgressLabel->hide();
        return;
    }


    int cnt= 0;
    int quescnt = 0;
    foreach(QString file, fileList){
        if (file.endsWith(".doc") || file.endsWith(".docx")) {
            DocReadWriter *docread = new DocReadWriter(this);

            QString outPath = QString("%1/%2/%3/%4")
                    .arg(questionLibraryPath)
                    .arg(subjectName)
                    .arg(questionTypeName)
                    .arg(numOfQuestions);
            QDir dir(".");
            dir.mkdir(outPath);
            docread->setSourceDest(QString("%1/%2")
                                   .arg(inputManyPath)
                                   .arg(file),
                                   outPath);
            docread->setDocuemnt(docs);

            if(!docread->readAndConvert()){
                QMessageBox::warning(this, tr("警告"), tr("文件\"%1\"书签信息提取失败！").arg(file), QMessageBox::Ok);
                word->dynamicCall("Quit(boolean)", true);
                delete word;
                ui->inputManyProgressBar->hide();
                ui->inputManyProgressLabel->hide();
                return ;
            }

            QString question = docread->getQuestion();
            question.replace("\"","'");

            QString answer = docread->getAnswer();
            answer.replace("\"","'");

            QString questionPath = docread->getQuestionDocPath();

            QString answerPath = docread->getAnswerDocPath();

            QString point = docread->getPoint();

            QString difficulty = docread->getDifficulty();

            QString degrade = docread->getDegrade();

            numOfQuestions++;

            QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                            "%3,"           //id
                                            "\"%4\","       //Question
                                            "\"%5\","       //Answer
                                            "'%6',"         //QuestionDocPath
                                            "'%7',"         //AnswerDocPath
                                            "'%8',"         //Point
                                            "'%9',"         //Difficulty
                                            "'%10')"        //degrade
                                            )
                    .arg(subjectName)
                    .arg(questionTypeName)
                    .arg(numOfQuestions)
                    .arg(question)
                    .arg(answer)
                    .arg(questionPath)
                    .arg(answerPath)
                    .arg(point)
                    .arg(difficulty)
                    .arg(degrade);

            if(!query.exec(insertInfoCmd)){
                QMessageBox::warning(this, tr("警告"), tr("录入文件\"%1\"时，发生错误！一般是书签信息提取失败。").arg(file), QMessageBox::Ok);
                ui->inputManyProgressBar->hide();
                ui->inputManyProgressLabel->hide();
                word->dynamicCall("Quit(boolean)", true);
                delete word;
                return;
            }
            delete docread;
            quescnt ++;
        }
        cnt ++;
        ui->inputManyProgressBar->setValue(cnt);
    }

    ui->inputManyProgressBar->hide();
    ui->inputManyProgressLabel->hide();

    query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));

    word->dynamicCall("Quit(boolean)", true);
    delete word;

    QMessageBox::information(this, tr("信息"),tr("批量录入文件完成，共录入了 %1 道题目").arg(quescnt), QMessageBox::Ok);

    emit dataChanged();

}

void NewQuestion::onDataChanged()
{
    subjectListRefresh();
    questionTypeListRefresh(ui->subjectCombox->currentText());
}

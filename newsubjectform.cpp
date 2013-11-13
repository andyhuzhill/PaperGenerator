#include "newsubjectform.h"
#include "ui_newsubjectform.h"


#include <ActiveQt/QAxWidget>
#include <ActiveQt/QAxObject>
#include "docreadwriter.h"
#include <QMessageBox>

#include <QString>
#include <QSqlQuery>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QTextBrowser>
#include <QVBoxLayout>
#include "defs.h"

newSubjectForm::newSubjectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newSubjectForm)
{
    ui->setupUi(this);

    setWindowTitle(tr("管理试题库"));

    ui->progressBar->hide();

    QSqlQuery query;
    query.exec("SELECT subjectName FROM subjects");
    ui->subjectsListsView->clear();
    while (query.next()) {
        ui->subjectsListsView->addItem(query.value(0).toString());
        ui->subjectsCB->addItem(query.value(0).toString());
    }

    QSettings settings(OrgName, AppName);

    questionLibraryPath = settings.value("paperPath").toString();
    questionLibraryPath.replace("\\","/");
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
        QDir dir(questionLibraryPath);
        dir.mkdir(subjectName);
        subjectListRefresh();
        QSqlQuery infoQuery;
        infoQuery.exec("SELECT numOfSubjects FROM allInof");
        while (infoQuery.next()) {
            int numOfSubjects = infoQuery.value(0).toInt();
            infoQuery.exec(QString("UPDATE allInfo SET numOfSubjects = %1 WHERE numOfSubjects = %2").arg(numOfSubjects+1).arg(numOfSubjects));
        }

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
                                    "'%3',"
                                    "%4)"
                                    ).arg(subjectName).arg(i+1).arg(questionType).arg(0);

    qDebug() << insertQuestionTypeCmd;

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
    qDebug() << insertQuestionTypeTableCmd;

    bool status1 = query.exec(insertQuestionTypeCmd);
    bool status2 = query.exec(insertQuestionTypeTableCmd);

    if (status1 && status2) {
        QMessageBox::information(this, tr("通知"), tr("题目类型插入完毕。"), QMessageBox::Ok);
        questionTypeListRefresh(subjectName);

        QDir dir(questionLibraryPath);
        dir.cd(subjectName);
        dir.mkdir(questionType);
    }else{
        QMessageBox::warning(this, tr("警告"), tr("题目类型插入失败！"), QMessageBox::Ok);
    }
}

void newSubjectForm::on_SubjectNameEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    questionTypeListRefresh(ui->SubjectNameEdit->text().trimmed());
}

void newSubjectForm::questionTypeListRefresh(QString subject)
{
    QSqlQuery query;

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        if (query.value(0).toString() == subject) {
            break;
        }
    }

    ui->questionTypeList->clear();
    ui->questionTypesCB->clear();
    if (query.value(0).toString() == subject) {
        query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(subject));
        while (query.next()) {
            ui->questionTypeList->addItem(query.value(0).toString());
            ui->questionTypesCB->addItem(query.value(0).toString());
        }
    }
}

void newSubjectForm::subjectListRefresh()
{
    QSqlQuery query;
    query.exec("SELECT subjectName from subjects");

    ui->subjectsListsView->clear();
    ui->subjectsCB->clear();
    while (query.next()) {
        ui->subjectsListsView->addItem(query.value(0).toString());
        ui->subjectsCB->addItem(query.value(0).toString());
    }
}

void newSubjectForm::on_subjectsListsView_currentTextChanged(const QString &currentText)
{
    questionTypeListRefresh(currentText);
    ui->SubjectNameEdit->setText(currentText);
}

void newSubjectForm::on_questionTypeList_currentTextChanged(const QString &currentText)
{
    ui->questionTypeEdit->setText(currentText);
}

void newSubjectForm::on_deleteSelectedSubject_clicked()
{
    QString subjectToBeDelete;
    if(ui->subjectsListsView->currentItem()){
        subjectToBeDelete = ui->subjectsListsView->currentItem()->text();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("没有选中要删除的课程名，请先选中"),QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;

    if(!query.exec(QString("SELECT questionTypes FROM '%1'").arg(subjectToBeDelete))){
        QMessageBox::warning(this, tr("警告"), tr("删除失败！"), QMessageBox::Ok);
        return ;
    }

    qDebug() << "SELECT questionTypes FROM '%1'" << QString("SELECT questionTypes FROM '%1'").arg(subjectToBeDelete);

    QStringList types;

    while (query.next()) {
        types.append(query.value(0).toString());
    }

    foreach (QString str, types) {
        if(!query.exec(QString("DROP TABLE '%1_%2'").arg(subjectToBeDelete).arg(str))){
            QMessageBox::warning(this, tr("警告"), tr("删除失败！"), QMessageBox::Ok);
            return;
        }
    }

    bool status1 = query.exec(QString("DELETE FROM subjects where subjectName = '%1'").arg(subjectToBeDelete));

    bool status2 = query.exec(QString("DROP TABLE '%1'").arg(subjectToBeDelete));

    query.exec("SELECT numOfSubjects FROM allInof");
    while (query.next()) {
        int numOfSubjects = query.value(0).toInt();
        query.exec(QString("UPDATE allInfo SET numOfSubjects = %1 WHERE numOfSubjects = %2").arg(numOfSubjects-1).arg(numOfSubjects));
    }

    if (status1 && status2) {
        QMessageBox::information(this, tr("信息"), tr("课程删除成功！"), QMessageBox::Ok);
        subjectListRefresh();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("课程删除失败"), QMessageBox::Ok);
    }

}

void newSubjectForm::on_deleteSelectedType_clicked()
{
    QString questionTypesToBeDelete;
    if (ui->questionTypeList->currentItem()) {
        questionTypesToBeDelete = ui->questionTypeList->currentItem()->text();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("没有选中要删除的题目类型，请先选中"),QMessageBox::Ok);
        return ;
    }

    QString subject = ui->SubjectNameEdit->text().trimmed();

    QSqlQuery query;
    bool status1 = query.exec(QString("DELETE FROM '%1' where questionTypes = '%2'").arg( subject ).arg(questionTypesToBeDelete));
    bool status2 = query.exec(QString("DROP TABLE '%1_%2").arg(subject).arg(questionTypesToBeDelete));

    if (status1 && status2) {
        QMessageBox::information(this, tr("信息"), tr("题目类型删除成功！"),QMessageBox::Ok);
        questionTypeListRefresh(subject);
    }else{
        QMessageBox::warning(this, tr("警告"), tr("题目类型删除失败！"),QMessageBox::Ok);
    }
}

void newSubjectForm::on_subjectsCB_currentIndexChanged(const QString &arg1)
{
    questionTypeListRefresh(arg1);
}

void newSubjectForm::on_chooseFileName_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开一个试题文件"),"",tr("Microsoft Word 97-2003文件(*.doc *.rtf);;Microsoft Word 2007-2010文件 (*.docx)"));

    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("文件名为空！"), QMessageBox::Ok);
        return ;
    }

    ui->inputFileNameEdit->setText(fileName);
}

void newSubjectForm::on_chooseInputFilePath_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, tr("选择试题文件保存的文件夹"),"");
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("未提供目录名！"), QMessageBox::Ok);
        return ;
    }

    ui->inputFilesPathEdit->setText(filePath);
}

void newSubjectForm::on_startInput_clicked()
{
    QString fileName = ui->inputFileNameEdit->text().trimmed();

    QString subjectName = ui->subjectsCB->currentText();
    QString questionTypeName = ui->questionTypesCB->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QFileInfo inputFile(fileName);

    if (inputFile.suffix() != "doc" && inputFile.suffix() != "docx") {
        QMessageBox::warning(this, tr("警告"), tr("输入的文件不是Word文档！"), QMessageBox::Ok);
        return;
    }

    word = new QAxWidget("Word.Application");
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);    //隐藏word程序


    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("获取当前打开文档失败！"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return ;
    }

    DocReadWriter *docread = new DocReadWriter(this, fileName, QString("%1/%2/%3").arg(questionLibraryPath).arg(subjectName).arg(questionTypeName));

    docread->setDocuemnt(docs);

    if(!docread->convert()){
        QMessageBox::warning(this, tr("警告"), tr("文件信息提取失败！"), QMessageBox::Ok);
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

    QString degrade = "";

    QSqlQuery query;

    QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                    "%3,"           //id
                                    "\"%4\","       //Question
                                    "\"%5\","       //Answer
                                    "'%6',"         //QuestionDocPath
                                    "'%7',"         //AnswerDocPath
                                    "'%8',"         //Point
                                    "'%9',"         //Difficulty
                                    "'%10')"        //degrade
                                    ).arg(ui->subjectsCB->currentText()).arg(ui->questionTypesCB->currentText()).arg(1).arg(question).arg(answer).arg(questionPath).arg(answerPath).arg(point).arg(difficulty).arg(degrade);

    bool status = query.exec(insertInfoCmd);
    if (status) {
        QMessageBox::information(this, tr("通知"),tr("试题录入成功！"),QMessageBox::Ok);
        //        QWidget *widget = new QWidget();
        //        QVBoxLayout *layout = new QVBoxLayout(widget);
        //        QTextBrowser *tb = new QTextBrowser(this);

        //        QString sele;
        //        sele = QString("SELECT Question FROM %1_%2").arg(subjectName).arg(questionType);
        //        query.exec(sele);
        //        while(query.next()){
        //            tb->insertHtml(query.value(0).toString());
        //        }

        //        sele = QString("SELECT Answer FROM %1_%2").arg(subjectName).arg(questionType);
        //        query.exec(sele);
        //        while(query.next()){
        //            tb->insertHtml(query.value(0).toString());
        //        }

        //        layout->addWidget(tb);
        //        widget->setLayout(layout);
        //        widget->show();

    }else{
        QMessageBox::warning(this, tr("警告"), tr("试题录入失败!"), QMessageBox::Ok);
    }
    delete docread;
}


void newSubjectForm::on_startInputMany_clicked()
{
    QString inPath = ui->inputFilesPathEdit->text().trimmed();

    if (inPath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择试题所在的文件夹"),QMessageBox::Ok);
        return ;
    }

    QString subjectName = ui->subjectsCB->currentText();
    QString questionTypeName = ui->questionTypesCB->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    word = new QAxWidget("Word.Application");
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);    //隐藏word程序

    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得Documents对象"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return;
    }

    QDir dir(inPath);
    QStringList fileList = dir.entryList(QDir::Files);

    QSqlQuery query;

    ui->progressBar->setMaximum(fileList.length());
    ui->progressBar->show();

    int cnt= 0;
    foreach(QString file, fileList){
        if (file.endsWith(".doc") || file.endsWith(".docx")) {
            DocReadWriter *docread = new DocReadWriter(this, QString("%1/%2").arg(inPath).arg(file), QString("%1/%2/%3").arg(questionLibraryPath).arg(subjectName).arg(questionTypeName));

            docread->setDocuemnt(docs);

            if(!docread->convert()){
                QMessageBox::warning(this, tr("警告"), tr("文件\"%1\"信息提取失败！").arg(file), QMessageBox::Ok);
                word->dynamicCall("Quit(boolean)", true);
                delete word;
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

            QString degrade = "";

            QSqlQuery query;

            QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                            "%3,"           //id
                                            "\"%4\","       //Question
                                            "\"%5\","       //Answer
                                            "'%6',"         //QuestionDocPath
                                            "'%7',"         //AnswerDocPath
                                            "'%8',"         //Point
                                            "'%9',"         //Difficulty
                                            "'%10')"        //degrade
                                            ).arg(subjectName).arg(questionTypeName).arg(1).arg(question).arg(answer).arg(questionPath).arg(answerPath).arg(point).arg(difficulty).arg(degrade);

            if(!query.exec(insertInfoCmd)){
                QMessageBox::warning(this, tr("警告"), tr("录入文件\"%1\"时，发生错误！").arg(file), QMessageBox::Ok);
                return;
            }

            delete docread;
        }
        cnt ++;
        ui->progressBar->setValue(cnt);
    }

    ui->progressBar->hide();

    word->dynamicCall("Quit(boolean)", true);
    delete word;
}

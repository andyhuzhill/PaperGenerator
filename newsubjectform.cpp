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
#include <QClipboard>
#include <QMimeData>
#include <QList>
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
    ui->subjectCB2->clear();
    ui->subjectsCB->clear();
    while (query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());

        ui->subjectsListsView->addItem(item);
        ui->subjectsCB->addItem(item->text());
        ui->subjectCB2->addItem(item->text());
    }

    QSettings settings(OrgName, AppName);

    questionLibraryPath = settings.value("paperPath").toString();
    questionLibraryPath.replace("\\","/");

    questionNumRefresh();
    modifyWhich = EMod_None;
}

newSubjectForm::~newSubjectForm()
{
    delete ui;
}

void newSubjectForm::closeEvent(QCloseEvent *)
{
    emit contentChanged();
}

/** @brief 将HTML字符串中的图片提取出来
 *
 *  程序中利用了Word复制到剪贴板时产生的HTML文件，\n
 *  需要将HTML中的图片提取出来单独保存。
 *
 *  @param INOUT QString &html Word产生的HTML文件
 *  @param QString  QorA       这是题目还是答案产生的HTML文件?
 *  @param QString  saveToPath 将图片保存到路径
 */
void newSubjectForm::extractImage(QString &html, QString QorA, QString saveToPath)
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

/** @brief 新建一个科目 */
void newSubjectForm::on_newSubjectButton_clicked()
{
    QString subjectName = ui->SubjectNameEdit->text().trimmed();
    if (subjectName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("课程名称不能为空！"), QMessageBox::Ok);
        return ;
    }

    ui->SubjectNameEdit->setText(subjectName);

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

        ui->questionTypeList->clear();
        ui->questionTypesCB->clear();
    }else{
        QMessageBox::information(this, tr("警告"), tr("课程数据库创建失败！"), QMessageBox::Ok);
    }
}

/** @brief 新建一个题目类型  */
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
                                    ).arg(subjectName).arg(i+1).arg(questionType).arg(0);

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
    ui->quesTypeCB2->clear();

    if (query.value(0).toString() == subject) {
        query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(subject));
        while (query.next()) {
            QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());
            ui->questionTypeList->addItem(item);
            ui->questionTypesCB->addItem(item->text());
            ui->quesTypeCB2->addItem(item->text());
        }
    }
}

void newSubjectForm::subjectListRefresh()
{
    QSqlQuery query;
    query.exec("SELECT subjectName from subjects");

    ui->subjectsListsView->clear();
    ui->subjectsCB->clear();
    ui->subjectCB2->clear();
    while (query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());

        ui->subjectsListsView->addItem(item);
        ui->subjectsCB->addItem(item->text());
        ui->subjectCB2->addItem(item->text());
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
            ui->questionTypesCB->clear();
            return ;
        }
    }

    subjectListRefresh();
    ui->questionTypeList->clear();
    ui->questionTypesCB->clear();

    QMessageBox::information(this, tr("通知"), tr("选择的科目已经删除"), QMessageBox::Ok);

}

void newSubjectForm::on_deleteSelectedType_clicked()
{
    QList<QListWidgetItem*> selectedQuestionTypes = ui->questionTypeList->selectedItems();

    if (selectedQuestionTypes.length() == 0) {
        QMessageBox::warning(this, tr("警告"), tr("没有选中要删除的题目类型，请先选中"),QMessageBox::Ok);
        return ;
    }

    QString subjectName = ui->SubjectNameEdit->text().trimmed();
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

    docread->setSourceDest(fileName, QString("%1/%2/%3").arg(questionLibraryPath).arg(subjectName).arg(questionTypeName));
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

    QSqlQuery query;

    query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
    query.next();
    int numOfQuestions = query.value(0).toInt()+1;

    QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                    "%3,"           //id
                                    "\"%4\","       //Question
                                    "\"%5\","       //Answer
                                    "'%6',"         //QuestionDocPath
                                    "'%7',"         //AnswerDocPath
                                    "'%8',"         //Point
                                    "'%9',"         //Difficulty
                                    "'%10')"        //degrade
                                    ).arg(ui->subjectsCB->currentText()).arg(ui->questionTypesCB->currentText()).arg(numOfQuestions).arg(question).arg(answer).arg(questionPath).arg(answerPath).arg(point).arg(difficulty).arg(degrade);

    bool status = query.exec(insertInfoCmd);
    if (status) {
        QMessageBox::information(this, tr("通知"),tr("试题录入成功！"),QMessageBox::Ok);
        query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));
        questionNumRefresh();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("试题录入失败!"), QMessageBox::Ok);
    }

    query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));
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

    QMessageBox::about(this, tr("说明"), tr("批量录入文件可能需要较长时间，请耐心等待进度条走完即可！"));

    word = new QAxWidget(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);        //隐藏word程序
    word->setProperty("DisplayAlerts", false);  //不显示任何警告信息

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

    query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
    query.next();
    int numOfQuestions = query.value(0).toInt();

    ui->progressBar->setMaximum(fileList.length());
    ui->progressBar->show();

    int cnt= 0;
    int quescnt = 0;
    foreach(QString file, fileList){
        if (file.endsWith(".doc") || file.endsWith(".docx")) {
            DocReadWriter *docread = new DocReadWriter(this);

            docread->setSourceDest(QString("%1/%2").arg(inPath).arg(file), QString("%1/%2/%3").arg(questionLibraryPath).arg(subjectName).arg(questionTypeName));
            docread->setDocuemnt(docs);

            if(!docread->readAndConvert()){
                QMessageBox::warning(this, tr("警告"), tr("文件\"%1\"书签信息提取失败！").arg(file), QMessageBox::Ok);
                word->dynamicCall("Quit(boolean)", true);
                delete word;
                ui->progressBar->hide();
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
                                            ).arg(subjectName).arg(questionTypeName).arg(numOfQuestions).arg(question).arg(answer).arg(questionPath).arg(answerPath).arg(point).arg(difficulty).arg(degrade);

            if(!query.exec(insertInfoCmd)){
                QMessageBox::warning(this, tr("警告"), tr("录入文件\"%1\"时，发生错误！一般是书签信息提取失败。").arg(file), QMessageBox::Ok);
                ui->progressBar->hide();
                word->dynamicCall("Quit(boolean)", true);
                delete word;
                return;
            }
            delete docread;
            quescnt ++;
        }
        cnt ++;
        ui->progressBar->setValue(cnt);
    }

    ui->progressBar->hide();

    query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));

    word->dynamicCall("Quit(boolean)", true);
    delete word;

    QMessageBox::information(this, tr("信息"),tr("批量录入文件成功，共录入了%1道题目").arg(quescnt), QMessageBox::Ok);

    questionNumRefresh();
}

void newSubjectForm::on_subjectCB2_currentIndexChanged(const QString &arg1)
{
    questionTypeListRefresh(arg1);

    questionNumRefresh();
}

void newSubjectForm::on_quesTypeCB2_currentIndexChanged(const QString &arg1)
{
    ui->questionTypeEdit->setText(arg1);

    questionNumRefresh();
}

void newSubjectForm::on_changeButton_clicked()
{
    QSqlQuery query;
    if (modifyWhich!=0) {
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

        QString subjectName = ui->subjectCB2->currentText();
        QString questionTypeName = ui->quesTypeCB2->currentText();

        if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
            QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
            word->dynamicCall("Quit(boolean)", true);
            delete word;
            return ;
        }

        QClipboard *clip = QApplication::clipboard();
        const QMimeData *dat;
        QAxObject *active_doc;
        switch(modifyWhich){
        case EMod_Question:
            query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
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
                query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Question", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Question = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questNumCB->text().toInt()));
            }
            clip->clear();
            break;
        case EMod_Answer:
            query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
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
                query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
                query.next();
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Answer", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Answer = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questNumCB->text().toInt()));
            }
            clip->clear();
            break;
        default:
            query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
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
                query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Question", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Question = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questNumCB->text().toInt()));
            }
            clip->clear();

            query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
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
                query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
                query.next();
                QString outPath = query.value(0).toString().remove("Answer.doc");
                extractImage(html, "Answer", outPath);
                html.replace("\"","'");
                query.exec(QString("UPDATE '%1_%2' SET Answer = \"%3\" WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(html).arg(ui->questNumCB->text().toInt()));
            }
            clip->clear();
            break;
        }
        word->dynamicCall("Quit(boolean)", true);
        delete word;
    }

    QString subjectName = ui->subjectCB2->currentText();
    QString questionTypeName = ui->quesTypeCB2->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QString strDegrade = ui->degradeEdit->text().trimmed();
    QString strDifficulty = ui->difficultyEdit->text().trimmed();
    QString strPoint = ui->pointEdit->toPlainText();

    bool updateDegradeResult = query.exec(QString("UPDATE '%1_%2' SET Degrade = '%3' WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(strDegrade).arg(ui->questNumCB->text().toInt()));
    bool updatePointResult = query.exec(QString("UPDATE '%1_%2' SET Point = '%3' WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(strPoint).arg(ui->questNumCB->text().toInt()));
    bool updateDifficultyResult = query.exec(QString("UPDATE '%1_%2' SET Difficulty = '%3' WHERE id = %4").arg(subjectName).arg(questionTypeName).arg(strDifficulty).arg(ui->questNumCB->text().toInt()));

    if (updateDegradeResult && updatePointResult && updateDifficultyResult) {
        QMessageBox::information(this, tr("通知"), tr("修改试题成功！"), QMessageBox::Ok);
        on_questNumCB_valueChanged(ui->questNumCB->text().toInt());

        modifyWhich = EMod_None;
    }else{
        QMessageBox::warning(this, tr("警告"), tr("修改试题失败！"), QMessageBox::Ok);
    }
}

void newSubjectForm::on_questNumCB_valueChanged(int arg1)
{
    QString subjectName = ui->subjectCB2->currentText();
    QString questionTypeName = ui->quesTypeCB2->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        ui->answerBrowser->clear();
        ui->questionBrowser->clear();
        ui->difficultyEdit->clear();
        ui->pointEdit->clear();
        ui->degradeEdit->clear();
        return ;
    }

    QSqlQuery query;

    query.exec(QString("SELECT * FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
    query.next();
    QString Question = query.value(1).toString();
    QString Answer = query.value(2).toString();
    QString Point = query.value(5).toString();
    QString Difficulty = query.value(6).toString();
    QString Degrade = query.value(7).toString();

    ui->questionBrowser->clear();
    ui->questionBrowser->insertHtml(Question);
    ui->answerBrowser->clear();
    ui->answerBrowser->insertHtml(Answer);
    ui->pointEdit->setText(Point);
    ui->degradeEdit->setText(Degrade);
    ui->difficultyEdit->setText(Difficulty);
}

void newSubjectForm::on_deleteButton_clicked()
{
    QString subjectName = ui->subjectCB2->currentText();
    QString questionTypeName = ui->quesTypeCB2->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;

    bool status1 = query.exec(QString("DELETE FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));

    if(status1){
        QMessageBox::information(this, tr("通知"), tr("试题删除成功！"), QMessageBox::Ok);
        ui->questionBrowser->clear();
        ui->answerBrowser->clear();
        ui->degradeEdit->clear();
        ui->difficultyEdit->clear();
        ui->pointEdit->clear();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("试题删除失败！"), QMessageBox::Ok);
    }
}

void newSubjectForm::questionNumRefresh()
{
    QSqlQuery query;

    if(query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(ui->subjectCB2->currentText()).arg(ui->quesTypeCB2->currentText()))){
        query.next();
        int maxNum = query.value(0).toInt();
        ui->questNumCB->setMaximum(maxNum);
        ui->numOfQuestiongLabel->setText(tr("共 %1 题").arg(maxNum));
    }
}

void newSubjectForm::on_newQuestionButton_clicked()
{
    word = new QAxWidget(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("无法找到Word程序，请检查是否已安装Micrsoft Word！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);
    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获取Documents对象！"), QMessageBox::Ok);
        word->dynamicCall("Quit(Boolean)", true);
        delete word;
        return;
    }
    QDir dir(".");
    QFile templa(QString("%1/question.dot").arg(dir.currentPath()));
    templa.copy(QString("%1/newQuestion.doc").arg(dir.currentPath()));

    docs->dynamicCall("Open(QString)",QString("%1/newQuestion.doc").arg(dir.currentPath()));
    word->setProperty("Visible", true);
    word = 0;
}

void newSubjectForm::on_modifyQuestion_clicked()
{
    modifyQA("Question");
}

void newSubjectForm::on_modifyAnswer_clicked()
{
    modifyQA("Answer");
}

void newSubjectForm::modifyQA(QString QorA)
{
    word = new QAxWidget(wordAppName);
    word->setControl(wordAppName);
    if (!word) {
        QMessageBox::warning(this, tr("警告"), tr("未发现在您的电脑上安装有Microsoft Word 程序， 请您先安装word以使用本程序！"), QMessageBox::Ok);
        return ;
    }
    word->setProperty("Visible", false);    //隐藏word程序
    word->setProperty("DisplayAlerts", false); //不显示任何警告信息
    QAxObject *docs = word->querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获取Documents对象！"), QMessageBox::Ok);
        return;
    }

    QString subjectName = ui->subjectCB2->currentText();
    QString questionTypeName = ui->quesTypeCB2->currentText();
    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;
    if (QorA == "Question") {
        query.exec(QString("SELECT QuestionDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
        if (modifyWhich != EMod_Answer) {
            modifyWhich = EMod_Question;
        }else{
            modifyWhich = EMod_QandA;
        }
    }else{
        query.exec(QString("SELECT AnswerDocPath FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(ui->questNumCB->text().toInt()));
        if (modifyWhich != EMod_Question) {
            modifyWhich = EMod_Answer;
        }else{
            modifyWhich = EMod_QandA;
        }
    }
    query.next();
    QString docPath = query.value(0).toString();
    docs->dynamicCall("Open(QString)", docPath);
    word->setProperty("Visible", true);
}

void newSubjectForm::on_inputButton_clicked()
{
    QDir dir(".");
    QString fileName = QString("%1/newQuestion.doc").arg(dir.currentPath());
    QFile fi(fileName);
    if (!fi.exists()) {
        QMessageBox::warning(this, tr("警告"), tr("没有新建的题目，请先新建！"), QMessageBox::Ok);
        return ;
    }

    QString subjectName = ui->subjectsCB->currentText();
    QString questionTypeName = ui->questionTypesCB->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先创建课程和题型！"), QMessageBox::Ok);
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
        QMessageBox::warning(this, tr("警告"), tr("获取当前打开文档失败！"), QMessageBox::Ok);
        word->dynamicCall("Quit(boolean)", true);
        delete word;
        return ;
    }

    DocReadWriter *docread = new DocReadWriter(this);

    docread->setSourceDest(fileName, QString("%1/%2/%3").arg(questionLibraryPath).arg(subjectName).arg(questionTypeName));
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

    QSqlQuery query;

    query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
    query.next();
    int numOfQuestions = query.value(0).toInt()+1;

    QString insertInfoCmd = QString("INSERT INTO '%1_%2' VALUES("
                                    "%3,"           //id
                                    "\"%4\","       //Question
                                    "\"%5\","       //Answer
                                    "'%6',"         //QuestionDocPath
                                    "'%7',"         //AnswerDocPath
                                    "'%8',"         //Point
                                    "'%9',"         //Difficulty
                                    "'%10')"        //degrade
                                    ).arg(ui->subjectsCB->currentText()).arg(ui->questionTypesCB->currentText()).arg(numOfQuestions).arg(question).arg(answer).arg(questionPath).arg(answerPath).arg(point).arg(difficulty).arg(degrade);

    bool status = query.exec(insertInfoCmd);
    if (status) {
        QMessageBox::information(this, tr("通知"),tr("试题录入成功！"),QMessageBox::Ok);
        query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));
        questionNumRefresh();
        QFile rmfile(fileName);
        rmfile.remove();
    }else{
        QMessageBox::warning(this, tr("警告"), tr("试题录入失败!"), QMessageBox::Ok);
    }

    query.exec(QString("UPDATE '%1' SET numOfQuestions = %2 WHERE questionTypes = '%3'").arg(subjectName).arg(numOfQuestions).arg(questionTypeName));
    delete docread;
}

void newSubjectForm::on_returnButton_clicked()
{
    emit contentChanged();
    close();
}

void newSubjectForm::on_subjectsCB_activated(const QString &arg1)
{
    questionTypeListRefresh(arg1);
}

void newSubjectForm::on_subjectCB2_activated(const QString &arg1)
{
    questionTypeListRefresh(arg1);
}

#include "newtestform.h"
#include "ui_newtestform.h"
#include <QSqlQuery>
#include <QDebug>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <ActiveQt/QAxWidget>
#include <ActiveQt/QAxObject>
#include <QDesktopServices>
#include <QDateTime>

#include "defs.h"
#include "docreadwriter.h"

newTestForm::newTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newTestForm)
{
    ui->setupUi(this);

    setWindowTitle(tr("新建一份试卷"));

    QSqlQuery query;

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        ui->SubjectNameCB->addItem(query.value(0).toString());
    }

    questions.clear();
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
    if (arg1.isEmpty()) {
        return ;
    }
    QSqlQuery query;

    query.exec("SELECT subjectName FROM subjects");

    while (query.next()) {
        if (query.value(0).toString() == arg1) {
            break;
        }
    }
    ui->questionTypeCB->clear();
    if (query.value(0).toString() == arg1) {
        query.exec(QString("SELECT questionTypes FROM \"%1\"").arg(arg1));
        while (query.next()) {
            ui->questionTypeCB->addItem(query.value(0).toString());
        }
    }

    questionNumRefresh();
}



void newTestForm::on_questionNum_valueChanged(int arg1)
{
    QString subjectName = ui->SubjectNameCB->currentText();
    QString questionTypeName = ui->questionTypeCB->currentText();

    if (subjectName.isEmpty() || questionTypeName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先新建课程与题目类型！"), QMessageBox::Ok);
        ui->questionTextBrowser->clear();
        ui->answerTextBrowser->clear();
        ui->degradeEdit->clear();
        ui->difficultyEdit->clear();
        ui->pointEdit->clear();
        return ;
    }

    QSqlQuery query;

    query.exec(QString("SELECT * FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(arg1));

    query.next();
    QString Question = query.value(1).toString();
    QString Answer = query.value(2).toString();
    QString Point = query.value(5).toString();
    QString Difficulty = query.value(6).toString();
    QString Degrade = query.value(7).toString();

    ui->questionTextBrowser->clear();
    ui->questionTextBrowser->insertHtml(Question);
    ui->answerTextBrowser->clear();
    ui->answerTextBrowser->insertHtml(Answer);
    ui->degradeEdit->setText(Degrade);
    ui->pointEdit->setText(Point);
    ui->difficultyEdit->setText(Difficulty);
}

void newTestForm::on_questionTypeCB_currentIndexChanged(const QString &arg1)
{
    QString subjectName = ui->SubjectNameCB->currentText();
    questionNumRefresh();
}

void newTestForm::on_chooseThisQuestion_clicked()
{
    QSqlQuery query;
    QString subjectName = ui->SubjectNameCB->currentText();
    QString questionTypeName = ui->questionTypeCB->currentText();
    int id  = ui->questionNum->text().toInt();

    query.exec(QString("SELECT * FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(id));

    query.next();
    QString QuestionPath = query.value(3).toString();
    QString AnswerPath = query.value(4).toString();
    QString Point = query.value(5).toString();
    QString Difficulty = query.value(6).toString();
    QString Degrade = query.value(7).toString();
    questions.append(Question(id, ui->questionTypeCB->currentText(), QuestionPath, AnswerPath, Point, Degrade, Difficulty));
    QMessageBox::about(this, tr("信息"), tr("已选中本题！"));
}

void newTestForm::questionNumRefresh()
{
    QSqlQuery query;

    if(query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(ui->SubjectNameCB->currentText()).arg(ui->questionTypeCB->currentText()))){
        query.next();
        int maxNum = query.value(0).toInt();
        ui->questionNum->setMaximum(maxNum);
        ui->questNum->setMaximum(maxNum);
        ui->totalQuestionNum->setText(tr("共 %1 题").arg(maxNum));
    }
}

void newTestForm::on_generatePaperButton_clicked()
{
    if (questions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"),tr("已选题目为空，请先选择题目！"),QMessageBox::Ok);
        return;
    }
    const QString chieseDigit[] = {
        tr("零"),
        tr("一"),
        tr("二"),
        tr("三"),
        tr("四"),
        tr("五"),
        tr("六"),
        tr("七"),
        tr("八"),
        tr("九"),
        tr("十")
    };

    QString outfileName = QFileDialog::getSaveFileName(this, tr("请输入要保存的试卷文件的名字："),".", "Microsoft Word 97-2003(*.doc);;Microsoft Word 2007-2013(*.docx)");
    if (outfileName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"),tr("输入的试卷文件名为空！"),QMessageBox::Ok);
        return ;
    }

    QAxWidget word(wordAppName);
    word.setProperty("Visible", false);
    word.setProperty("DisplayAlerts", false);
    QDir dir(".");

    QAxObject *docs = word.querySubObject("Documents");
    if (!docs) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得Documents对象!"),QMessageBox::Ok);
        return ;
    }

    QList<Question> answers;
    for (int i = 0; i < questions.length(); ++i) {
        answers.append(questions.at(i));
    }

    /////////////////////////////////////////////////////////////////////////

    docs->dynamicCall("Add(QString)", QString("%1/template.dot").arg(dir.absolutePath()));

    QAxObject *questionDoc = word.querySubObject("ActiveDocument");
    if(!questionDoc){
        QMessageBox::warning(this, tr("警告"), tr("无法获取当前打开文件对象!"),QMessageBox::Ok);
        return;
    }
    QAxObject *questionBookmarks = questionDoc->querySubObject("bookmarks");
    int count = questionBookmarks->property("Count").toInt();

    QString bookmarksDoc = questionBookmarks->generateDocumentation();
    QFile outfile("Bookmarks.html");
    outfile.open(QIODevice::WriteOnly);
    QTextStream ts(&outfile);
    ts << bookmarksDoc ;
    outfile.close();

    QAxObject *Item = questionBookmarks->querySubObject("Item(QVariant)", 2);

    QString ItemDoc = Item->generateDocumentation();
    QFile oitfil("Item.html");
    oitfil.open(QIODevice::WriteOnly);
    QTextStream tts(&oitfil);
    tts << ItemDoc ;
    oitfil.close();
    for (int i = count; i > 0; --i) {
        QAxObject *bookmark = questionBookmarks->querySubObject("Item(QVariant)", i);
        QString name= bookmark->property("Name").toString();
        QString text = QInputDialog::getText(this, tr("请输入"), tr("%1").arg(name));
        bookmark->querySubObject("Range")->setProperty("Text", text);
    }

    int cnt=0;
    int tihao = 1;
    do{
        Question ques = questions.at(cnt);
        QString questionType = ques.getQuestionType();
        QAxObject *questionRange = questionDoc->querySubObject("Range()");
        int rangeEnd = questionRange->property("End").toInt();
        questionRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
        questionRange->dynamicCall("InsertAfter(QString)", QString("%1. %2\n").arg(chieseDigit[tihao]).arg(questionType));
        tihao ++;

        QList<int> quesToBeRemoved;
        int xiaotihao=1;
        for (int i = 0; i < questions.length(); ++i) {
            Question ques = questions.at(i);
            if (questionType == ques.getQuestionType()) {
                questionDoc->dynamicCall("Select()");
                rangeEnd = questionDoc->querySubObject("Range()")->property("End").toInt();
                questionRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
                questionRange->dynamicCall("InsertAfter(QString)", QString("%1. ").arg(xiaotihao));
                questionDoc->dynamicCall("Select()");
                rangeEnd = questionDoc->querySubObject("Range()")->property("End").toInt();
                questionRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
                questionRange->dynamicCall("InsertFile(QString)", ques.getQuestionPath());
                questionDoc->dynamicCall("Select()");
                rangeEnd = questionDoc->querySubObject("Range()")->property("End").toInt();
                questionRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
                questionRange->dynamicCall("InsertAfter(QString)", "\n");
                quesToBeRemoved.append(i);
                xiaotihao ++;
            }
        }
        for (int i = (quesToBeRemoved.length()>0?quesToBeRemoved.length()-1:-1); i >= 0; --i) {
            questions.removeAt(quesToBeRemoved.at(i));
        }

    }while(!questions.isEmpty());

    questionDoc->dynamicCall("SaveAs(QString&)", outfileName);
    questionDoc->dynamicCall("Close()");

    /////////////////////////////////////////////////////////////////////////

    QAxObject *answerDoc = docs->querySubObject("Add(QString)", QString("%1/template.dot").arg(dir.absolutePath()));
    if (!answerDoc) {
        QMessageBox::warning(this, tr("警告"), tr("无法获得answerDocument对象!"), QMessageBox::Ok);
        return;
    }
    QAxObject *answerBookmarks = answerDoc->querySubObject("bookmarks");
    count = answerBookmarks->property("Count").toInt();

    for (int i = count; i >0 ; --i) {
        QAxObject *bookmark = answerBookmarks->querySubObject("Item(QVariant)", i);
        QString name= bookmark->property("Name").toString();
        QString text = QInputDialog::getText(this, tr("请输入"), tr("%1").arg(name));
        bookmark->querySubObject("Range")->setProperty("Text", text);
    }

    cnt=0;
    tihao = 1;
    do{
        Question ans = answers.at(cnt);
        QString questionType = ans.getQuestionType();
        QAxObject *answerRange = answerDoc->querySubObject("Range()");
        int rangeEnd = answerRange->property("End").toInt();
        answerRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
        answerRange->dynamicCall("InsertAfter(QString)", QString("%1. %2\n").arg(chieseDigit[tihao]).arg(questionType));
        tihao ++;

        QList<int> quesToBeRemoved;
        int xiaotihao=1;
        for (int i = 0; i < answers.length(); ++i) {
            Question ques = answers.at(i);
            if (questionType == ques.getQuestionType()) {
                answerDoc->dynamicCall("Select()");
                rangeEnd = answerDoc->querySubObject("Range()")->property("End").toInt();
                answerRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
                answerRange->dynamicCall("InsertAfter(QString)", QString("%1. ").arg(xiaotihao));
                answerDoc->dynamicCall("Select()");
                rangeEnd = answerDoc->querySubObject("Range()")->property("End").toInt();
                answerRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
                answerRange->dynamicCall("InsertFile(QString)", ques.getAnswerPath());
                answerDoc->dynamicCall("Select()");
                rangeEnd = answerDoc->querySubObject("Range()")->property("End").toInt();
                answerRange->dynamicCall("setRange(QVariant, QVariant)", rangeEnd, rangeEnd);
                answerRange->dynamicCall("InsertAfter(QString)", "\n");
                quesToBeRemoved.append(i);
                xiaotihao ++;
            }
        }
        for (int i = (quesToBeRemoved.length()>0?quesToBeRemoved.length()-1:-1); i >= 0; --i) {
            answers.removeAt(quesToBeRemoved.at(i));
        }
    }while(!answers.isEmpty());

    answerDoc->dynamicCall("SaveAs(QString&)", tr("%1_答案.doc").arg(outfileName.remove(".doc")));
    answerDoc->dynamicCall("Close()");

    /////////////////////////////////////////////////////////////////////////

    word.dynamicCall("Quit(boolean)", true);

    QDesktopServices::openUrl(QUrl::fromLocalFile(outfileName));
    QDesktopServices::openUrl(QUrl::fromLocalFile(tr("%1_答案.doc").arg(outfileName.remove(".doc"))));

    QMessageBox::information(this, tr("通知"), tr("成功生成试卷！"), QMessageBox::Ok);

    close();
}

void newTestForm::onQuestionsrefresh()
{
    ui->alreadySelectQuestions->clear();
}

void newTestForm::on_deleteThisQuestion_clicked()
{
    if (!ui->alreadySelectQuestions->currentItem()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选中一个题目"), QMessageBox::Ok);
        return;
    }
    questions.removeAt(ui->alreadySelectQuestions->currentRow());
    QMessageBox::information(this, tr("通知"), tr("删除成功！"),QMessageBox::Ok);
    on_tabWidget_currentChanged(NULL);
}

void newTestForm::on_chooseMany_clicked()
{
    ui->alreadySelected->moveCursor(QTextCursor::End);
    ui->alreadySelected->insertPlainText(tr("%1:%2:%3道题\n").arg(ui->SubjectNameCB->currentText()).arg(ui->questionTypeCB->currentText()).arg(ui->questNum->text()));
}

void newTestForm::on_tabWidget_currentChanged(QWidget *arg1)
{
    ui->alreadySelectQuestions->clear();
    int i=1;
    foreach (Question que, questions) {
        ui->alreadySelectQuestions->insertItem(i++,QString("%1:%2:%3").arg(ui->SubjectNameCB->currentText()).arg(que.getQuestionType()).arg(que.getId()));
    }
}

void newTestForm::on_alreadySelectQuestions_currentRowChanged(int currentRow)
{
    if(ui->alreadySelectQuestions->currentItem()){
        QString subjectName = ui->alreadySelectQuestions->currentItem()->text().split(":").at(0);
        QString questionTypeName = ui->alreadySelectQuestions->currentItem()->text().split(":").at(1);
        int id = ui->alreadySelectQuestions->currentItem()->text().split(":").at(2).toInt();
        QSqlQuery query;

        query.exec(QString("SELECT * FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(id));

        query.next();
        QString Question = query.value(1).toString();
        QString Answer = query.value(2).toString();

        ui->answerTextBrowser_2->clear();
        ui->answerTextBrowser_2->insertHtml(Answer);
        ui->questionTextBrowser_2->clear();
        ui->questionTextBrowser_2->insertHtml(Question);
    }
}

void newTestForm::on_autoChooseQuestions_clicked()
{
    if (ui->alreadySelected->toPlainText().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("已选题目数量为空，请重新选择！"), QMessageBox::Ok);
        return ;
    }
    qsrand(QDateTime::currentMSecsSinceEpoch());
    QSqlQuery query;
    QString Content = ui->alreadySelected->toPlainText();
    QStringList rows = Content.split("\n", QString::SkipEmptyParts);
    foreach (QString row, rows) {
        QStringList element = row.split(":", QString::SkipEmptyParts);
        QString subjectName = element.at(0);
        QString questionTypeName = element.at(1);
        QString element2 = element.at(2);
        element2.remove(tr("道题"));
        int numOfQuestion = element2.toInt();
        query.exec(QString("SELECT numOfQuestions FROM '%1' WHERE questionTypes = '%2'").arg(subjectName).arg(questionTypeName));
        query.next();

        int maxNumOfQuestions = query.value(0).toInt();
        if (numOfQuestion > maxNumOfQuestions) {
            QMessageBox::warning(this, tr("警告"), tr("科目《%1》 题型“%2”没有%3道题目！").arg(subjectName).arg(questionTypeName).arg(numOfQuestion), QMessageBox::Ok);
            return;
        }

        int cnt = 0;
        int failCnt = 0;

        do{
            int id = static_cast<int>((static_cast<float> (qrand())/RAND_MAX)*maxNumOfQuestions);
            if(!query.exec(QString("SELECT * FROM '%1_%2' WHERE id = %3").arg(subjectName).arg(questionTypeName).arg(id))){
                failCnt ++;
                if (failCnt > 20) {
                    QMessageBox::warning(this, tr("警告"),tr("自动组卷失败！"), QMessageBox::Ok);
                    return;
                }
                continue;
            }
            query.next();
            QString QuestionPath = query.value(3).toString();
            if (QuestionPath.isEmpty()) {
                failCnt ++;
                if (failCnt > 20) {
                    QMessageBox::warning(this, tr("警告"),tr("自动组卷失败！"), QMessageBox::Ok);
                    return;
                }
            }
            QString AnswerPath = query.value(4).toString();
            QString Point = query.value(5).toString();
            QString Difficulty = query.value(6).toString();
            QString Degrade = query.value(7).toString();

            int maxIdx = questions.length();
            bool gotSame = false;
            for (int i = 0; i < maxIdx; ++i) {
                Question ques = questions.at(i);
                if (((ques.getQuestionType() == questionTypeName) && (ques.getId() == id)) ||(ques.getPoint() == Point)) {
                    gotSame = true;
                }
            }
            if (gotSame) {
                failCnt ++;
                if (failCnt > 20) {
                    QMessageBox::warning(this, tr("警告"),tr("自动组卷失败！"), QMessageBox::Ok);
                    return;
                }
                continue;
            }else{
                questions.append(Question(id, questionTypeName, QuestionPath, AnswerPath, Point, Degrade, Difficulty));
                cnt ++;
                failCnt = 0;
            }
        }while(cnt < numOfQuestion);
    }

    QMessageBox::about(this, tr("通知"), tr("自动组卷成功！"));
}

void newTestForm::on_clearButton_clicked()
{
    ui->alreadySelected->clear();
    ui->alreadySelectQuestions->clear();
    ui->questionTextBrowser->clear();
    ui->questionTextBrowser_2->clear();
    ui->answerTextBrowser->clear();
    ui->answerTextBrowser_2->clear();
}

void newTestForm::on_clearAlreadySelection_clicked()
{
    ui->alreadySelected->clear();
    questions.clear();
    ui->alreadySelectQuestions->clear();
    ui->questionTextBrowser->clear();
    ui->questionTextBrowser_2->clear();
    ui->answerTextBrowser->clear();
    ui->answerTextBrowser_2->clear();
}

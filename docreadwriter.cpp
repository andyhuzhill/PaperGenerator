#include "docreadwriter.h"

#include <QClipboard>
#include <QMimeData>
#include <QtGui>
#include <QList>
#include <QDir>
#include <QFile>
#include <QTextStream>

DocReadWriter::DocReadWriter(QObject *parent) :
    QObject(parent)
{
}


bool DocReadWriter::readAndConvert()
{
    QClipboard *clip = QApplication::clipboard();   //获取系统粘贴板
    inputFileName.replace("/","\\");        //获取Windows下的正确路径名

    QDir dstDir(outPath);
    dstDir.mkdir(inputFileBaseName);

    //打开输入文件
    QAxObject *inputFile = documents->querySubObject("Open(QString)", inputFileName);
    if (!inputFile) {
        return false;
    }

    //获取打开文件的一个“Range”
    QAxObject *docRange = inputFile->querySubObject("Range()");
    if (!docRange) {
        return false;
    }

    //获取“题目”书签
    QAxObject *questionBookmarks = inputFile->querySubObject("Bookmarks(QVariant)",QObject::tr("题目"));
    if (!questionBookmarks) {
        return false;
    }
    int questionBookmarksEnd = questionBookmarks->querySubObject("Range")->property("End").toInt();

    //获取“答案”书签
    QAxObject *answerBookmarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("答案"));
    if (!answerBookmarks) {
        return false;
    }
    int answerBookmarksStart = answerBookmarks->querySubObject("Range")->property("Start").toInt();
    int answerBookmarksEnd = answerBookmarks->querySubObject("Range")->property("End").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", questionBookmarksEnd, answerBookmarksStart);

    docRange->dynamicCall("Select()");

    docRange->dynamicCall("Copy()");

    QAxObject *Questdoc = documents->querySubObject("Add()");
    if (!Questdoc) {
        return false;
    }

    Questdoc->dynamicCall("Select()");

    Questdoc->querySubObject("Range()")->dynamicCall("Paste()");

    Questdoc->dynamicCall("Select()");

    Questdoc->querySubObject("Range()")->dynamicCall("Copy()");

    const QMimeData *dat = clip->mimeData();  //获取剪贴板的内容

    if (dat->hasHtml()) {
        questionHTML = dat->html();
        parserImage(questionHTML, "Question");
    }

    questionDocPath = QString("%1/%2/%3.doc").arg(outPath).arg(inputFileBaseName).arg("Question");

    Questdoc->dynamicCall("SaveAs(const QString&)", questionDocPath);
    Questdoc->dynamicCall("Close(boolean)", true);

    //获取“知识点”书签
    QAxObject *pointBookmarks  = inputFile->querySubObject("Bookmarks(QVariant)",QObject::tr("知识点"));
    if (!pointBookmarks) {
        return false;
    }

    int pointBookmarksStart = pointBookmarks->querySubObject("Range")->property("Start").toInt();
    int pointBookmarksEnd = pointBookmarks->querySubObject("Range")->property("End").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", answerBookmarksEnd, pointBookmarksStart);

    docRange->dynamicCall("Select()");

    docRange->dynamicCall("Copy()");

    QAxObject *answerdoc = documents->querySubObject("Add()");
    if (!answerdoc) {
        return false;
    }

    answerdoc->dynamicCall("Select()");

    answerdoc->querySubObject("Range()")->dynamicCall("Paste()");

    answerdoc->dynamicCall("Select()");

    answerdoc->querySubObject("Range()")->dynamicCall("Copy()");

    dat = clip->mimeData();
    if (dat->hasHtml()) {
        answerHTML = dat->html();
        parserImage(answerHTML, "Answer");
    }
    answerDocPath = QString("%1/%2/%3.doc").arg(outPath).arg(inputFileBaseName).arg("Answer");

    answerdoc->dynamicCall("SaveAs(const QString&)", answerDocPath);
    answerdoc->dynamicCall("Close(boolean)", true);

    QAxObject *degradeBookmarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("分数"));
    if (!degradeBookmarks) {
        return false;
    }
    int degradeBookmarksStart = degradeBookmarks->querySubObject("Range")->property("Start").toInt();
    int degradeBookmarksEnd = degradeBookmarks->querySubObject("Range")->property("End").toInt();

    //获取“难度”书签
    QAxObject *difficultyBookemarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("难度"));
    if (!difficultyBookemarks) {
        return false;
    }

    int difficultyBookemarksStart = difficultyBookemarks->querySubObject("Range")->property("Start").toInt();
    int difficultyBookemarksEnd = difficultyBookemarks->querySubObject("Range")->property("End").toInt();

    //获取“题目结束”书签
    QAxObject *endBookmarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("题目结束"));
    if (!endBookmarks) {
        return false;
    }
    int endBookmarksStart = endBookmarks->querySubObject("Range")->property("Start").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", pointBookmarksEnd, degradeBookmarksStart);
    point = docRange->property("Text").toString().trimmed();

    docRange->dynamicCall("setRange(QVariant, QVariant)", degradeBookmarksEnd, difficultyBookemarksStart);
    degrade = docRange->property("Text").toString().trimmed();

    docRange->dynamicCall("setRange(QVariant, QVariant)", difficultyBookemarksEnd, endBookmarksStart);

    difficulty = docRange->property("Text").toString().trimmed();

    clip->clear();      //关闭文档前 清空剪贴板

    return true;
}



QString DocReadWriter::getQuestion()
{
    return questionHTML;
}

QString DocReadWriter::getQuestionDocPath()
{
    return questionDocPath;
}

QString DocReadWriter::getAnswer()
{
    return answerHTML;
}

QString DocReadWriter::getAnswerDocPath()
{
    return answerDocPath;
}

QString DocReadWriter::getPoint()
{
    return point;
}

QString DocReadWriter::getDegrade()
{
    return degrade;
}

QString DocReadWriter::getDifficulty()
{
    return difficulty;
}

void DocReadWriter::parserImage(QString &html, QString QorA)
{
    html.replace(QString("\\"), QString("/"));
    html.remove("file:///");

    QList<int> start;
    QList<int> end;

    int startidx = 0;
    int endidx = 0;

    while((startidx = html.indexOf("<!--[if gte vml 1]>",startidx)) != -1){
        start.prepend(startidx);
        startidx ++;
        if((endidx = html.indexOf("<![endif]-->", startidx)) != -1){
            end.prepend(endidx);
        }
    }

    for (int i = 0; i < start.length(); ++i) {
        html.remove(start.at(i), end.at(i)-start.at(i)+12);
    }

    start.clear();
    startidx = 0;

    while((startidx = html.indexOf("<img", startidx)) != -1){
        start.prepend(startidx);
        startidx ++;
    }

    for (int i = 0; i < start.length(); ++i) {
        int srcStart = html.indexOf("src=\"", start.at(i))+5;
        int srcEnd = html.indexOf("\"",srcStart);
        srcEnd = html.indexOf("\"", srcEnd);
        QString imgOriginalPath = html.mid(srcStart,srcEnd-srcStart);

        QFile imgfile(imgOriginalPath);
        QFileInfo imgFileInfo(imgOriginalPath);
        QString imgFinalPath = QString("%1/%2/%3%4.%5").arg(outPath).arg(inputFileBaseName).arg(QorA).arg(i).arg(imgFileInfo.completeSuffix());
        imgfile.copy(imgFinalPath);

        html.replace(imgOriginalPath, imgFinalPath);
    }
}


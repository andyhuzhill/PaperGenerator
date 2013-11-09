#include "docreadwriter.h"

#include <QClipboard>
#include <QMimeData>
#include <QtGui>
#include <QList>

DocReadWriter::DocReadWriter(QObject *parent) :
    QObject(parent)
{
}

DocReadWriter::DocReadWriter(QObject *parent, QString sourceFile, QString DestinationPath)
{
    inputFileName = sourceFile;
    outPath = DestinationPath;
}


bool DocReadWriter::converse()
{
    QClipboard *clip = QApplication::clipboard();   //获取系统粘贴板
    inputFileName.replace("/","\\");        //获取Windows下的正确路径名

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
    int questionBookmarksStart = questionBookmarks->querySubObject("Range")->property("Start").toInt();

    //获取“答案”书签
    QAxObject *answerBookmarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("答案"));
    if (!answerBookmarks) {
        return false;
    }
    int answerBookmarksStart = answerBookmarks->querySubObject("Range")->property("Start").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", questionBookmarksStart, answerBookmarksStart);

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
        QFile out("ques.txt");
        out.open(QIODevice::WriteOnly);
        QTextStream outs(&out);
        outs << tr("\n问题如下\n");
        outs << questionHTML;
        out.close();
    }

    Questdoc->dynamicCall("SaveAs(const QString&)",QString("%1_%2_%3%4.doc").arg("outPath").arg("quesType").arg("Q").arg("1"));
    Questdoc->dynamicCall("Close(boolean)", true);


    //获取“知识点”书签
    QAxObject *pointBookmarks  = inputFile->querySubObject("Bookmarks(QVariant)",QObject::tr("知识点"));
    if (!pointBookmarks) {
        return false;
    }

    int pointBookmarksStart = pointBookmarks->querySubObject("Range")->property("Start").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", answerBookmarksStart, pointBookmarksStart);

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

        QFile out("answeold.txt");
        out.open(QIODevice::WriteOnly);
        QTextStream outs(&out);
        outs << tr("\n答案如下\n");
        outs << answerHTML;
        out.close();

        QList<int> start;
        QList<int> end;

        int startidx = 0;
        int endidx = 0;

        while((startidx = answerHTML.indexOf("<!--[if gte vml 1]>",startidx)) != -1){
            start.prepend(startidx);
            startidx ++;
            if((endidx = answerHTML.indexOf("<![endif]-->", startidx)) != -1){
                end.prepend(endidx);
            }
        }

        for (int i = 0; i < start.length(); ++i) {
            answerHTML.remove(start.at(i), end.at(i)-start.at(i)+12);
        }

        start.clear();
        end.clear();

        startidx = 0;
        endidx = 0;

        while((startidx = answerHTML.indexOf("<![if !vml]>", startidx)) != -1){
            start.prepend(startidx);
            startidx ++;
            if ((endidx = answerHTML.indexOf("<![endif]>", startidx)) != -1 ) {
                end.prepend(endidx);
            }
        }

        for (int i = 0; i < start.length(); ++i) {
            int imgStart = answerHTML.indexOf("<img", start.at(i));
            int srcStart = answerHTML.indexOf("src=", imgStart);
            int srcEnd = answerHTML.indexOf("shapes",srcStart);
            QString imgPath = answerHTML.mid(srcStart+5,srcEnd-2);
            qDebug() << "Image path are " << imgPath;
        }

//        qDebug() << answerHTML;

        QFile out2("answe.txt");
        out2.open(QIODevice::WriteOnly);
        QTextStream outs2(&out);
        outs2 << tr("\n答案如下\n");
        outs2 << answerHTML;
        out2.close();
    }

    answerdoc->dynamicCall("SaveAs(const QString&)", QString("%1_%2_%3%4.doc").arg("outPath").arg("quesType").arg("A").arg("1"));
    answerdoc->dynamicCall("Close(boolean)", true);

    //获取“难度”书签
    QAxObject *difficultyBookemarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("难度"));
    if (!difficultyBookemarks) {
        return false;
    }
    int difficultyBookemarksStart = difficultyBookemarks->querySubObject("Range")->property("Start").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", pointBookmarksStart, difficultyBookemarksStart);

    //获取“结束”书签
    QAxObject *endBookmarks = inputFile->querySubObject("Bookmarks(QVariant)", QObject::tr("结束"));
    if (!endBookmarks) {
        return false;
    }
    int endBookmarksStart = endBookmarks->querySubObject("Range")->property("Start").toInt();

    docRange->dynamicCall("setRange(QVariant, QVariant)", pointBookmarksStart, difficultyBookemarksStart);
    point = docRange->property("Text").toString().trimmed();

    docRange->dynamicCall("setRange(QVariant, QVariant)", difficultyBookemarksStart, endBookmarksStart);

    difficulty = docRange->property("Text").toString().trimmed();

    clip->clear();      //关闭文档前 清空剪贴板
    documents->dynamicCall("Close(boolean)", true);

    return true;
}

QString DocReadWriter::getQuestion()
{
    return questionHTML;
}

QString DocReadWriter::getAnswer()
{
    return answerHTML;
}

QString DocReadWriter::getPoint()
{
    return point;
}

QString DocReadWriter::getDifficulty()
{
    return difficulty;
}

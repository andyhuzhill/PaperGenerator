#ifndef DOCREADWRITER_H
#define DOCREADWRITER_H

#include <QObject>
#include <ActiveQt/QAxObject>
#include <ActiveQt/QAxWidget>
#include <QString>
#include <iostream>
#include <QFileInfo>

class DocReadWriter : public QObject
{
    Q_OBJECT
public:
    explicit DocReadWriter(QObject *parent = 0);

    explicit DocReadWriter(QObject *parent = 0, QString sourceFile = "", QString DestinationPath = "");

    void setSourceDest(QString sourceFile, QString destPath){
        inputFileName = sourceFile;
        QFileInfo srcFileInfo(inputFileName);
        inputFileBaseName = srcFileInfo.baseName();
        outPath = destPath;
    }

    void setDocuemnt(QAxObject *docs){
        documents = docs;
    }

    void setSelection(QAxObject *sel){
        selection = sel;
    }

    void setQuestionType(QString type){
        questionType = type;
    }

    bool convert();
    QString getQuestion();
    QString getQuestionDocPath();
    QString getAnswer();
    QString getAnswerDocPath();
    QString getPoint();
    QString getDifficulty();

signals:

public slots:

private:
    void parserImage(QString &html, QString type);
    QAxObject *documents;
    QAxObject *selection;
    QString inputFileName;  //输入文件名
    QString inputFileBaseName;
    QString outPath;        //输出目录
    QString questionType;   //题型
    QString questionHTML;   //问题
    QString questionDocPath;
    QString answerHTML;     //答案
    QString answerDocPath;
    QString point;          //知识点
    QString difficulty;     //难度 （1～ 10）
};

#endif // DOCREADWRITER_H

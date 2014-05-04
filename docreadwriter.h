#ifndef DOCREADWRITER_H
#define DOCREADWRITER_H

#include <QObject>
#include <ActiveQt/QAxObject>
#include <ActiveQt/QAxWidget>
#include <QString>
#include <iostream>
#include <QFileInfo>
#include "question.h"
#include <QList>


class DocReadWriter : public QObject
{
    Q_OBJECT
public:
    explicit DocReadWriter(QObject *parent = 0);

    void setSourceDest(QString sourceFile, QString destPath){
        inputFileName = sourceFile;
        QFileInfo srcFileInfo(inputFileName);
        inputFileBaseName = srcFileInfo.baseName();
        outPath = destPath;
    }

    void setDocuemnt(QAxObject *docs){
        documents = docs;
    }

    void setQuestionType(QString type){
        questionType = type;
    }

    bool readAndConvert();
    QString getQuestion();
    QString getQuestionDocPath();
    QString getAnswer();
    QString getAnswerDocPath();
    QString getPoint();
//    QString getDegrade();
    QString getDifficulty();

    void parserImage(QString &html, QString type);

signals:

public slots:

private:

    QAxObject *documents;
    QString inputFileName;  //输入文件名
    QString inputFileBaseName;
    QString outPath;        //输出目录
    QString questionType;   //题型
    QString questionHTML;   //问题
    QString questionDocPath; //问题文件路径
    QString answerHTML;     //答案
    QString answerDocPath;  //答案文件路径
    QString point;          //知识点
//    QString degrade;        //分数
    QString difficulty;     //难度 （1～ 10）
};

#endif // DOCREADWRITER_H

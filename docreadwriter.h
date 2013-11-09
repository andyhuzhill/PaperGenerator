#ifndef DOCREADWRITER_H
#define DOCREADWRITER_H

#include <QObject>
#include <ActiveQt/QAxObject>
#include <ActiveQt/QAxWidget>
#include <QString>
#include <iostream>

class DocReadWriter : public QObject
{
    Q_OBJECT
public:
    explicit DocReadWriter(QObject *parent = 0);

    explicit DocReadWriter(QObject *parent = 0, QString sourceFile = "", QString DestinationPath = "");

    void setSourceDest(QString sourceFile, QString destPath){
        inputFileName = sourceFile;
        outPath = destPath;
    }

    void setDocuemnt(QAxObject *docs){
        documents = docs;
    }

    void setSelection(QAxObject *sel){
        selection = sel;
    }

    void setQuestionType(QString type){
        quesType = type;
    }

    bool converse();
    QString getQuestion();
    QString getAnswer();
    QString getPoint();
    QString getDifficulty();

signals:

public slots:

private:
    QAxObject *documents;
    QAxObject *selection;
    QString inputFileName;  //输入文件名
    QString outPath;        //输出目录
    QString quesType;       //题型
    QString questionHTML;   //问题
    QString answerHTML;     //答案
    QString point;          //知识点
    QString difficulty;     //难度 （1～ 10）
};

#endif // DOCREADWRITER_H

#ifndef QUESTION_H
#define QUESTION_H

#include <QString>

class Question
{
public:
    Question(int id , QString QuestionType, QString QuestionPath, QString AnswerPath, QString Point, QString Degrade, QString Difficulty);
    int getId();
    QString getQuestionType();
    QString getPoint();
    QString getDegrade();
    QString getDifficulty();
    QString getQuestionPath();
    QString getAnswerPath();
private:
    int prvid;
    QString questionType;
    QString point;
    QString degrade;
    QString difficulty;
    QString questionDocPath;
    QString answerDocPath;
};

#endif // QUESTION_H

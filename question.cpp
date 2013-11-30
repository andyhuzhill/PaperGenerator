#include "question.h"

Question::Question(QString subject, int id,QString QuestionType, QString QuestionPath, QString AnswerPath, QString Point, QString Degrade, QString Difficulty)
{
    subjectName = subject;
    prvid = id;
    questionType = QuestionType;
    point = Point;
    degrade = Degrade;
    difficulty = Difficulty;
    questionDocPath = QuestionPath;
    answerDocPath = AnswerPath;
}

QString Question::getSubjectName()
{
    return subjectName;
}

int Question::getId()
{
    return prvid;
}

QString Question::getQuestionType()
{
    return questionType;
}

QString Question::getPoint()
{
    return point;
}

QString Question::getDegrade()
{
    return degrade;
}

QString Question::getDifficulty()
{
    return difficulty;
}

QString Question::getQuestionPath()
{
    return questionDocPath;
}

QString Question::getAnswerPath()
{
    return answerDocPath;
}

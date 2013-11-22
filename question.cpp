#include "question.h"

Question::Question(int id, QString QuestionType,QString QuestionPath, QString AnswerPath, QString Point, QString Degrade, QString Difficulty)
{
    prvid = id;
    questionType = QuestionType;
    point = Point;
    degrade = Degrade;
    difficulty = Difficulty;
    questionDocPath = QuestionPath;
    answerDocPath = AnswerPath;
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

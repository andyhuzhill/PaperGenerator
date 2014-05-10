#include "question.h"

Question::Question(QString subject, int id, QString QuestionType, QString QuestionPath, QString AnswerPath, QString Point, QString Difficulty, int grade)
{
    subjectName = subject;
    prvid = id;
    questionType = QuestionType;
    point = Point;
    difficulty = Difficulty;
    questionDocPath = QuestionPath;
    answerDocPath = AnswerPath;
    this->grade = grade;
}

QString Question::getSubjectName()
{
    return subjectName;
}

int Question::getGrade() const
{
    return grade;
}

void Question::setDegree(int value)
{
    grade = value;
}
QString Question::getDifficulty() const
{
    return difficulty;
}

void Question::setDifficulty(const QString &value)
{
    difficulty = value;
}
int Question::getPrvid() const
{
    return prvid;
}

void Question::setPrvid(int value)
{
    prvid = value;
}
QString Question::getPoint() const
{
    return point;
}

void Question::setPoint(const QString &value)
{
    point = value;
}
QString Question::getQuestionType() const
{
    return questionType;
}

void Question::setQuestionType(const QString &value)
{
    questionType = value;
}
QString Question::getQuestionDocPath() const
{
    return questionDocPath;
}

void Question::setQuestionDocPath(const QString &value)
{
    questionDocPath = value;
}
QString Question::getAnswerDocPath() const
{
    return answerDocPath;
}

void Question::setAnswerDocPath(const QString &value)
{
    answerDocPath = value;
}








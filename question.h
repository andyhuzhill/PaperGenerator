#ifndef QUESTION_H
#define QUESTION_H

#include <QString>

class Question
{
public:
    Question(QString subject , int id , QString QuestionType, QString QuestionPath, QString AnswerPath, QString Point, QString Difficulty, int grade);
    QString getSubjectName();

    int getPrvid() const;
    void setPrvid(int value);

    int getGrade() const;
    void setDegree(int value);

    QString getDifficulty() const;
    void setDifficulty(const QString &value);

    QString getPoint() const;
    void setPoint(const QString &value);

    QString getQuestionType() const;
    void setQuestionType(const QString &value);

    QString getQuestionDocPath() const;
    void setQuestionDocPath(const QString &value);

    QString getAnswerDocPath() const;
    void setAnswerDocPath(const QString &value);

private:
    int prvid;
    QString subjectName;
    QString questionType;
    QString point;
    int grade;
    QString difficulty;
    QString questionDocPath;
    QString answerDocPath;
};

#endif // QUESTION_H

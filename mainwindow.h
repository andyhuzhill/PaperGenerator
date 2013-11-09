#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <ActiveQt/QAxWidget>

class QAction;
class newSubjectForm;
class newTestForm;
class manageUserForm;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void newTest();
    void manageSubject();
    void manageUser();

    void about();

private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void createToolBars();


    QAction *newTestAction;
    QAction *manageSubjectAction;
    QAction *manageUserAction;
    QAction *quitAction;
    QAction *modifyAction;

    QAction *aboutAction;
    QAction *aboutQtAction;

    QMenu *fileMenu;
    QMenu *adminMenu;
    QMenu *helpMenu;

    newTestForm     *testForm;
    newSubjectForm  *subjectForm;
    manageUserForm  *userForm;

    QAxWidget *word;
};

#endif // MAINWINDOW_H

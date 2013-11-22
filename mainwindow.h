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

    void closeEvent(QCloseEvent *);
private slots:
    void newTest();
    void manageSubject();
    void manageUser();

    void about();
    void help();
    void textViewRefresh();

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

    QAction *helpAction;
//    QAction *aboutAction;
    QAction *aboutQtAction;

    QMenu *fileMenu;
    QMenu *adminMenu;
    QMenu *helpMenu;

    newTestForm     *testForm;
    newSubjectForm  *subjectForm;
    manageUserForm  *userForm;

    QAxWidget *word;
    QString paperPath;
};

#endif // MAINWINDOW_H

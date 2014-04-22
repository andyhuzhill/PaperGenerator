#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <ActiveQt/QAxWidget>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class QAction;
class newTestForm;
class manageUserForm;
class SubjectManager;
class QListWidgetItem;

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
    void focusInEvent(QFocusEvent *);
private slots:
    void newTest();
    void manageSubject();
    void manageUser();

    void about();
    void help();
    void textViewRefresh();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_deleteSelectPaper_clicked();

    void onCheckUpdate();

private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void createToolBars();

    void checkUpdate();

    void showUpdate(QString version);


    QAction *newTestAction;
    QAction *manageSubjectAction;
    QAction *manageUserAction;
    QAction *quitAction;
    QAction *modifyAction;

    QAction *helpAction;
    QAction *aboutQtAction;

    QMenu *fileMenu;
    QMenu *adminMenu;
    QMenu *helpMenu;

    newTestForm     *testForm;
    manageUserForm  *userForm;
    SubjectManager  *subjectManager;

    QAxWidget *word;
    QString paperPath;

    QNetworkAccessManager *manager;
    QNetworkReply         *reply;
};

#endif // MAINWINDOW_H

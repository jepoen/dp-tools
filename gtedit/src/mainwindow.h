#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include "pattern.h"

class QAction;
class QActionGroup;
class QLabel;
class QLineEdit;
class QScrollArea;

class LineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QAction *openAction;
    QAction *quitAction;
    QAction *scale100Action;
    QAction *scale50Action;
    QScrollArea *mainWidget;
    QActionGroup *editActions;
    QLabel *lCurrentChar;
    QLabel *lGtLines;

    Pattern *myDict;
    QList<LineEdit*> myLineEdits;
    QString myDir;
    double myScale;

    void createActions();
    void createMenus();
    void createMainWidget();
    void createStatusBar();
    void showPage(const QString& dirName);
    void readDict();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void openPage();
    void handleLine(QAction *);
    void getCurrentChar();
    void countGtLines();
    void setScale();
};
#endif // MAINWINDOW_H

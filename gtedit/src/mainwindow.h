#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

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

    QList<LineEdit*> myLineEdits;
    QString myDir;
    double myScale;

    void createActions();
    void createMenus();
    void createMainWidget();
    void createStatusBar();
    void showPage(const QString& dirName);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void openPage();
    void handleLine(QAction *);
    void getCurrentChar(int /*old*/, int pos);
    void countGtLines();
    void setScale();
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

class QAction;
class QActionGroup;
class QLineEdit;
class QScrollArea;

class LineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QAction *openAction;
    QScrollArea *mainWidget;
    QActionGroup *editActions;

    QList<LineEdit*> myLineEdits;

    void createActions();
    void createMenus();
    void createMainWidget();
    void showPage(const QString& dirName);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void openPage();
    void handleLine(QAction *);
};
#endif // MAINWINDOW_H

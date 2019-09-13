#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QListWidget;
class QListWidgetItem;
class Page;
class LineboxEdit;
class Dictionary;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Dictionary *myDict;
    QListWidget *myFilesWidget;
    LineboxEdit *myEdit;
    Page *myPage;
    QAction *openAction;
    QAction *saveAction;
    QString myCurrentDir;
    QString myCurrentFile;
    void createActions();
    void createMenu();
    void showPage(const QString& baseName);
    void readDir(const QString& dirName);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void open();
    void save();
private slots:
    void openFileItem(QListWidgetItem *);
    void changeEditPos();
};
#endif // MAINWINDOW_H

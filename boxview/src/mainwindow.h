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
    QAction *updateDistAction;
    QString myCurrentDir;
    QString myCurrentFile;
    QString mytesseractDir;
    void createActions();
    void createMenu();
    void showPage(const QString& baseName);
    void readDir(const QString& dirName);
    void loadSettings();
    void saveSettings();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);
public slots:
    void open();
    void save();
private slots:
    void openFileItem(QListWidgetItem *);
    void changeEditPos();
};
#endif // MAINWINDOW_H

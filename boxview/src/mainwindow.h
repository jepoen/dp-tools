#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QListWidget;
class QListWidgetItem;
class Page;
class LineboxEdit;
class Dictionary;
class Tesseract;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Dictionary *myDict;
    QListWidget *myFilesWidget;
    LineboxEdit *myEdit;
    Page *myPage;
    Tesseract *tesseract;
    QAction *openAction;
    QAction *saveAction;
    QAction *updateDistAction;
    QAction *deleteLineAction;
    QAction *replaceFromProofedAction;
    QString myCurrentDir;
    QString myCurrentFile;
    QString myTesseractPath;
    QString myTesseractModel;
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

#include <QDir>
#include <QtDebug>
#include <QtWidgets>
#include "lineseditor.h"

LinesEditor::LinesEditor() {
    QFont font("DPCustomMono2", 14);
    dir = QDir("testDir");
    fileNames = dir.entryList(QStringList()<<"line-???.png", QDir::Files, QDir::Name);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
    QVBoxLayout *layout = new QVBoxLayout();
    QScrollArea *scroller = new QScrollArea();
    QWidget *dlgWidget = new QWidget();
    dlgWidget->setLayout(layout);
    for (QString f: fileNames) {
        QString baseName = QFileInfo(f).baseName();
        qDebug()<<f<<QFileInfo(f).baseName();
        QFileInfo gtFile(dir, baseName+".gt.txt");
        QFileInfo predFile(dir, baseName+".pred.txt");
        //qDebug()<<"gt"<<gtFile<<"pred"<<predFile;
        QFile file;
        if (gtFile.exists()) {
            qDebug()<<gtFile;
            file.setFileName(gtFile.absoluteFilePath());
        }
        if (predFile.exists()) {
            qDebug()<<predFile;
            file.setFileName(predFile.absoluteFilePath());
        }
        if (!file.fileName().isEmpty()) {
            QLabel *lbaseName = new QLabel(baseName);
            layout->addWidget(lbaseName);
            QFileInfo imgFile = QFileInfo(dir, f);
            qDebug()<<imgFile.absoluteFilePath();
            QLabel *label = new QLabel();
            label->setPixmap(QPixmap(imgFile.absoluteFilePath()));
            layout->addWidget(label);
            QLineEdit *lineEdit = new QLineEdit();
            lineEdit->setFont(font);
            file.open(QFile::ReadOnly|QFile::Text);
            lineEdit->setText(QString(file.readAll()));
            layout->addWidget(lineEdit);
            lines.append(lineEdit);
        }
    }
    scroller->setWidget(dlgWidget);
    mainLayout->addWidget(scroller);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);
    mainLayout->addWidget(box);
    resize(1000, 800);
}

#ifndef PAGE_H
#define PAGE_H

#include <QRect>
#include <QLabel>

class Page : public QWidget
{
    Q_OBJECT
private:
    QRect myLineBox;
    QPixmap myPage;
public:
    Page();
    void setLineBox(const QRect& linebox) { myLineBox = linebox; repaint();}
    void setPage(const QPixmap& page) { myPage = page; myLineBox = QRect(); repaint();}
protected:
    void paintEvent(QPaintEvent *event) override;
public slots:
    void changeLineBox(const QRect& box);
};

#endif // PAGE_H

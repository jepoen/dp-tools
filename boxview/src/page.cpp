#include <QtWidgets>
#include <QtDebug>
#include "page.h"

Page::Page()
{
    resize(600, 800);
}

void Page::changeLineBox(const QRect& rect) {
    myLineBox = QRect(rect.x(), myPage.height()-rect.height(), rect.width()-rect.x(), rect.height()-rect.y());
    qDebug()<<"changeLinebox"<<myLineBox;
    repaint();
}

void Page::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    double scaleX = double(this->width())/myPage.width();
    double scaleY = double(this->height())/myPage.height();
    double scale = scaleX < scaleY? scaleX: scaleY;
    painter.scale(scale, scale);
    QPen pen(Qt::red, 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.begin(this);
    painter.drawPixmap(0, 0, myPage);
    if (!myLineBox.isNull()) {
        painter.drawRect(myLineBox.x(), myLineBox.y(), myLineBox.width(), myLineBox.height());
    }
    painter.end();
}

#include "CrossButton.h"
#include <QPen>

CrossButton::CrossButton(QWidget *parent) : QPushButton(parent)
{
    isWrong = false;
}
CrossButton::CrossButton(const QString& text, QWidget *parent) : QPushButton(parent)
{
    isWrong = false;
    this->setText(text);
}

void CrossButton::setCrossStatus(bool cross)
{
    isWrong = cross;
}

void CrossButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    if (isWrong)
    {
        QPainter painter(this);
        QPen pen(Qt::red, 2, Qt::SolidLine);
        painter.setPen(pen);
        painter.drawLine(5, 5, this->width() - 5, this->height() - 5);
        painter.drawLine(5, this->height() - 5, this->width() - 5, 5);
    }
}

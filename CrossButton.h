#ifndef CROSSBUTTON_H
#define CROSSBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QPainter>

class CrossButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CrossButton(QWidget *parent = 0);
    explicit CrossButton(const QString &, QWidget *parent = 0);
    void setCrossStatus(bool);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool isWrong;
};

#endif // CROSSBUTTON_H

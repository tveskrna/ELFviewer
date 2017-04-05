#ifndef MY_QLABEL_H
#define MY_QLABEL_H

#include <QWidget>
#include <QMouseEvent>
#include <QEvent>

class my_qlabel : public QWidget
{
    Q_OBJECT
public:
    explicit my_qlabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *e);

    int x,y;

signals:
    void mouse_pressed();

public slots:
};

#endif // MY_QLABEL_H

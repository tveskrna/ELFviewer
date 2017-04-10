#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>

#include <QPoint>
#include <QBrush>
#include <QPen>
#include <string>
#include <QString>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent = 0);

signals:
    void clickedOnMe(QPointF pt);

public slots:
    void mousePressEvent(QMouseEvent * e);
    void changeSize(int width, int height);
    void addRectangle(QString name, int type, int sectionNumb);
    void drawLine(QPointF from, QPointF to);
    void clearScene();

    // void mouseReleaseEvent(QMouseEvent * e);
    // void mouseDoubleClickEvent(QMouseEvent * e);
    // void mouseMoveEvent(QMouseEvent * e);
private:
    QGraphicsScene * scene;
};
#endif // MYGRAPHICVIEW_H

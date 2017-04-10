#include "myqgraphicsview.h"

MyQGraphicsView::MyQGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    this->setSceneRect(0, 0, 150, 451);
    this->setScene(scene);
}

void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    double rad = 10;
    QPointF pt = mapToScene(e->pos());
    /*scene->addEllipse(pt.x()-rad, pt.y()-rad, rad*2.0, rad*2.0,
        QPen(), QBrush(Qt::SolidPattern));*/
    emit clickedOnMe(pt);
}

void MyQGraphicsView::changeSize(int width, int height)
{
    if (this->scene->height() <= height)
    {
        this->setSceneRect(0, 0, width-20, height-50);
        this->setScene(scene);
    }
}

void MyQGraphicsView::clearScene()
{
    this->scene->clear();
}

void MyQGraphicsView::addRectangle(QString name, int type, int sectionNumb)
{
    QBrush *brush = new QBrush();
    if (type == 0)  //header table
    {
        //qDebug() << "type == 0";
        brush->setColor(QColor(255, 0, 0, 255));
    }
    else if (type == 1)   //segment
    {
        brush->setColor(QColor(255, 100, 0, 255));
    }
    else  //section
    {
        brush->setColor(QColor(255, 204, 0, 255));
    }


    QGraphicsRectItem * rect = new QGraphicsRectItem;
    rect->setPen(QColor("black"));
    rect->setRect(50 + type * 20, sectionNumb * 35 + 10, 200, 25);
    rect->setBrush(brush->color());

    int posOfText = 100 - name.size() * 4;

    QGraphicsTextItem * txt = new QGraphicsTextItem;
    txt->setFont(QFont("Arial", 10, QFont::Bold));
    txt->setPos(50 + type * 20 + posOfText, sectionNumb * 35 + 10);
    txt->setPlainText(name);

    scene->addItem(rect);
    scene->addItem(txt);
}

void MyQGraphicsView::drawLine(QPointF from, QPointF to)
{

}

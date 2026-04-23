#include "parallelogram.h"

#include <cmath>

Parallelogram::Parallelogram(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Parallelogram::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    polygon_.clear();
    
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();

    
    qreal left = qMin(start.x(), end.x());
    qreal right = qMax(start.x(), end.x());
    qreal top = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());
    qreal offset = (end.x() - start.x()) * 0.3; 

    
    polygon_ << QPointF(left, top)                  
            << QPointF(right, top)                 
            << QPointF(right + offset, bottom)     
            << QPointF(left + offset, bottom);     

    painter->drawPolygon(polygon_);
}

QPainterPath Parallelogram::shape() const
{
    QPainterPath painter;
    painter.addPolygon(polygon_);
    return painter;
}

qreal Parallelogram::getPerimeter()
{
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    qreal left = qMin(start.x(), end.x());
    qreal right = qMax(start.x(), end.x());
    qreal top = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());
    qreal offset = (end.x() - start.x()) * 0.3;
    QPolygonF poly;
    poly << QPointF(left, top)
         << QPointF(right, top)
         << QPointF(right + offset, bottom)
         << QPointF(left + offset, bottom);

    qreal perimeter = 0;
    for (int i = 0; i < poly.size() - 1; i++) {
        perimeter += std::sqrt(std::pow(poly.at(i + 1).x() - poly.at(i).x(), 2)
                               + std::pow(poly.at(i + 1).y() - poly.at(i).y(), 2));
    }
    if (poly.size() > 1) {
        perimeter += std::sqrt(std::pow(poly.at(poly.size() - 1).x() - poly.at(0).x(), 2)
                               + std::pow(poly.at(poly.size() - 1).y() - poly.at(0).y(), 2));
    }
    return perimeter;
}

qreal Parallelogram::getSquare()
{
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    qreal left = qMin(start.x(), end.x());
    qreal right = qMax(start.x(), end.x());
    qreal top = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());
    qreal offset = (end.x() - start.x()) * 0.3;
    QPolygonF poly;
    poly << QPointF(left, top)
         << QPointF(right, top)
         << QPointF(right + offset, bottom)
         << QPointF(left + offset, bottom);

    qreal sum = 0;
    int n = poly.size();
    if (n > 1)
    {
        for (int i = 0; i < n - 1; i++)
        {
            sum += poly.at(i).x() * poly.at(i + 1).y();
            sum -= poly.at(i + 1).x() * poly.at(i).y();
        }
        sum += poly.at(n - 1).x() * poly.at(0).y();
        sum -= poly.at(0).x() * poly.at(n - 1).y();
    }
    return 0.5 * qAbs(sum);
}


Figure* Parallelogram::clone() const
{
    Parallelogram* copy = new Parallelogram(getStartPoint());
    copy->setEndPoint(getEndPoint());
    copy->setPenColor(getPenColor());
    copy->setBrushColor(getBrushColor());
    copy->setPenWidth(getPenWidth());
    copy->setPos(pos());
    copy->setRotation(rotation());
    copy->setTransformOriginPoint(transformOriginPoint());
    copy->setLayer(layer_);
    return copy;
}
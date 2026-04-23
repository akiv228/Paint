#include "trapezoidex.h"
#include <cmath>
#include <QtGlobal>  

TrapezoidEx::TrapezoidEx(QPointF point, QObject *parent) : Figure(point, parent) {}

void TrapezoidEx::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    polygon_.clear();
    
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    
    qreal dx = qAbs(end.x() - start.x());
    int sign = (end.x() < start.x()) ? -1 : 1;
    
    polygon_ << QPointF(start.x() + sign * dx / 4, start.y())
             << QPointF(end.x()   - sign * dx / 4, start.y())
             << end
             << QPointF(start.x(), end.y());
             
    painter->drawPolygon(polygon_);
}

QPainterPath TrapezoidEx::shape() const
{
    QPainterPath path;
    path.addPolygon(polygon_);
    return path;
}

qreal TrapezoidEx::getPerimeter()
{
    qreal p = 0;
    for (int i = 0; i < polygon_.size() - 1; ++i)
        p += std::hypot(polygon_[i+1].x() - polygon_[i].x(), polygon_[i+1].y() - polygon_[i].y());
    p += std::hypot(polygon_.last().x() - polygon_.first().x(), polygon_.last().y() - polygon_.first().y());
    return p;
}

qreal TrapezoidEx::getSquare()
{
    qreal sum = 0;
    int n = polygon_.size();
    for (int i = 0; i < n - 1; ++i)
        sum += polygon_[i].x() * polygon_[i+1].y() - polygon_[i+1].x() * polygon_[i].y();
    sum += polygon_.last().x() * polygon_.first().y() - polygon_.first().x() * polygon_.last().y();
    return 0.5 * qAbs(sum);
}

Figure* TrapezoidEx::clone() const
{
    TrapezoidEx* copy = new TrapezoidEx(getStartPoint());
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
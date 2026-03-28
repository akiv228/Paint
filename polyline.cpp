#include "polyline.h"
#include <cmath>

Polyline::Polyline(QPointF point, QObject *parent)
    : Figure{point, parent}
{
    points_ << point;
    updateTransformOriginPoint();
}

void Polyline::addPoint(const QPointF &point)
{
    points_ << point;
    prepareGeometryChange();
    update();
    updateTransformOriginPoint();
}

void Polyline::setTempPoint(const QPointF &point)
{
    tempPoint_ = point;
    update();
}

void Polyline::clearTemp()
{
    tempPoint_ = QPointF();
    update();
}

void Polyline::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painterInit(painter);
    if (points_.size() >= 2)
        painter->drawPolyline(points_);
    if (isBuilding_ && !tempPoint_.isNull() && !points_.isEmpty())
        painter->drawLine(points_.last(), tempPoint_);
}

QRectF Polyline::boundingRect() const
{
    if (points_.isEmpty())
        return QRectF();
    QRectF rect = QRectF(points_.first(), points_.first());
    for (const QPointF &p : points_)
        rect = rect.united(QRectF(p, p));
    if (isBuilding_ && !tempPoint_.isNull())
        rect = rect.united(QRectF(tempPoint_, tempPoint_));
    return rect;
}

QPainterPath Polyline::shape() const
{
    QPainterPath path;
    if (points_.size() >= 2)
    {
        path.moveTo(points_.first());
        for (int i = 1; i < points_.size(); ++i)
            path.lineTo(points_.at(i));
    }
    return path;
}

qreal Polyline::getPerimeter()
{
    qreal perimeter = 0.0;
    for (int i = 0; i < points_.size() - 1; ++i)
    {
        QLineF seg(points_.at(i), points_.at(i + 1));
        perimeter += seg.length();
    }
    return perimeter;
}

qreal Polyline::getSquare()
{
    return 0.0;
}

FigureType Polyline::getFigureType()
{
    return kPolyline;
}

void Polyline::setPoints(const QPolygonF &points)
{
    points_ = points;
    prepareGeometryChange();
    update();
    updateTransformOriginPoint(); 
}


Figure* Polyline::clone() const
{
    Polyline* copy = new Polyline(getStartPoint());
    copy->setPoints(points_);
    copy->setPenColor(getPenColor());
    copy->setBrushColor(getBrushColor());
    copy->setPenWidth(getPenWidth());
    copy->setPos(pos());
    copy->setRotation(rotation());
    copy->setTransformOriginPoint(transformOriginPoint());
    return copy;
}
// polyline.h (не меняется, только при необходимости добавить метод)
// polyline.cpp

#include "polyline.h"
#include <cmath>
#include <QPainterPathStroker>

Polyline::Polyline(QPointF point, QObject *parent)
    : Figure{QPointF(), parent} // позиция будет (0,0)
{
    setPos(point);               // фигура находится в точке point
    points_ << QPointF(0,0);     // первая точка локально (0,0)
    updateTransformOriginPoint();
}

void Polyline::addPoint(const QPointF &point)
{
    // Преобразуем глобальные координаты в локальные (относительно позиции фигуры)
    points_ << mapFromScene(point);
    prepareGeometryChange();
    update();
    updateTransformOriginPoint();
}

void Polyline::setTempPoint(const QPointF &point)
{
    tempPoint_ = mapFromScene(point);
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

    // Добавляем запас для выделения (половина ширины пера + 5 пикселей)
    qreal extra = (getPenWidth() + 5) / 2.0;
    return rect.normalized().adjusted(-extra, -extra, extra, extra);
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
    if (path.isEmpty())
        return path;
    QPainterPathStroker stroker;
    stroker.setWidth(getPenWidth() + 10); // увеличили запас
    QPainterPath stroked = stroker.createStroke(path);
    return stroked;
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
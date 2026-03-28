// line.cpp
#include "line.h"
#include <cmath>

Line::Line(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    painter->drawLine(getStartPoint(), getEndPoint());
}

QPainterPath Line::shape() const
{
    QPainterPath path;
    path.moveTo(getStartPoint());
    path.lineTo(getEndPoint());
    return path;
}

qreal Line::getPerimeter()
{
    return std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) +
                     std::pow(getEndPoint().y() - getStartPoint().y(), 2));
}

qreal Line::getSquare()
{
    return 0.0;
}

FigureType Line::getFigureType()
{
    return kLine;
}

Figure* Line::clone() const
{
    Line* copy = new Line(getStartPoint());
    copy->setEndPoint(getEndPoint());
    copy->setPenColor(getPenColor());
    copy->setBrushColor(getBrushColor());
    copy->setPenWidth(getPenWidth());
    copy->setPos(pos());
    copy->setRotation(rotation());
    copy->setTransformOriginPoint(transformOriginPoint());
    return copy;
}
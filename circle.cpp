#include "circle.h"
#include "QDebug"
#include <cmath>

Circle::Circle(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Circle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    // a = qAbs(getEndPoint().x() - getStartPoint().x());
    // b = qAbs(getEndPoint().y() - getStartPoint().y());
    radius_ = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    // painter->drawEllipse(mapToScene(getStartPoint()), radius, radius);
    painter->drawEllipse(getStartPoint(), radius_, radius_);
}

QPainterPath Circle::shape() const
{
    QPainterPath painter;
    painter.addEllipse(getStartPoint(), radius_, radius_);
    return painter;
}

QRectF Circle::boundingRect() const
{
    return QRectF(QPointF(getStartPoint().x() - radius_, getStartPoint().y() - radius_), QPointF(getStartPoint().x() + radius_, getStartPoint().y() + radius_));
}

qreal Circle::getRadius()
{
    return radius_;
}

qreal Circle::getPerimeter()
{
    return 2 * M_PI * radius_;
}

qreal Circle::getSquare()
{
    return M_PI * radius_ * radius_;
}

FigureType Circle::getFigureType()
{
    return kCircle;
}

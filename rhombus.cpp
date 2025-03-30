#include "rhombus.h"

#include <cmath>

Rhombus::Rhombus(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Rhombus::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    polygon_.clear();
    polygon_ << QPointF(getStartPoint().x() + (getEndPoint().x() > getStartPoint().x() ? 1 : -1) * qAbs(getStartPoint().x() - getEndPoint().x()) / 2, getStartPoint().y())
            << QPointF(getEndPoint().x(), getStartPoint().y() + (getEndPoint().y() > getStartPoint().y() ? 1 : -1) * qAbs(getEndPoint().y() - getStartPoint().y()) / 2)
            << QPointF(getStartPoint().x() + (getEndPoint().x() > getStartPoint().x() ? 1 : -1) * qAbs(getStartPoint().x() - getEndPoint().x()) / 2, getEndPoint().y())
            << QPointF(getStartPoint().x(), getStartPoint().y() + (getEndPoint().y() > getStartPoint().y() ? 1 : -1) * qAbs(getEndPoint().y() - getStartPoint().y()) / 2);
    painter->drawPolygon(polygon_);
}

QPainterPath Rhombus::shape() const
{
    QPainterPath painter;
    painter.addPolygon(polygon_);
    return painter;
}

qreal Rhombus::getPerimeter()
{
    qreal perimeter = 0;
    for (int i = 0; i < polygon_.size() - 1; i++) {
        perimeter += std::sqrt(std::pow(polygon_.at(i + 1).x() - polygon_.at(i).x(), 2)
                               + std::pow(polygon_.at(i + 1).y() - polygon_.at(i).y(), 2));
    }
    if (polygon_.size() > 1) {
        perimeter += std::sqrt(
            std::pow(polygon_.at(polygon_.size() - 1).x() - polygon_.at(0).x(), 2)
            + std::pow(polygon_.at(polygon_.size() - 1).y() - polygon_.at(0).y(), 2));
    }
    return perimeter;
}

qreal Rhombus::getSquare()
{
    qreal sum = 0;
    int n = polygon_.size();
    if (n > 1)
    {
        for (int i = 0; i < n - 1; i++)
        {
            sum += polygon_.at(i).x() * polygon_.at(i + 1).y();
            sum -= polygon_.at(i + 1).x() * polygon_.at(i).y();
        }
        sum += polygon_.at(n - 1).x() * polygon_.at(0).y();
        sum -= polygon_.at(0).x() * polygon_.at(n - 1).y();
    }
    return 0.5 * qAbs(sum);
}

FigureType Rhombus::getFigureType()
{
    return kRhombus;
}

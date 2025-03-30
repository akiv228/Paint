#include "triangle.h"

#include <cmath>

Triangle::Triangle(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Triangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    polygon_.clear();
    polygon_ << QPointF(getStartPoint().x() + (getEndPoint().x() > getStartPoint().x() ? 1 : -1) * qAbs(getEndPoint().x() - getStartPoint().x()) / 2, getStartPoint().y())
            << QPointF(getEndPoint().x() > getStartPoint().x() ? getEndPoint().x() : getStartPoint().x(), getEndPoint().y())
            << QPointF(getEndPoint().x() > getStartPoint().x() ? getStartPoint().x() : getEndPoint().x(), getEndPoint().y());
    painter->drawPolygon(polygon_);
}

QPainterPath Triangle::shape() const
{
    QPainterPath painter;
    painter.addPolygon(polygon_);
    return painter;
}

qreal Triangle::getPerimeter()
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

qreal Triangle::getSquare()
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

FigureType Triangle::getFigureType()
{
    return kTriangle;
}

//среднее арифметическое из координат его вершин: xc =1/3(x1+x2+x3) ; yc =1/3(y1+y2+y3).
QPointF Triangle::getCenterOfMass() const
{
    if (polygon_.size() == 3) {
        qreal x = (polygon_[0].x() + polygon_[1].x() + polygon_[2].x()) / 3.0;
        qreal y = (polygon_[0].y() + polygon_[1].y() + polygon_[2].y()) / 3.0;
        return QPointF(x, y); //в локальных координатах
    }
    return boundingRect().center();
}

#include "polygon.h"
#include <cmath>

Polygon::Polygon(QPointF point, QObject *parent)
    : Figure{point, parent}
{
    currentPointsCount_ = points_count_;
}

Polygon::Polygon(QPointF point, int currentPointsCount, QObject *parent)
    : Figure{point, parent}
{
    this->currentPointsCount_ = currentPointsCount;
}

void Polygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    radius_ = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    qreal step = 2 * M_PI / currentPointsCount_;
    polygon_.clear();
    for (int i = 0; i < currentPointsCount_; i++) {
        polygon_ << QPointF(getStartPoint().x() + std::cos(i * step - M_PI_2) * radius_,
                           getStartPoint().y() + std::sin(i * step - M_PI_2) * radius_);
    }
    painter->drawPolygon(polygon_);
}

QRectF Polygon::boundingRect() const
{
    qreal r = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + 
                        std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    return QRectF(QPointF(getStartPoint().x() - r, getStartPoint().y() - r),
                  QPointF(getStartPoint().x() + r, getStartPoint().y() + r));
}

QPainterPath Polygon::shape() const
{
    qreal r = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + 
                        std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    qreal step = 2 * M_PI / currentPointsCount_;
    QPolygonF polygon;
    for (int i = 0; i < currentPointsCount_; i++) {
        polygon << QPointF(getStartPoint().x() + std::cos(i * step - M_PI_2) * r,
                           getStartPoint().y() + std::sin(i * step - M_PI_2) * r);
    }
    QPainterPath painter;
    painter.addPolygon(polygon);
    return painter;
}

int Polygon::points_count_ = 6;

void Polygon::setPointsCount(int count)
{
    points_count_ = count;
}

int Polygon::getPointsCount()
{
    return points_count_;
}

qreal Polygon::getRadius()
{
    return radius_;
}

qreal Polygon::getPerimeter()
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

qreal Polygon::getSquare()
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

FigureType Polygon::getFigureType()
{
    return FigureType::kPolygon;
}


Figure* Polygon::clone() const
{
    Polygon* copy = new Polygon(getStartPoint(), currentPointsCount_);
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
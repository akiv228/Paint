#include "star.h"
#include <cmath>
#include <QJsonObject>

Star::Star(QPointF point, QObject *parent)
    : Figure{point, parent}
{
    currentPointsCount_ = points_count_;
}

Star::Star(QPointF point, int currentPointsCount, QObject *parent)
    : Figure{point, parent}
{
    this->currentPointsCount_ = currentPointsCount;
}

void Star::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    radius_ = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    qreal step = M_PI / currentPointsCount_;
    polygon_.clear();
    for (int i = 0; i < currentPointsCount_ * 2; i++) {
        polygon_ << QPointF(getStartPoint().x()
                               + std::cos(i * step - M_PI_2) * radius_ * (((i % 2) != 0) ? 0.5 : 1),
                           getStartPoint().y()
                               + std::sin(i * step - M_PI_2) * radius_ * (((i % 2) != 0) ? 0.5 : 1));
    }
    painter->drawPolygon(polygon_);
}

QPainterPath Star::shape() const
{
    qreal r = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + 
                        std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    qreal step = M_PI / currentPointsCount_;
    QPolygonF polygon;
    for (int i = 0; i < currentPointsCount_ * 2; i++) {
        polygon << QPointF(getStartPoint().x()
                               + std::cos(i * step - M_PI_2) * r * (((i % 2) != 0) ? 0.5 : 1),
                           getStartPoint().y()
                               + std::sin(i * step - M_PI_2) * r * (((i % 2) != 0) ? 0.5 : 1));
    }
    QPainterPath painter;
    painter.addPolygon(polygon);
    return painter;
}

int Star::points_count_ = 5;

void Star::setPointsCount(int count)
{
    points_count_ = count;
}

int Star::getPointsCount()
{
    return points_count_;
}

QRectF Star::boundingRect() const
{
    qreal r = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + 
                        std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    return QRectF(QPointF(getStartPoint().x() - r, getStartPoint().y() - r),
                  QPointF(getStartPoint().x() + r, getStartPoint().y() + r));
}

qreal Star::getRadius()
{
    return radius_;
}

qreal Star::getPerimeter()
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

qreal Star::getSquare()
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

Figure* Star::clone() const
{
    Star* copy = new Star(getStartPoint(), currentPointsCount_);
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


QJsonObject Star::toJson() const {
    QJsonObject obj = Figure::toJson();
    obj["pointsCount"] = currentPointsCount_;
    return obj;
}

void Star::fromJson(const QJsonObject &json) {
    Figure::fromJson(json);
    if (json.contains("pointsCount"))
        currentPointsCount_ = json["pointsCount"].toInt();
}
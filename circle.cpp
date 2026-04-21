#include "circle.h"
#include "QDebug"
#include <cmath>

Circle::Circle(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Circle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    
    
    radius_ = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    
    painter->drawEllipse(getStartPoint(), radius_, radius_);
}

QPainterPath Circle::shape() const
{
    qreal r = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + 
                        std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    QPainterPath painter;
    painter.addEllipse(getStartPoint(), r, r);
    return painter;
}

QRectF Circle::boundingRect() const
{
    qreal r = std::sqrt(std::pow(getEndPoint().x() - getStartPoint().x(), 2) + 
                        std::pow(getEndPoint().y() - getStartPoint().y(), 2));
    return QRectF(QPointF(getStartPoint().x() - r, getStartPoint().y() - r),
                  QPointF(getStartPoint().x() + r, getStartPoint().y() + r));
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

FigureType Circle::getFigureType() const
{
    return kCircle;
}


Figure* Circle::clone() const
{
    Circle* copy = new Circle(getStartPoint());
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

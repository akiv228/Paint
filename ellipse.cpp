#include "ellipse.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ellipse::Ellipse(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

void Ellipse::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painterInit(painter);
    QRectF rect(getStartPoint(), getEndPoint());
    painter->drawEllipse(rect);
}

QRectF Ellipse::boundingRect() const
{
    return QRectF(getStartPoint(), getEndPoint()).normalized();
}

QPainterPath Ellipse::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

qreal Ellipse::getPerimeter()
{
    qreal a = std::abs(getEndPoint().x() - getStartPoint().x()) / 2.0;
    qreal b = std::abs(getEndPoint().y() - getStartPoint().y()) / 2.0;
    if (a <= 0 || b <= 0) return 0;
    qreal h = (a - b) * (a - b) / ((a + b) * (a + b));
    return M_PI * (a + b) * (1 + 3 * h / (10 + std::sqrt(4 - 3 * h)));
}

qreal Ellipse::getSquare()
{
    qreal a = std::abs(getEndPoint().x() - getStartPoint().x()) / 2.0;
    qreal b = std::abs(getEndPoint().y() - getStartPoint().y()) / 2.0;
    return M_PI * a * b;
}


Figure* Ellipse::clone() const
{
    Ellipse* copy = new Ellipse(getStartPoint());
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
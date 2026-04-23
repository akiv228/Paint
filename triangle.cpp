#include "triangle.h"
#include <QLineF>
#include <cmath>

Triangle::Triangle(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

QPolygonF Triangle::getPolygon() const
{
    QPolygonF poly;
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();

    
    qreal left   = qMin(start.x(), end.x());
    qreal right  = qMax(start.x(), end.x());
    qreal top    = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());

    
    if (qFuzzyCompare(top, bottom)) {
        bottom = top + 1.0;
    }
    if (qFuzzyCompare(left, right)) {
        right = left + 1.0;
    }

    
    poly << QPointF((left + right) / 2.0, top)   
         << QPointF(right, bottom)                
         << QPointF(left, bottom);                

    return poly;
}

void Triangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painterInit(painter);
    polygon_ = getPolygon();
    painter->drawPolygon(polygon_);
}

QPainterPath Triangle::shape() const
{
    QPainterPath path;
    path.addPolygon(getPolygon());
    
    
    QPainterPathStroker stroker;
    stroker.setWidth(2.0);
    QPainterPath strokedPath = stroker.createStroke(path);
    return strokedPath.united(path);
}

qreal Triangle::getPerimeter()
{
    qreal perimeter = 0;
    polygon_ = getPolygon();
    int n = polygon_.size();
    
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            perimeter += QLineF(polygon_.at(i), polygon_.at(i + 1)).length();
        }
        perimeter += QLineF(polygon_.at(n - 1), polygon_.at(0)).length();
    }
    
    return perimeter;
}

qreal Triangle::getSquare()
{
    polygon_ = getPolygon();
    qreal sum = 0;
    int n = polygon_.size();
    
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            sum += polygon_.at(i).x() * polygon_.at(i + 1).y();
            sum -= polygon_.at(i + 1).x() * polygon_.at(i).y();
        }
        sum += polygon_.at(n - 1).x() * polygon_.at(0).y();
        sum -= polygon_.at(0).x() * polygon_.at(n - 1).y();
    }
    
    return 0.5 * qAbs(sum);
}

QPointF Triangle::getCenterOfMass() const
{
    QPolygonF poly = getPolygon();
    if (poly.size() == 3) {
        
        qreal x = (poly[0].x() + poly[1].x() + poly[2].x()) / 3.0;
        qreal y = (poly[0].y() + poly[1].y() + poly[2].y()) / 3.0;
        return QPointF(x, y);
    }
    return boundingRect().center();
}

Figure* Triangle::clone() const
{
    Triangle* copy = new Triangle(getStartPoint());
    copy->setEndPoint(getEndPoint());
    copy->setPenColor(getPenColor());
    copy->setBrushColor(getBrushColor());
    copy->setPenWidth(getPenWidth());
    copy->setPos(pos());
    copy->setRotation(rotation());
    copy->setTransformOriginPoint(transformOriginPoint());
    return copy;
}

QRectF Triangle::boundingRect() const
{
    return getPolygon().boundingRect();
}
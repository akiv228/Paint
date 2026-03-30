#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "figure.h"
#include "figureType.h"

class Triangle : public Figure
{
    Q_OBJECT

public:
    explicit Triangle(QPointF point, QObject *parent = nullptr);
    void prepareGeometryChangePublic() {
        prepareGeometryChange();
    }
    
    qreal getPerimeter() override;
    qreal getSquare() override;
    FigureType getFigureType() override;
    QPointF getCenterOfMass() const override;
    Figure* clone() const override;
    QRectF boundingRect() const override;

protected:
    
    QPolygonF getPolygon() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;

private:
    mutable QPolygonF polygon_;  
};

#endif 
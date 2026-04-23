#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include "figure.h"

#include "figureType.h"

class Parallelogram : public Figure
{
public:
    explicit Parallelogram(QPointF point, QObject *parent = nullptr);

    qreal getPerimeter();
    qreal getSquare();
    QString getFigureTypeId() const override { return "Parallelogram"; }
    Figure* clone() const override;

private:
    QPolygonF polygon_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
};

#endif 

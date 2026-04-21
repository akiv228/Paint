#ifndef CIRCLE_H
#define CIRCLE_H

#include "figure.h"

#include "figureType.h"

class Circle : public Figure
{
public:
    explicit Circle(QPointF point, QObject *parent = nullptr);

    qreal getRadius();
    qreal getPerimeter();
    qreal getSquare();
    FigureType getFigureType() const override;

    Figure* clone() const override;

private:
    qreal radius_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QRectF boundingRect() const;
};

#endif 

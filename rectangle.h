#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "figure.h"

#include "figureType.h"

class Rectangle : public Figure
{
public:
    explicit Rectangle(QPointF point, QObject *parent = nullptr);

    qreal getPerimeter();
    qreal getSquare();
    FigureType getFigureType();

private:
    QPolygonF polygon_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
};

#endif // RECTANGLE_H

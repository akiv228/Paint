#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "figure.h"

class Ellipse : public Figure
{
public:
    explicit Ellipse(QPointF point, QObject *parent = nullptr);

    qreal getPerimeter() override;
    qreal getSquare() override;
    FigureType getFigureType() override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    Figure* clone() const override;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // ELLIPSE_H
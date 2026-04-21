
#ifndef LINE_H
#define LINE_H

#include "figure.h"

class Line : public Figure
{
public:
    explicit Line(QPointF point, QObject *parent = nullptr);

    qreal getPerimeter() override;
    qreal getSquare() override;
    FigureType getFigureType() const override;
    QPainterPath shape() const override;
    Figure* clone() const override;
    QRectF boundingRect() const override;


private:
    QLineF line_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif 
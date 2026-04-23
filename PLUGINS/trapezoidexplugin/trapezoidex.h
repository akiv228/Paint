#ifndef TRAPEZOIDEX_H
#define TRAPEZOIDEX_H

#include "figure.h"

class TrapezoidEx : public Figure
{
    Q_OBJECT
public:
    explicit TrapezoidEx(QPointF point, QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;
    qreal getPerimeter() override;
    qreal getSquare() override;
    QString getFigureTypeId() const override { return "TrapezoidEx"; }
    Figure* clone() const override;
private:
    QPolygonF polygon_;
};

#endif

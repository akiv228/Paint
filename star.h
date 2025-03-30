#ifndef STAR_H
#define STAR_H

#include "figure.h"

#include "figureType.h"

class Star : public Figure
{
public:
    explicit Star(QPointF point, QObject *parent = nullptr);
    explicit Star(QPointF point, int pointsCount, QObject *parent = nullptr);

    static void setPointsCount(int pointsCount);
    static int getPointsCount();
    qreal getRadius();
    qreal getPerimeter();
    qreal getSquare();
    FigureType getFigureType();

private:
    QPolygonF polygon_;
    qreal radius_;

    static int points_count_;
    int currentPointsCount_;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QRectF boundingRect() const;
};

#endif // STAR_H

#ifndef POLYLINE_H
#define POLYLINE_H

#include "figure.h"
#include <QPolygonF>

class Polyline : public Figure
{
public:
    explicit Polyline(QPointF point, QObject *parent = nullptr);

    void addPoint(const QPointF &point);
    void setTempPoint(const QPointF &point);
    void clearTemp();
    bool isBuilding() const { return isBuilding_; }
    void setBuilding(bool building) { isBuilding_ = building; }
    void setPoints(const QPolygonF &points);
    QPolygonF points() const { return points_; }
    qreal getPerimeter() override;
    qreal getSquare() override;
    FigureType getFigureType() override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    Figure* clone() const override;

private:
    QPolygonF points_;
    QPointF tempPoint_;
    bool isBuilding_ = false;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // POLYLINE_H
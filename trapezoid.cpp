#include "trapezoid.h"

#include <cmath>

Trapezoid::Trapezoid(QPointF point, QObject *parent)
    : Figure{point, parent}
{}

// void Trapezoid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
// {
//     painterInit(painter);
//     polygon_.clear();
//     polygon_ << QPointF(getStartPoint().x() + (getEndPoint().x() < getStartPoint().x() ? -1 : 1) * qAbs(getEndPoint().x() - getStartPoint().x()) / 4, getStartPoint().y())
//             << QPointF(getEndPoint().x() + (getEndPoint().x() < getStartPoint().x() ? 1 : -1) * qAbs(getEndPoint().x() - getStartPoint().x()) / 4, getStartPoint().y())
//             << getEndPoint()
//             << QPointF(getStartPoint().x(), getEndPoint().y());
//     painter->drawPolygon(polygon_);
// }



void Trapezoid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painterInit(painter);
    polygon_.clear();
    // Получаем начальную и конечную точки
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();

    // Вычисляем координаты ограничивающего прямоугольника
    qreal left = qMin(start.x(), end.x());
    qreal right = qMax(start.x(), end.x());
    qreal top = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());
    qreal offset = (end.x() - start.x()) * 0.3; // Смещение для наклона

    // Определяем точки параллелограмма
    polygon_ << QPointF(left, top)                  // Верхняя левая
            << QPointF(right, top)                 // Верхняя правая
            << QPointF(right + offset, bottom)     // Нижняя правая
            << QPointF(left + offset, bottom);     // Нижняя левая

    painter->drawPolygon(polygon_);
}

QPainterPath Trapezoid::shape() const
{
    QPainterPath painter;
    painter.addPolygon(polygon_);
    return painter;
}

qreal Trapezoid::getPerimeter()
{
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    qreal left = qMin(start.x(), end.x());
    qreal right = qMax(start.x(), end.x());
    qreal top = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());
    qreal offset = (end.x() - start.x()) * 0.3;
    QPolygonF poly;
    poly << QPointF(left, top)
         << QPointF(right, top)
         << QPointF(right + offset, bottom)
         << QPointF(left + offset, bottom);

    qreal perimeter = 0;
    for (int i = 0; i < poly.size() - 1; i++) {
        perimeter += std::sqrt(std::pow(poly.at(i + 1).x() - poly.at(i).x(), 2)
                               + std::pow(poly.at(i + 1).y() - poly.at(i).y(), 2));
    }
    if (poly.size() > 1) {
        perimeter += std::sqrt(std::pow(poly.at(poly.size() - 1).x() - poly.at(0).x(), 2)
                               + std::pow(poly.at(poly.size() - 1).y() - poly.at(0).y(), 2));
    }
    return perimeter;
}

qreal Trapezoid::getSquare()
{
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    qreal left = qMin(start.x(), end.x());
    qreal right = qMax(start.x(), end.x());
    qreal top = qMin(start.y(), end.y());
    qreal bottom = qMax(start.y(), end.y());
    qreal offset = (end.x() - start.x()) * 0.3;
    QPolygonF poly;
    poly << QPointF(left, top)
         << QPointF(right, top)
         << QPointF(right + offset, bottom)
         << QPointF(left + offset, bottom);

    qreal sum = 0;
    int n = poly.size();
    if (n > 1)
    {
        for (int i = 0; i < n - 1; i++)
        {
            sum += poly.at(i).x() * poly.at(i + 1).y();
            sum -= poly.at(i + 1).x() * poly.at(i).y();
        }
        sum += poly.at(n - 1).x() * poly.at(0).y();
        sum -= poly.at(0).x() * poly.at(n - 1).y();
    }
    return 0.5 * qAbs(sum);
}

// qreal Trapezoid::getPerimeter()
// {
//     qreal perimeter = 0;
//     for (int i = 0; i < polygon_.size() - 1; i++) {
//         perimeter += std::sqrt(std::pow(polygon_.at(i + 1).x() - polygon_.at(i).x(), 2)
//                                + std::pow(polygon_.at(i + 1).y() - polygon_.at(i).y(), 2));
//     }
//     if (polygon_.size() > 1) {
//         perimeter += std::sqrt(
//             std::pow(polygon_.at(polygon_.size() - 1).x() - polygon_.at(0).x(), 2)
//             + std::pow(polygon_.at(polygon_.size() - 1).y() - polygon_.at(0).y(), 2));
//     }
//     return perimeter;
// }

// qreal Trapezoid::getSquare()
// {
//     qreal sum = 0;
//     int n = polygon_.size();
//     if (n > 1)
//     {
//         for (int i = 0; i < n - 1; i++)
//         {
//             sum += polygon_.at(i).x() * polygon_.at(i + 1).y();
//             sum -= polygon_.at(i + 1).x() * polygon_.at(i).y();
//         }
//         sum += polygon_.at(n - 1).x() * polygon_.at(0).y();
//         sum -= polygon_.at(0).x() * polygon_.at(n - 1).y();
//     }
//     return 0.5 * qAbs(sum);
// }

FigureType Trapezoid::getFigureType()
{
    return kTrapezoid;
}

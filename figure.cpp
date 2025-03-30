#include "figure.h"

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <qdebug.h>
#include <QGraphicsScene>

Figure::Figure(QPointF point, QObject *parent)
    : QObject{parent}
{
    this->setStartPoint(mapFromScene(point));
    this->setEndPoint(mapFromScene(point));
    connect(this, &Figure::pointChanged, this, &Figure::updateRect);
}

// QRectF Figure::boundingRect() const
// {
//     return QRectF((getEndPoint().x() > getStartPoint().x() ? getStartPoint().x() : getEndPoint().x()) - 5,
//                   (getEndPoint().y() > getStartPoint().y() ? getStartPoint().y() : getEndPoint().y()) - 5,
//                   qAbs(getEndPoint().x() - getStartPoint().x()) + 10,
//                   qAbs(getEndPoint().y() - getStartPoint().y()) + 10);
// }

QRectF Figure::boundingRect() const
{
    return shape().boundingRect();
}

QPointF Figure::getCenterOfMass() const
{
    return boundingRect().center(); // По умолчанию центр описанного прямоугольника
}

// void Figure::updateRect()
// {
//     this->update((getEndPoint().x() > getStartPoint().x() ? getStartPoint().x() : getEndPoint().x()) - 5,
//                  (getEndPoint().y() > getStartPoint().y() ? getStartPoint().y() : getEndPoint().y()) - 5,
//                  qAbs(getEndPoint().x() - getStartPoint().x()) + 10,
//                  qAbs(getEndPoint().y() - getStartPoint().y()) + 10);
// }


void Figure::updateRect()
{
    this->update();
}


void Figure::setStartPoint(const QPointF point) {
    startPoint_ = point; // локальные координаты
    emit pointChanged();
}

void Figure::setEndPoint(const QPointF point) {
    endPoint_ = point; // локальные координаты
    // endPoint_ = mapFromScene(point);
    emit pointChanged();
}

void Figure::setPenColor(QColor color)
{
    penColor_ = color;
}

void Figure::setBrushColor(QColor color)
{
    brushColor_ = color;
}

void Figure::setPenWidth(qreal width)
{
    penWidth_ = width;
}

QPointF Figure::getStartPoint() const
{
    return startPoint_;
}

QPointF Figure::getEndPoint() const
{
    return endPoint_;
}

void Figure::select()
{
    selected_ = true;
}

void Figure::un_select()
{
    selected_ = false;
}

bool Figure::is_selected()
{
    return selected_;
}

void Figure::painterInit(QPainter *painter)
{
    if (selected_) {
        drawSelectFrame(painter);
    }
    painter->setPen(QPen(penColor_, penWidth_));
    if (brushColor_.isValid() && brushColor_ != QColor(QStringLiteral("#1f1f1f"))) {
        painter->setBrush(brushColor_);
    }
}

void Figure::drawSelectFrame(QPainter *painter)
{
    painter->setPen(QPen(Qt::white, 1, Qt::DashDotLine));
    painter->drawRect(boundingRect());
    QPointF center = getCenterOfMass(); // Используем центр масс
    painter->drawPoint(center);
}

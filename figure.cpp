#include "figure.h"
#include "layer.h"
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

QRectF Figure::boundingRect() const
{
    return shape().boundingRect();
}

QPointF Figure::getCenterOfMass() const
{
    return boundingRect().center();
}

void Figure::updateRect()
{
    this->update();
}

void Figure::setStartPoint(const QPointF point) {
    startPoint_ = point;
    emit pointChanged();;
}

void Figure::setEndPoint(const QPointF point) {
    endPoint_ = point;
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
    QPointF center = getCenterOfMass();
    painter->drawPoint(center);
}

void Figure::setLayer(Layer *layer)
{
    qDebug() << "Figure::setLayer: this =" << (void*)this
             << "old layer =" << (void*)layer_
             << "new layer =" << (void*)layer;
    layer_ = layer;
}

Layer* Figure::layer() const
{
    return layer_;
}


void Figure::updateTransformOriginPoint()
{
    QPointF center = boundingRect().center();
    setTransformOriginPoint(center);
}
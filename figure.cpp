#include "figure.h"
#include "layer.h"
#include "triangle.h"
#include "circle.h"
#include "rhombus.h"
#include "square.h"
#include "rectangle.h"
#include "star.h"
#include "polygon.h"
#include "trapezoid.h"
#include "line.h"
#include "polyline.h"
#include "ellipse.h"
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>
#include <qdebug.h>

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
    emit pointChanged();
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

// JSON сериализация

QString figureTypeToString(FigureType type) {
    switch (type) {
    case kTriangle:   return "Triangle";
    case kCircle:     return "Circle";
    case kRhombus:    return "Rhombus";
    case kSquare:     return "Square";
    case kRectangle:  return "Rectangle";
    case kStar:       return "Star";
    case kPolygon:    return "Polygon";
    case kTrapezoid:  return "Trapezoid";
    case kLine:       return "Line";
    case kEllipse:    return "Ellipse";
    case kPolyline:   return "Polyline";
    default:          return "Unknown";
    }
}

FigureType figureTypeFromString(const QString &str) {
    if (str == "Triangle")  return kTriangle;
    if (str == "Circle")    return kCircle;
    if (str == "Rhombus")   return kRhombus;
    if (str == "Square")    return kSquare;
    if (str == "Rectangle") return kRectangle;
    if (str == "Star")      return kStar;
    if (str == "Polygon")   return kPolygon;
    if (str == "Trapezoid") return kTrapezoid;
    if (str == "Line")      return kLine;
    if (str == "Ellipse")   return kEllipse;
    if (str == "Polyline")  return kPolyline;
    return kTriangle;
}

QJsonObject Figure::toJson() const
{
    QJsonObject obj;
    obj["type"] = figureTypeToString(getFigureType());
    obj["startPoint"] = QJsonObject{{"x", startPoint_.x()}, {"y", startPoint_.y()}};
    obj["endPoint"]   = QJsonObject{{"x", endPoint_.x()},   {"y", endPoint_.y()}};
    obj["penColor"]   = penColor_.name(QColor::HexArgb);
    // Сохраняем кисть только если она валидна (т.е. была установлена)
    if (brushColor_.isValid()) {
        obj["brushColor"] = brushColor_.name(QColor::HexArgb);
    } else {
        obj["brushColor"] = QJsonValue(); // null
    }
    obj["penWidth"]   = penWidth_;
    obj["pos"]        = QJsonObject{{"x", pos().x()}, {"y", pos().y()}};
    obj["rotation"]   = rotation();
    obj["transformOriginPoint"] = QJsonObject{{"x", transformOriginPoint().x()},
                                              {"y", transformOriginPoint().y()}};
    obj["zValue"] = zValue();
    return obj;
}

void Figure::fromJson(const QJsonObject &obj)
{
    auto readPoint = [](const QJsonObject &o) -> QPointF {
        return QPointF(o["x"].toDouble(), o["y"].toDouble());
    };

    if (obj.contains("startPoint"))
        startPoint_ = readPoint(obj["startPoint"].toObject());
    if (obj.contains("endPoint"))
        endPoint_ = readPoint(obj["endPoint"].toObject());
    if (obj.contains("penColor"))
        penColor_ = QColor(obj["penColor"].toString());
    if (obj.contains("brushColor") && !obj["brushColor"].isNull()) {
        brushColor_ = QColor(obj["brushColor"].toString());
    } else {
        brushColor_ = QColor(); // невалидный цвет (без кисти)
    }
    if (obj.contains("penWidth"))
        penWidth_ = obj["penWidth"].toDouble();
    if (obj.contains("pos"))
        setPos(readPoint(obj["pos"].toObject()));
    if (obj.contains("rotation"))
        setRotation(obj["rotation"].toDouble());
    if (obj.contains("transformOriginPoint"))
        setTransformOriginPoint(readPoint(obj["transformOriginPoint"].toObject()));
    if (obj.contains("zValue"))
        setZValue(obj["zValue"].toDouble());

    updateRect();
}

Figure* Figure::createFromJson(const QJsonObject &obj)
{
    FigureType type = figureTypeFromString(obj["type"].toString());
    Figure *fig = nullptr;

    switch (type) {
    case kTriangle:   fig = new Triangle(QPointF()); break;
    case kCircle:     fig = new Circle(QPointF());   break;
    case kRhombus:    fig = new Rhombus(QPointF());  break;
    case kSquare:     fig = new Square(QPointF());   break;
    case kRectangle:  fig = new Rectangle(QPointF());break;
    case kStar:       fig = new Star(QPointF());     break;
    case kPolygon:    fig = new Polygon(QPointF());  break;
    case kTrapezoid:  fig = new Trapezoid(QPointF());break;
    case kLine:       fig = new Line(QPointF());     break;
    case kEllipse:    fig = new Ellipse(QPointF());  break;
    case kPolyline:   fig = new Polyline(QPointF()); break;
    default: return nullptr;
    }

    fig->fromJson(obj);
    return fig;
}
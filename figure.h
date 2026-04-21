#ifndef FIGURE_H
#define FIGURE_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>

#include "figureType.h"

class Layer;

class Figure : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(QPointF startPoint READ getStartPoint WRITE setStartPoint NOTIFY pointChanged)
    Q_PROPERTY(QPointF endPoint READ getEndPoint WRITE setEndPoint NOTIFY pointChanged)

public:
    explicit Figure(QPointF point, QObject *parent = nullptr);
    void prepareGeometryChangePublic() {
        prepareGeometryChange();
    }
    QPointF getStartPoint() const;
    QPointF getEndPoint() const;
    void setStartPoint(const QPointF point);
    void setEndPoint(const QPointF point);
    void setPenColor(QColor color);
    void setBrushColor(QColor color);
    void setPenWidth(qreal width);
    void select();
    void un_select();
    bool is_selected();
    QRectF boundingRect() const override;
    virtual qreal getPerimeter() = 0;
    virtual qreal getSquare() = 0;
    virtual FigureType getFigureType() const = 0;
    virtual QPointF getCenterOfMass() const;
    virtual Figure* clone() const = 0;
    QColor getPenColor() const { return penColor_; }
    QColor getBrushColor() const { return brushColor_; }
    qreal getPenWidth() const { return penWidth_; }
    void updateTransformOriginPoint();
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject &json);
    static Figure* createFromJson(const QJsonObject &json);
    void setLayer(Layer *layer);
    Layer* layer() const;
    virtual ~Figure() = default;

public slots:
    void updateRect();

private:
    QPointF startPoint_;
    QPointF endPoint_;

protected:
    QColor penColor_;
    QColor brushColor_ = QColor(QStringLiteral("#1f1f1f"));
    qreal penWidth_ = 1;
    bool selected_ = false;
    Layer *layer_ = nullptr;
    void painterInit(QPainter *painter);
    void drawSelectFrame(QPainter *painter);

signals:
    void pointChanged();
};

QString figureTypeToString(FigureType type);
FigureType figureTypeFromString(const QString &str);

#endif 
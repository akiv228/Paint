#ifndef FIGURE_H
#define FIGURE_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>

#include "figureType.h"

class Figure : public QObject, public QGraphicsItem
{
    Q_OBJECT
    //свойство стартовой точки, относительно которой отрисовываем фигуру
    Q_PROPERTY(QPointF startPoint READ getStartPoint WRITE setStartPoint NOTIFY pointChanged)
     // Свойство конечной точки, до куда отрисовываем фигуру
    Q_PROPERTY(QPointF endPoint READ getEndPoint WRITE setEndPoint NOTIFY pointChanged)
public:
    explicit Figure(QPointF pouint, QObject *parent = nullptr);

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
    QRectF boundingRect() const;
    virtual qreal getPerimeter() = 0;
    virtual qreal getSquare() = 0;
    virtual FigureType getFigureType() = 0;
    virtual QPointF getCenterOfMass() const;


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

    void painterInit(QPainter *painter);
    void drawSelectFrame(QPainter *painter);

signals:
    void pointChanged();
};

#endif // FIGURE_H

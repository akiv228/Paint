#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "figure.h"
#include "figureType.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT // cbuyfks выпускаются объектом, когда его внутреннее состояние изменилось
    // Свойство текущего типа используемой фигуры
    Q_PROPERTY(FigureType figureType READ getFigureType WRITE setFigureType NOTIFY figureTypeChanged FINAL)
    // макрос определяет свойство figureType типа FigureType
    // READ: Свойство можно получить с помощью getFigureType.
    // WRITE: Его можно установить с помощью setFigureType.
    // NOTIFY: При изменении свойства испускается сигнал figureTypeChanged.
    // FINAL: Это свойство не может быть переопределено в производных классах.

public:
    explicit Scene(QObject *parent = nullptr); //запрещает неявное преобразование типов в конструкторах

    FigureType getFigureType() const;
    void setFigureType(const FigureType type);
    void setPenColor(QColor color);
    void setBrushColor(QColor color);
    void setPenWidth(qreal width);
    qreal getPenWidth();
    void deleteSelectedFigure();
    qreal getSelectedFigurePerimeter(bool &ok);
    qreal getSelectedFigureSquare(bool &ok);
    Figure *getSelectedFigure() const { return selectedFigure_; } //возвращает указатель на выбранную фигуру
    void removeAllItems();
    void un_select();

private:
    Figure *currentFigure_;
    Figure *selectedFigure_ = nullptr;
    FigureType figureType_;
    QColor currentPenColor_;
    QColor currentBrushColor_;
    qreal currentPenWidth_ = 1;
    QPointF pressPos_; // координаты нажатия
    QPointF selectedStartPoint_;
    QPointF selectedEndPoint_;
    bool currentAdded_ = false;
    int currentSide_ = -1; // за какую сторону рамки брать
    enum { kChanging, kMoving, kRotating, kDrawing} mode_;
    qreal startRotation_ = 0; // для повторного поворота setrotate просит конкретное, нельзя к текущему прибавлять
    QPointF startRotateVector_; // координаты точки относительно центра фигруы
    qreal commonAngel_ = 0;
    qreal intermediateAngel_ = 0;
    qreal startAngle_= 0;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    Figure *figureAt(QPointF point);
    bool onRectBoard(QRectF rect, QPointF point, int &side);

signals:
    void figureTypeChanged();
};

#endif // SCENE_H

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "figure.h"
#include "figureType.h"
#include "line.h"
#include "polyline.h"
#include "ellipse.h"
#include "layer.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(FigureType figureType READ getFigureType WRITE setFigureType NOTIFY figureTypeChanged FINAL)

public:
    explicit Scene(QObject *parent = nullptr);

    FigureType getFigureType() const;
    void setFigureType(const FigureType type);
    void setPenColor(QColor color);
    void setBrushColor(QColor color);
    void setPenWidth(qreal width);
    qreal getPenWidth();
    void deleteSelectedFigure();
    qreal getSelectedFigurePerimeter(bool &ok);
    qreal getSelectedFigureSquare(bool &ok);
    Figure *getSelectedFigure() const { return selectedFigures_.isEmpty() ? nullptr : selectedFigures_.first(); }
    QList<Figure*> getSelectedFigures() const { return selectedFigures_; }
    void removeAllItems();
    void un_select();
    void copySelectedFigure();
    void cancelPolylineBuilding();
    void scaleSelectedFigure(qreal factor);
    void pushUndoState();
    void undo();
    void redo();
    void rotateSelected(qreal deltaAngle);

    // Управление слоями
    void addLayer(const QString &name);
    void removeLayer(int index);
    void moveLayerUp(int index);
    void moveLayerDown(int index);
    void setLayerVisible(int index, bool visible);
    QList<Layer*> layers() const;
    Layer* currentLayer() const;
    void setCurrentLayer(int index);
    void moveFigureToLayer(Figure *figure, int layerIndex);


signals:
    void figureTypeChanged();
    void layersChanged();

private:
    Figure *currentFigure_;
    QList<Figure*> selectedFigures_;
    FigureType figureType_;
    QColor currentPenColor_;
    QColor currentBrushColor_;
    qreal currentPenWidth_ = 1;
    QPointF pressPos_;
    QPointF selectedStartPoint_;
    QPointF selectedEndPoint_;
    bool currentAdded_ = false;
    int currentSide_ = -1;
    enum { kChanging, kMoving, kRotating, kDrawing } mode_;
    qreal startRotation_ = 0;
    QPointF startRotateVector_;
    qreal commonAngel_ = 0;
    qreal intermediateAngel_ = 0;
    qreal startAngle_ = 0;
    int selectedPointIndex_;
    QPointF rotationCenter_;

    bool isBuildingPolyline_ = false;
    Polyline *buildingPolyline_ = nullptr;

    QVector<QList<Figure*>> undoStack_;
    QVector<QList<Figure*>> redoStack_;

    QList<Layer*> layers_;
    int currentLayerIndex_ = -1;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    Figure *figureAt(QPointF point);
    bool onRectBoard(QRectF rect, QPointF point, int &side);
    void restoreState(const QList<Figure*>& state);
    void clearSelection();
    void addToSelection(Figure *fig);
    void removeFromSelection(Figure *fig);
    bool isSelected(Figure *fig) const;
    void applyToAllSelected(std::function<void(Figure*)> func);
    void updateZOrder();
};

#endif // SCENE_H
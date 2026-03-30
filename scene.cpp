#include "scene.h"

#include "circle.h"
#include "polygon.h"
#include "rectangle.h"
#include "rhombus.h"
#include "square.h"
#include "star.h"
#include "trapezoid.h"
#include "triangle.h"
#include "line.h"
#include "polyline.h"
#include "ellipse.h"

#include <QGraphicsView>
#include <QInputDialog>
#include <qdebug.h>
#include <cmath>
#include <algorithm>

Scene::Scene(QObject *parent)
    : QGraphicsScene{parent}, selectedPointIndex_(0)
{
    addLayer("Layer 1");
    setCurrentLayer(0);
    rotationCenter_ = QPointF();
}

FigureType Scene::getFigureType() const
{
    return figureType_;
}

void Scene::setFigureType(const FigureType type)
{
    figureType_ = type;
}

void Scene::setPenColor(QColor color)
{
    currentPenColor_ = color;
    
    QList<Figure*> savedSelection = selectedFigures_;
    applyToAllSelected([color](Figure *fig) {
        fig->setPenColor(color);
        fig->update();
    });
    
    for (Figure *fig : savedSelection) {
        if (isSelected(fig)) {
            addToSelection(fig);
        }
    }
}

void Scene::setBrushColor(QColor color)
{
    currentBrushColor_ = color;
    QList<Figure*> savedSelection = selectedFigures_;
    applyToAllSelected([color](Figure *fig) {
        fig->setBrushColor(color);
        fig->update();
    });
    for (Figure *fig : savedSelection) {
        if (isSelected(fig)) {
            addToSelection(fig);
        }
    }
}

void Scene::setPenWidth(qreal width)
{
    currentPenWidth_ = width;
    QList<Figure*> savedSelection = selectedFigures_;
    applyToAllSelected([width](Figure *fig) {
        fig->setPenWidth(width);
        fig->update();
    });
    for (Figure *fig : savedSelection) {
        if (isSelected(fig)) {
            addToSelection(fig);
        }
    }
}

qreal Scene::getPenWidth()
{
    return currentPenWidth_;
}

void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (isBuildingPolyline_ && buildingPolyline_)
    {
        buildingPolyline_->clearTemp();
        buildingPolyline_->setBuilding(false);
        isBuildingPolyline_ = false;
        buildingPolyline_ = nullptr;
        currentFigure_ = nullptr;
        pushUndoState();
        this->update();
        return;
    }

    
    Figure *temp_figure = nullptr;
    QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
    for (QGraphicsItem *item : itemsAtPos) {
        Figure *fig = dynamic_cast<Figure*>(item);
        if (fig && fig->layer() && fig->layer() == currentLayer()) {
            temp_figure = fig;
            break;
        }
    }

    if (temp_figure != nullptr) {
        clearSelection();
        addToSelection(temp_figure);
        pressPos_ = event->scenePos();
        selectedStartPoint_ = temp_figure->getStartPoint();
        selectedEndPoint_ = temp_figure->getEndPoint();
    }
    else
    {
        
        clearSelection();
    }
    this->update(QRectF(0, 0, this->width(), this->height()));
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    
    if (isBuildingPolyline_)
    {
        if (buildingPolyline_)
        {
            QPointF newPoint = event->scenePos();
            if (buildingPolyline_->points().isEmpty() ||
                QLineF(buildingPolyline_->points().last(), newPoint).length() > 5)
            {
                buildingPolyline_->addPoint(newPoint);
            }
        }
        return;
    }

    
    if (event->button() == Qt::RightButton)
    {
        Figure *clicked = nullptr;
        
        if (currentLayer()) {
            for (Figure *fig : currentLayer()->figures()) {
                if (fig->boundingRect().contains(fig->mapFromScene(event->scenePos()))) {
                    clicked = fig;
                    break;
                }
            }
        }
        if (clicked) {
            if (isSelected(clicked))
                removeFromSelection(clicked);
        } else {
            clearSelection();
        }
        return;
    }

    
    
    Figure *clicked = nullptr;
    if (currentLayer()) {
        QList<Figure*> candidates;
        for (Figure *fig : currentLayer()->figures()) {
            if (fig->boundingRect().contains(fig->mapFromScene(event->scenePos())))
                candidates.append(fig);
        }
        if (!candidates.isEmpty()) {
            
            std::sort(candidates.begin(), candidates.end(),
                      [](Figure *a, Figure *b) { return a->zValue() > b->zValue(); });
            clicked = candidates.first();
        }
    }

    
    qDebug() << "Clicked figure:" << (void*)clicked
             << "layer:" << (clicked ? clicked->layer() : nullptr)
             << "scene:" << (clicked ? clicked->scene() : nullptr)
             << "currentLayer:" << currentLayer();

    if (clicked == nullptr) {
        QList<QGraphicsItem*> all = items();
        qDebug() << "No figure under cursor. Total items:" << all.size();
        for (QGraphicsItem* item : all) {
            Figure* fig = dynamic_cast<Figure*>(item);
            if (fig) {
                qDebug() << "  Figure" << (void*)fig
                         << "layer:" << fig->layer()
                         << "boundingRect:" << fig->boundingRect()
                         << "scenePos of item:" << item->scenePos();
            }
        }
    }

    
    if (clicked != nullptr) {
        
        if (clicked->scene() != this) {
            qWarning() << "Figure is not on this scene, ignoring";
            return;
        }

        
        if (clicked->layer() == nullptr && currentLayer()) {
            qWarning() << "Figure has no layer, reassigning to current layer";
            currentLayer()->addFigure(clicked);
        }

        
        if (clicked->layer() != currentLayer()) {
            qDebug() << "Figure layer doesn't match current layer, ignoring";
            return;
        }

        
        bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);
        bool ctrlPressed = (event->modifiers() & Qt::ControlModifier);

        if (!shiftPressed && !ctrlPressed) {
            if (!isSelected(clicked)) {
                clearSelection();
            }
        }

        if (shiftPressed) {
            if (isSelected(clicked))
                removeFromSelection(clicked);
            else
                addToSelection(clicked);
        } else if (ctrlPressed) {
            if (!isSelected(clicked)) {
                clearSelection();
                addToSelection(clicked);
            }

            
            if (selectedFigures_.size() == 1) {
                rotationCenter_ = clicked->mapToScene(clicked->boundingRect().center());
            } else {
                QRectF totalRect;
                for (Figure *fig : selectedFigures_) {
                    totalRect = totalRect.united(fig->mapToScene(fig->boundingRect()).boundingRect());
                }
                rotationCenter_ = totalRect.center();
            }
            QPointF start_vector = event->scenePos() - rotationCenter_;
            startAngle_ = std::atan2(start_vector.y(), start_vector.x()) * 180 / M_PI;
            pressPos_ = event->scenePos();
            mode_ = kRotating;
            clicked->update();
            return;
        } else {
            if (!isSelected(clicked)) {
                addToSelection(clicked);
            }
        }

        
        if (!selectedFigures_.isEmpty() && isSelected(clicked) && mode_ != kRotating) {
            Figure *selected = clicked;
            QPointF local_pos = selected->mapFromScene(event->scenePos());
            if (onRectBoard(selected->boundingRect(), local_pos, currentSide_)) {
                mode_ = kChanging;
                selectedStartPoint_ = selected->getStartPoint();
                selectedEndPoint_ = selected->getEndPoint();
            }
            
            else if (selected->boundingRect().contains(local_pos)) {
                mode_ = kMoving;
                pressPos_ = event->scenePos();
            }
            selected->update();
        }
        return;
    }

    
    bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);
    if (shiftPressed) {
        return; 
    }

    switch (figureType_)
    {
    case kTriangle:
        currentFigure_ = new Triangle(event->scenePos());
        break;
    case kCircle:
        currentFigure_ = new Circle(event->scenePos());
        break;
    case kRhombus:
        currentFigure_ = new Rhombus(event->scenePos());
        break;
    case kSquare:
        currentFigure_ = new Square(event->scenePos());
        break;
    case kRectangle:
        currentFigure_ = new Rectangle(event->scenePos());
        break;
    case kStar:
        currentFigure_ = new Star(event->scenePos());
        break;
    case kPolygon:
        currentFigure_ = new Polygon(event->scenePos());
        break;
    case kTrapezoid:
        currentFigure_ = new Trapezoid(event->scenePos());
        break;
    case kLine:
        currentFigure_ = new Line(event->scenePos());
        break;
    case kEllipse:
        currentFigure_ = new Ellipse(event->scenePos());
        break;
    case kPolyline:
    {
        Polyline *polyline = new Polyline(event->scenePos());
        currentFigure_ = polyline;
        buildingPolyline_ = polyline;
        isBuildingPolyline_ = true;
        buildingPolyline_->setBuilding(true);
        polyline->setPenColor(currentPenColor_);
        polyline->setBrushColor(currentBrushColor_);
        polyline->setPenWidth(currentPenWidth_);
        this->addItem(currentFigure_);
        currentFigure_->updateTransformOriginPoint();
        if (currentLayer())
            currentLayer()->addFigure(currentFigure_);
        currentAdded_ = true;
        return;
    }
    default:
        currentFigure_ = new Triangle(event->scenePos());
        break;
    }

    currentFigure_->setPos(event->pos());
    currentFigure_->setPenColor(currentPenColor_);
    currentFigure_->setBrushColor(currentBrushColor_);
    currentFigure_->setPenWidth(currentPenWidth_);

    Layer* targetLayer = currentLayer();
    qDebug() << "mousePressEvent: targetLayer =" << (void*)targetLayer
             << "layers_.size() =" << layers_.size()
             << "currentLayerIndex_ =" << currentLayerIndex_;

    if (!targetLayer && !layers_.isEmpty()) {
        targetLayer = layers_.first();
        setCurrentLayer(0);
        qDebug() << "currentLayer was null, using first layer, new targetLayer =" << (void*)targetLayer;
    }

    if (targetLayer) {
        qDebug() << "Calling targetLayer->addFigure for new figure";
        targetLayer->addFigure(currentFigure_);
        qDebug() << "After addFigure, figure layer:" << (void*)(currentFigure_->layer());
        if (!currentFigure_->layer()) {
            qWarning() << "CRITICAL: figure layer is null after addFigure!";
        }
    }

    currentAdded_ = false;
    this->update(QRectF(0, 0, this->width(), this->height()));
}


void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    
    if (isBuildingPolyline_ && buildingPolyline_)
    {
        buildingPolyline_->setTempPoint(event->scenePos());
        this->update(QRectF(0, 0, this->width(), this->height()));
        return;
    }

    if (!selectedFigures_.isEmpty())
    {
        if (mode_ == kMoving)
        {
            QPointF delta = event->scenePos() - pressPos_;
            applyToAllSelected([delta](Figure *fig) {
                fig->setPos(fig->pos() + delta);
                fig->update();
            });
            pressPos_ = event->scenePos();
            this->update();
            return;
        }
        else if (mode_ == kRotating)
        {
            QPointF current_vector = event->scenePos() - rotationCenter_;
            qreal current_angle = std::atan2(current_vector.y(), current_vector.x()) * 180 / M_PI;
            qreal angle_delta = current_angle - startAngle_;
            if (angle_delta > 180) angle_delta -= 360;
            else if (angle_delta < -180) angle_delta += 360;

            if (angle_delta != 0)
            {
                for (Figure *fig : selectedFigures_) {
                    fig->setTransformOriginPoint(fig->boundingRect().center());
                    fig->setRotation(fig->rotation() + angle_delta);
                }
                startAngle_ = current_angle;
                this->update();
            }
            return;
        }
        else if (mode_ == kChanging && selectedFigures_.size() == 1)
        {
            Figure *selected = selectedFigures_.first();
            if (selected->getFigureType() == kLine)
            {
                Line *line = dynamic_cast<Line*>(selected);
                if (line)
                {
                    QPointF new_point = event->scenePos();
                    if (selectedPointIndex_ == 0)
                        line->setStartPoint(new_point);
                    else
                        line->setEndPoint(new_point);
                }
            }
            else
            {
                qreal dx = event->scenePos().x() - pressPos_.x();
                qreal dy = event->scenePos().y() - pressPos_.y();
                FigureType selected_figure_type = selected->getFigureType();

                if (selected_figure_type == kCircle || selected_figure_type == kStar
                    || selected_figure_type == kPolygon || selected_figure_type == kEllipse)
                {
                    qreal radius = std::sqrt(std::pow(selectedEndPoint_.x() - selectedStartPoint_.x(), 2) + std::pow(selectedEndPoint_.y() - selectedStartPoint_.y(), 2));
                    switch (currentSide_)
                    {
                    case 0:
                        selectedEndPoint_.y() > selectedStartPoint_.y() ? selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dy / radius)))
                                                                    : selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dy / radius)));
                        break;
                    case 1:
                        selectedEndPoint_.y() > selectedStartPoint_.y() ? selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dx / radius)))
                                                                    : selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dx / radius)));
                        break;
                    case 2:
                        selectedEndPoint_.y() > selectedStartPoint_.y() ? selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dy / radius)))
                                                                    : selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dy / radius)));
                        break;
                    case 3:
                        selectedEndPoint_.y() > selectedStartPoint_.y() ? selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dx / radius)))
                                                                    : selected->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dx / radius)));
                        break;
                    default:
                        break;
                    }
                }
                else if (selected_figure_type == kPolyline)
                {
                    Polyline *polyline = dynamic_cast<Polyline*>(selected);
                    if (polyline)
                    {
                        QRectF rect = polyline->boundingRect();
                        QPointF center = rect.center();
                        qreal scaleX = 1.0, scaleY = 1.0;

                        switch (currentSide_)
                        {
                        case 0:
                            scaleY = (rect.height() - dy) / rect.height();
                            break;
                        case 1:
                            scaleX = (rect.width() + dx) / rect.width();
                            break;
                        case 2:
                            scaleY = (rect.height() + dy) / rect.height();
                            break;
                        case 3:
                            scaleX = (rect.width() - dx) / rect.width();
                            break;
                        default:
                            break;
                        }

                        if (scaleX <= 0) scaleX = 0.01;
                        if (scaleY <= 0) scaleY = 0.01;

                        QPolygonF points = polyline->points();
                        for (int i = 0; i < points.size(); ++i)
                        {
                            points[i].setX(center.x() + (points[i].x() - center.x()) * scaleX);
                            points[i].setY(center.y() + (points[i].y() - center.y()) * scaleY);
                        }
                        polyline->setPoints(points);
                    }
                }
                else
                {
                    switch (currentSide_)
                    {
                    case 0:
                        selectedEndPoint_.y() > selectedStartPoint_.y() ? selected->setStartPoint(QPointF(selectedStartPoint_.x(), selectedStartPoint_.y() + dy))
                                                                    : selected->setEndPoint(QPointF(selectedEndPoint_.x(), selectedEndPoint_.y() + dy));
                        break;
                    case 1:
                        selectedEndPoint_.x() > selectedStartPoint_.x() ? selected->setEndPoint(QPointF(selectedEndPoint_.x() + dx, selectedEndPoint_.y()))
                                                                    : selected->setStartPoint(QPointF(selectedStartPoint_.x() + dx, selectedStartPoint_.y()));
                        break;
                    case 2:
                        selectedEndPoint_.y() > selectedStartPoint_.y() ? selected->setEndPoint(QPointF(selectedEndPoint_.x(), selectedEndPoint_.y() + dy))
                                                                    : selected->setStartPoint(QPointF(selectedStartPoint_.x(), selectedStartPoint_.y() + dy));
                        break;
                    case 3:
                        selectedEndPoint_.x() > selectedStartPoint_.x() ? selected->setStartPoint(QPointF(selectedStartPoint_.x() + dx, selectedStartPoint_.y()))
                                                                    : selected->setEndPoint(QPointF(selectedEndPoint_.x() + dx, selectedEndPoint_.y()));
                        break;
                    default:
                        break;
                    }
                }
            }
            selected->update();
            selected->updateTransformOriginPoint();
        }
    }
    else
    {
        
        if (!currentAdded_ && currentFigure_)
        {
            this->addItem(currentFigure_);
            currentAdded_ = true;
            currentFigure_->updateTransformOriginPoint();
        }
        if (currentFigure_)
            currentFigure_->setEndPoint(event->scenePos());
    }

    this->update(QRectF(0, 0, this->width(), this->height()));
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mouseReleaseEvent: currentFigure_ =" << (void*)currentFigure_
             << "currentAdded_ =" << currentAdded_
             << "isBuildingPolyline_ =" << isBuildingPolyline_;
    mode_ = kDrawing;
    selectedPointIndex_ = 0;

    
    if (currentAdded_ && currentFigure_ && !isBuildingPolyline_ && currentFigure_->scene() == this)
    {
        pushUndoState();
    }
    
    rotationCenter_ = QPointF();
    
    
    
}

void Scene::cancelPolylineBuilding()
{
    if (isBuildingPolyline_ && buildingPolyline_)
    {
        this->removeItem(buildingPolyline_);
        delete buildingPolyline_;
        buildingPolyline_ = nullptr;
        isBuildingPolyline_ = false;
        currentFigure_ = nullptr;
        this->update();
    }
}

Figure *Scene::figureAt(QPointF point)
{
    foreach (QGraphicsItem *item, this->items())
    {
        QPointF local_point = item->mapFromScene(point);
        if (item->shape().contains(local_point)) {
            return dynamic_cast<Figure *>(item);
        }
    }
    return nullptr;
}

void Scene::deleteSelectedFigure()
{
    if (selectedFigures_.isEmpty()) return;
    pushUndoState();
    for (Figure *fig : selectedFigures_) {
        if (fig->layer())
            fig->layer()->removeFigure(fig);
        this->removeItem(fig);
        delete fig;
    }
    clearSelection();
    this->update();
}


qreal Scene::getSelectedFigurePerimeter(bool &ok)
{
    if (selectedFigures_.size() == 1 && selectedFigures_.first()->is_selected()) {
        ok = true;
        return selectedFigures_.first()->getPerimeter();
    }
    ok = false;
    return 0;
}

qreal Scene::getSelectedFigureSquare(bool &ok)
{
    if (selectedFigures_.size() == 1 && selectedFigures_.first()->is_selected()) {
        ok = true;
        return selectedFigures_.first()->getSquare();
    }
    ok = false;
    return 0;
}

void Scene::removeAllItems()
{
    
    clearSelection();

    pushUndoState();

    QList<QGraphicsItem*> allItems = items();
    for (QGraphicsItem* item : allItems) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig) {
            removeItem(fig);
            delete fig;
        }
    }

    for (Layer* layer : layers_) {
        layer->clear();
    }

    qDeleteAll(layers_);
    layers_.clear();

    addLayer("Layer 1");
    setCurrentLayer(0);
    update();
    emit layersChanged();
}



void Scene::un_select()
{
    clearSelection();
}

bool Scene::onRectBoard(QRectF rect, QPointF point, int &side)
{
    side = -1;
    qreal offset = 30;
    if ((rect.contains(point) &&
        (
            !rect.contains(QPointF(point.x() + offset, point.y())) ||
            !rect.contains(QPointF(point.x() - offset, point.y())) ||
            !rect.contains(QPointF(point.x(), point.y() + offset)) ||
            !rect.contains(QPointF(point.x(), point.y() - offset))
                                  )) ||
        (!rect.contains(point) &&
            (
                rect.contains(QPointF(point.x() + offset, point.y())) ||
                rect.contains(QPointF(point.x() - offset, point.y())) ||
                rect.contains(QPointF(point.x(), point.y() + offset)) ||
                rect.contains(QPointF(point.x(), point.y() - offset))))
        )
    {
        if ((rect.contains(point) && !rect.contains(QPointF(point.x(), point.y() - offset)))
            || (!rect.contains(point) && rect.contains(QPointF(point.x(), point.y() + offset)))) {
            side = 0;
        }
        if ((rect.contains(point) && !rect.contains(QPointF(point.x() + offset, point.y())))
            || (!rect.contains(point) && rect.contains(QPointF(point.x() - offset, point.y())))) {
            side = 1;
        }
        if ((rect.contains(point) && !rect.contains(QPointF(point.x(), point.y() + offset)))
            || (!rect.contains(point) && rect.contains(QPointF(point.x(), point.y() - offset)))) {
            side = 2;
        }
        if ((rect.contains(point) && !rect.contains(QPointF(point.x() - offset, point.y())))
            || (!rect.contains(point) && rect.contains(QPointF(point.x() + offset, point.y())))) {
            side = 3;
        }
        return true;
    }
    return false;
}

void Scene::scaleSelectedFigure(qreal factor)
{
    if (selectedFigures_.size() != 1) return;
    Figure *selected = selectedFigures_.first();
    if (!selected->is_selected()) return;

    FigureType type = selected->getFigureType();
    if (type == kLine)
    {
        QPointF center = selected->boundingRect().center();
        QPointF start = selected->getStartPoint();
        QPointF end = selected->getEndPoint();
        QPointF newStart = center + (start - center) * factor;
        QPointF newEnd = center + (end - center) * factor;
        selected->setStartPoint(newStart);
        selected->setEndPoint(newEnd);
    }
    else if (type == kPolyline)
    {
        Polyline* polyline = dynamic_cast<Polyline*>(selected);
        if (polyline)
        {
            QPointF center = polyline->boundingRect().center();
            QPolygonF points = polyline->points();
            for (int i = 0; i < points.size(); ++i)
                points[i] = center + (points[i] - center) * factor;
            polyline->setPoints(points);
        }
    }
    else
    {
        QPointF center = selected->boundingRect().center();
        QPointF start = selected->getStartPoint();
        QPointF end = selected->getEndPoint();
        QPointF newStart = center + (start - center) * factor;
        QPointF newEnd = center + (end - center) * factor;
        selected->setStartPoint(newStart);
        selected->setEndPoint(newEnd);
    }
    selected->updateTransformOriginPoint();
    selected->update();
}


void Scene::copySelectedFigure()
{
    if (selectedFigures_.isEmpty()) return;

    QList<Figure*> copies;

    for (Figure *original : selectedFigures_) {
        Figure *copy = original->clone();
        if (!copy) continue;

        
        copy->setPos(original->pos() + QPointF(20, 20));

        
        addItem(copy);
        if (currentLayer())
            currentLayer()->addFigure(copy);

        copies.append(copy);
    }

    if (copies.isEmpty()) return;

    pushUndoState();

    
    for (Figure *original : selectedFigures_) {
        original->un_select();
        original->update();
    }
    clearSelection();

    
    for (Figure *copy : copies) {
        addToSelection(copy);
        copy->update();
    }

    update();
}


void Scene::pushUndoState()
{
    QList<UndoStateItem> state;
    foreach (QGraphicsItem* item, items()) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig && fig->scene() == this) {
            int layerIndex = -1;
            if (fig->layer()) {
                layerIndex = layers_.indexOf(fig->layer());
            } else {
                qWarning() << "pushUndoState: figure has no layer, skipping?";
                continue;
            }
            state.append(UndoStateItem(fig->clone(), layerIndex));
        }
    }
    undoStack_.append(state);
    redoStack_.clear();
}

void Scene::restoreState(const QList<UndoStateItem>& state)
{
    qDebug() << "restoreState: restoring" << state.size() << "figures";

    clearSelection();

    if (isBuildingPolyline_ && buildingPolyline_) {
        delete buildingPolyline_;
        buildingPolyline_ = nullptr;
        isBuildingPolyline_ = false;
    }
    currentFigure_ = nullptr;
    currentAdded_ = false;
    mode_ = kDrawing;

    for (Layer* layer : layers_) {
        layer->clear();
    }
    clear();

    for (const UndoStateItem& item : state) {
        Figure* fig = item.figure;
        int layerIndex = item.layerIndex;

        Layer* targetLayer = nullptr;
        if (layerIndex >= 0 && layerIndex < layers_.size()) {
            targetLayer = layers_[layerIndex];
        } else {
            targetLayer = currentLayer();
            if (!targetLayer && !layers_.isEmpty())
                targetLayer = layers_.first();
        }

        if (targetLayer) {
            targetLayer->addFigure(fig);
        } else {
            qWarning() << "No target layer for figure!";
        }
        
        
        fig->prepareGeometryChangePublic(); 
        addItem(fig);
        
        
        fig->updateTransformOriginPoint();
        fig->update();
    }

    update();
}

void Scene::undo()
{
    if (undoStack_.isEmpty()) return;

    QList<UndoStateItem> currentState;
    foreach (QGraphicsItem* item, items()) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig) {
            int layerIndex = layers_.indexOf(fig->layer());
            currentState.append(UndoStateItem(fig->clone(), layerIndex));
        }
    }
    redoStack_.append(currentState);

    QList<UndoStateItem> prevState = undoStack_.takeLast();
    restoreState(prevState);
    
    
    currentFigure_ = nullptr;
    currentAdded_ = false;
    isBuildingPolyline_ = false;
    buildingPolyline_ = nullptr;
}

void Scene::redo()
{
    if (redoStack_.isEmpty()) return;

    QList<UndoStateItem> currentState;
    foreach (QGraphicsItem* item, items()) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig) {
            int layerIndex = layers_.indexOf(fig->layer());
            currentState.append(UndoStateItem(fig->clone(), layerIndex));
        }
    }
    undoStack_.append(currentState);

    QList<UndoStateItem> nextState = redoStack_.takeLast();
    restoreState(nextState);
    
    
    currentFigure_ = nullptr;
    currentAdded_ = false;
    isBuildingPolyline_ = false;
    buildingPolyline_ = nullptr;
}

void Scene::clearSelection()
{
    for (Figure *fig : selectedFigures_) {
        fig->un_select();
        fig->update();
    }
    selectedFigures_.clear();
}

void Scene::addToSelection(Figure *fig)
{
    if (!selectedFigures_.contains(fig)) {
        selectedFigures_.append(fig);
        fig->select();
        fig->update();
    }
}

void Scene::removeFromSelection(Figure *fig)
{
    if (selectedFigures_.removeOne(fig)) {
        fig->un_select();
        fig->update();
    }
}

bool Scene::isSelected(Figure *fig) const
{
    return selectedFigures_.contains(fig);
}

void Scene::applyToAllSelected(std::function<void(Figure*)> func)
{
    for (Figure *fig : selectedFigures_) {
        func(fig);
    }
}


void Scene::addLayer(const QString &name)
{
    Layer *layer = new Layer(name, this);
    layers_.append(layer);
    updateZOrder();
    emit layersChanged();
}

void Scene::removeLayer(int index) {
    if (index < 0 || index >= layers_.size()) return;

    Layer *layer = layers_[index];

    
    for (Figure *fig : layer->figures()) {
        if (selectedFigures_.contains(fig)) {
            selectedFigures_.removeOne(fig);
            fig->un_select();
        }
    }

    
    layers_.removeAt(index);

    
    for (Figure *figure : layer->figures()) {
        if (figure->scene() == this) {
            removeItem(figure);
        }
        delete figure;
    }

    
    layer->clear();

    
    delete layer;

    
    if (layers_.isEmpty()) {
        addLayer("Layer 1");
        currentLayerIndex_ = 0;
    } else {
        if (currentLayerIndex_ >= layers_.size()) {
            currentLayerIndex_ = layers_.size() - 1;
        } else if (index <= currentLayerIndex_) {
            currentLayerIndex_ = currentLayerIndex_ - 1;
            if (currentLayerIndex_ < 0) currentLayerIndex_ = 0;
        }
    }

    updateZOrder();
    emit layersChanged();
}

void Scene::moveLayerUp(int index)
{
    if (index <= 0 || index >= layers_.size()) return;
    std::swap(layers_[index], layers_[index-1]);
    updateZOrder();
    emit layersChanged();
}

void Scene::moveLayerDown(int index)
{
    if (index < 0 || index >= layers_.size()-1) return;
    std::swap(layers_[index], layers_[index+1]);
    updateZOrder();
    emit layersChanged();
}

void Scene::setLayerVisible(int index, bool visible)
{
    if (index >= 0 && index < layers_.size())
        layers_[index]->setVisible(visible);
}

QList<Layer*> Scene::layers() const
{
    return layers_;
}

Layer* Scene::currentLayer() const
{
    if (currentLayerIndex_ >= 0 && currentLayerIndex_ < layers_.size())
        return layers_[currentLayerIndex_];
    return nullptr;
}

void Scene::setCurrentLayer(int index)
{
    if (index >= 0 && index < layers_.size())
        currentLayerIndex_ = index;
}

void Scene::moveFigureToLayer(Figure *figure, int layerIndex)
{
    if (!figure) return;
    Layer *oldLayer = figure->layer();
    if (oldLayer) {
        oldLayer->removeFigure(figure);
    }
    if (layerIndex >= 0 && layerIndex < layers_.size()) {
        layers_[layerIndex]->addFigure(figure);
    }
    updateZOrder();
}

void Scene::updateZOrder()
{
    const int layerCount = layers_.size();
    for (int i = 0; i < layerCount; ++i) {
        
        int zBase = (layerCount - 1 - i) * 1000;
        layers_[i]->setZBase(zBase);
    }
}


void Scene::rotateSelected(qreal deltaAngle)
{
    if (selectedFigures_.isEmpty()) return;

    
    for (Figure *fig : selectedFigures_) {
        fig->setTransformOriginPoint(fig->boundingRect().center());
        fig->setRotation(fig->rotation() + deltaAngle);
    }
    update();
}



void Scene::moveSelectedFiguresToLayer(int layerIndex)
{
    if (layerIndex < 0 || layerIndex >= layers_.size()) return;
    for (Figure *fig : selectedFigures_) {
        moveFigureToLayer(fig, layerIndex);
    }
    update();
}




void Scene::scaleSelectedFigures(qreal factor)
{
    if (selectedFigures_.isEmpty()) return;
    
    for (Figure *fig : selectedFigures_) {
        FigureType type = fig->getFigureType();
        
        if (type == kLine) {
            Line *line = dynamic_cast<Line*>(fig);
            if (line) {
                QPointF center = line->boundingRect().center();
                QPointF start = line->getStartPoint();
                QPointF end = line->getEndPoint();
                QPointF newStart = center + (start - center) * factor;
                QPointF newEnd = center + (end - center) * factor;
                line->setStartPoint(newStart);
                line->setEndPoint(newEnd);
            }
        }
        else if (type == kPolyline) {
            Polyline *polyline = dynamic_cast<Polyline*>(fig);
            if (polyline) {
                QPointF center = polyline->boundingRect().center();
                QPolygonF points = polyline->points();
                for (int i = 0; i < points.size(); ++i) {
                    points[i] = center + (points[i] - center) * factor;
                }
                polyline->setPoints(points);
            }
        }
        else {
            
            QPointF center = fig->boundingRect().center();
            QPointF start = fig->getStartPoint();
            QPointF end = fig->getEndPoint();
            QPointF newStart = center + (start - center) * factor;
            QPointF newEnd = center + (end - center) * factor;
            fig->setStartPoint(newStart);
            fig->setEndPoint(newEnd);
        }
        
        fig->updateTransformOriginPoint();
        fig->update();
    }
    
    update();
}
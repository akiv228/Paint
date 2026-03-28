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
    applyToAllSelected([color](Figure *fig) {
        fig->setPenColor(color);
        fig->update();
    });
}

void Scene::setBrushColor(QColor color)
{
    currentBrushColor_ = color;
    applyToAllSelected([color](Figure *fig) {
        fig->setBrushColor(color);
        fig->update();
    });
}

void Scene::setPenWidth(qreal width)
{
    currentPenWidth_ = width;
    applyToAllSelected([width](Figure *fig) {
        fig->setPenWidth(width);
        fig->update();
    });
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
    Figure *temp_figure = figureAt(event->scenePos());
    clearSelection();
    if (temp_figure != nullptr) {
        addToSelection(temp_figure);
        pressPos_ = event->scenePos();
        selectedStartPoint_ = temp_figure->getStartPoint();
        selectedEndPoint_ = temp_figure->getEndPoint();
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

    QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
    Figure *clicked = nullptr;
    for (QGraphicsItem *item : itemsAtPos) {
        Figure *fig = dynamic_cast<Figure*>(item);
        if (fig && fig->layer() == currentLayer()) {
            clicked = fig;
            break;
        }
    }

    bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);

    if (!shiftPressed) {
        clearSelection();
    }
    if (clicked != nullptr) {
        if (shiftPressed) {
            if (isSelected(clicked))
                removeFromSelection(clicked);
            else
                addToSelection(clicked);
        } else {
            addToSelection(clicked);
        }
    }

    if (!selectedFigures_.isEmpty() && clicked != nullptr && isSelected(clicked)) {
        Figure *selected = clicked;
        QPointF local_pos = selected->mapFromScene(event->scenePos());
        if (onRectBoard(selected->boundingRect(), local_pos, currentSide_)) {
            mode_ = kChanging;
            selectedStartPoint_ = selected->getStartPoint();
            selectedEndPoint_ = selected->getEndPoint();
        } else if (selected->shape().contains(local_pos)) {
            mode_ = kMoving;
            pressPos_ = event->scenePos();
        } else {
            if (selectedFigures_.size() == 1) {
                rotationCenter_ = selected->mapToScene(selected->boundingRect().center());
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
        }
        selected->update();
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
    if (currentLayer())
        currentLayer()->addFigure(currentFigure_);
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
                qreal rad = angle_delta * M_PI / 180.0;
                qreal cosA = cos(rad);
                qreal sinA = sin(rad);

                if (selectedFigures_.size() == 1) {
                    // Для одной фигуры – только поворот вокруг её центра
                    Figure *fig = selectedFigures_.first();
                    fig->setRotation(fig->rotation() + angle_delta);
                } else {
                    // Для группы – поворачиваем каждую фигуру вокруг общего центра
                    for (Figure *fig : selectedFigures_) {
                        QPointF pos = fig->pos();
                        QPointF vec = pos - rotationCenter_;
                        QPointF newVec(vec.x() * cosA - vec.y() * sinA,
                                    vec.x() * sinA + vec.y() * cosA);
                        QPointF newPos = rotationCenter_ + newVec;
                        fig->setPos(newPos);
                        fig->setRotation(fig->rotation() + angle_delta);
                    }
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
    if (!selectedFigures_.isEmpty() && mode_ == kRotating && qAbs(pressPos_.x() - event->scenePos().x()) < 10 && qAbs(pressPos_.y() - event->scenePos().y()) < 10)
    {
        clearSelection();
    }
    mode_ = kDrawing;
    selectedPointIndex_ = 0;

    if (currentAdded_ && currentFigure_ && !isBuildingPolyline_)
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
    clearSelection();
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
    Figure *original = selectedFigures_.first();
    Figure *copy = nullptr;

    FigureType type = original->getFigureType();

    switch (type) {
    case kTriangle:
        copy = new Triangle(original->getStartPoint());
        break;
    case kCircle:
        copy = new Circle(original->getStartPoint());
        break;
    case kRhombus:
        copy = new Rhombus(original->getStartPoint());
        break;
    case kSquare:
        copy = new Square(original->getStartPoint());
        break;
    case kRectangle:
        copy = new Rectangle(original->getStartPoint());
        break;
    case kStar:
        copy = new Star(original->getStartPoint());
        break;
    case kPolygon:
        copy = new Polygon(original->getStartPoint());
        break;
    case kTrapezoid:
        copy = new Trapezoid(original->getStartPoint());
        break;
    case kLine:
        copy = new Line(original->getStartPoint());
        break;
    case kPolyline:
        {
            Polyline *origPoly = dynamic_cast<Polyline*>(original);
            if (origPoly) {
                copy = new Polyline(original->getStartPoint());
                Polyline *copyPoly = dynamic_cast<Polyline*>(copy);
                if (copyPoly) {
                    copyPoly->setPoints(origPoly->points());
                }
            }
        }
        break;
    case kEllipse:
        copy = new Ellipse(original->getStartPoint());
        break;
    default:
        return;
    }

    if (copy == nullptr) return;

    copy->setPenColor(original->getPenColor());
    copy->setBrushColor(original->getBrushColor());
    copy->setPenWidth(original->getPenWidth());
    copy->setStartPoint(original->getStartPoint());
    copy->setEndPoint(original->getEndPoint());
    copy->setPos(original->pos());
    copy->setRotation(original->rotation());
    copy->setTransformOriginPoint(original->transformOriginPoint());

    copy->setPos(copy->pos() + QPointF(20, 20));

    this->addItem(copy);
    if (currentLayer())
        currentLayer()->addFigure(copy);
    pushUndoState();

    original->un_select();
    original->update();

    clearSelection();
    addToSelection(copy);
}

void Scene::pushUndoState()
{
    QList<Figure*> state;
    foreach (QGraphicsItem* item, items()) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig)
            state.append(fig->clone());
    }
    undoStack_.append(state);
    redoStack_.clear();
}

void Scene::restoreState(const QList<Figure*>& state)
{
    clear();
    clearSelection();
    foreach (Figure* fig, state) {
        addItem(fig);
    }
    update();
}

void Scene::undo()
{
    if (undoStack_.isEmpty()) return;
    QList<Figure*> currentState;
    foreach (QGraphicsItem* item, items()) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig)
            currentState.append(fig->clone());
    }
    redoStack_.append(currentState);
    QList<Figure*> prevState = undoStack_.takeLast();
    restoreState(prevState);
}

void Scene::redo()
{
    if (redoStack_.isEmpty()) return;
    QList<Figure*> currentState;
    foreach (QGraphicsItem* item, items()) {
        Figure* fig = dynamic_cast<Figure*>(item);
        if (fig)
            currentState.append(fig->clone());
    }
    undoStack_.append(currentState);
    QList<Figure*> nextState = redoStack_.takeLast();
    restoreState(nextState);
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
    Layer *layer = layers_.takeAt(index);
    for (Figure *figure : layer->figures()) {
        if (figure->scene() == this) {
            removeItem(figure);
        }
        delete figure;
    }
    delete layer;
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
    for (int i = 0; i < layers_.size(); ++i) {
        layers_[i]->setZBase(i * 1000);
    }
}


void Scene::rotateSelected(qreal deltaAngle)
{
    if (selectedFigures_.isEmpty()) return;

    if (selectedFigures_.size() == 1) {
        // Для одной фигуры – поворачиваем вокруг её центра
        Figure *fig = selectedFigures_.first();
        fig->setRotation(fig->rotation() + deltaAngle);
    } else {
        // Для группы – вычисляем общий центр и поворачиваем все фигуры вокруг него
        QRectF totalRect;
        for (Figure *fig : selectedFigures_) {
            totalRect = totalRect.united(fig->mapToScene(fig->boundingRect()).boundingRect());
        }
        QPointF center = totalRect.center();

        qreal rad = deltaAngle * M_PI / 180.0;
        qreal cosA = std::cos(rad);
        qreal sinA = std::sin(rad);

        for (Figure *fig : selectedFigures_) {
            QPointF pos = fig->pos();
            QPointF vec = pos - center;
            QPointF newVec(vec.x() * cosA - vec.y() * sinA,
                           vec.x() * sinA + vec.y() * cosA);
            QPointF newPos = center + newVec;
            fig->setPos(newPos);
            fig->setRotation(fig->rotation() + deltaAngle);
        }
    }
    update();
}
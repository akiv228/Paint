#include "scene.h"

#include "circle.h"
#include "polygon.h"
#include "rectangle.h"
#include "rhombus.h"
#include "square.h"
#include "star.h"
#include "trapezoid.h"
#include "triangle.h"

#include <QGraphicsView>
#include <QInputDialog>
#include <qdebug.h>
#include <cmath>

Scene::Scene(QObject *parent)
    : QGraphicsScene{parent}
{}

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
}

void Scene::setBrushColor(QColor color)
{
    currentBrushColor_ = color;
}

void Scene::setPenWidth(qreal width)
{
    currentPenWidth_ = width;
}

qreal Scene::getPenWidth()
{
    return currentPenWidth_;
}

void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Figure *temp_figure = figureAt(event->scenePos()); //
    if (selectedFigure_ != nullptr && selectedFigure_ != temp_figure) {
        pressPos_ = event->scenePos();
        selectedFigure_->un_select();
        selectedFigure_->update(); //перерисовать без рамки
    }
    selectedFigure_ = temp_figure;
    if (selectedFigure_ != nullptr) {
        pressPos_ = event->scenePos();
        selectedStartPoint_ = selectedFigure_->getStartPoint();
        selectedEndPoint_ = selectedFigure_->getEndPoint();
        selectedFigure_->select();
        selectedFigure_->update();
    }
    this->update(QRectF(0, 0, this->width(), this->height())); //обновляем сцену убираем артефакты
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected())
    {

        pressPos_ = event->scenePos(); // позиции нажатия в сценовых координатах
        QPointF local_pos = selectedFigure_->mapFromScene(pressPos_);
        selectedStartPoint_ = selectedFigure_->getStartPoint();
        selectedEndPoint_ = selectedFigure_->getEndPoint();
        if (onRectBoard(selectedFigure_->boundingRect(), local_pos, currentSide_)) {
            mode_ = kChanging;
            qDebug() << "Mode: kChanging";
        } else if (selectedFigure_->shape().contains(local_pos)) {
            mode_ = kMoving;
            qDebug() << "Mode: kMoving";
        }
        else {
            QPointF center = selectedFigure_->mapToScene(selectedFigure_->boundingRect().center());
            QPointF start_vector = event->scenePos() - center; // Вектор от центра до точки нажатия
            startAngle_ = std::atan2(start_vector.y(), start_vector.x()) * 180
                          / M_PI;                         // Угол в градусах
            startRotation_ = selectedFigure_->rotation(); // Текущий угол фигуры
            pressPos_ = event->scenePos();
            mode_ = kRotating; // Режим вращения
            qDebug() << "Mode: kRotating";
        }
        selectedFigure_->update();
    }
    //создается новая фигура относит сцен
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
    default:
        currentFigure_ = new Triangle(event->scenePos());
        break;
    }
    currentFigure_->setPos(event->pos());
    currentFigure_->setPenColor(currentPenColor_);
    currentFigure_->setBrushColor(currentBrushColor_);
    currentFigure_->setPenWidth(currentPenWidth_);
    currentAdded_ = false; // если случайно просто нажали чтобы не рисовались маленькие фигурки
    this->update(QRectF(0, 0, this->width(), this->height()));
}



void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected())
    {

        if (mode_ == kChanging)
        {

            qreal dx = event->scenePos().x() - pressPos_.x(); //расстояние межжду позицией на которую нажал и текущей
            qreal dy = event->scenePos().y() - pressPos_.y();
            FigureType selected_figure_type = selectedFigure_->getFigureType();

            if (selected_figure_type == kCircle || selected_figure_type == kStar
                || selected_figure_type == kPolygon) {
                qreal radius = std::sqrt(std::pow(selectedEndPoint_.x() - selectedStartPoint_.x(), 2) + std::pow(selectedEndPoint_.y() - selectedStartPoint_.y(), 2));
                switch (currentSide_)
                {
                case 0:
                    selectedEndPoint_.y() > selectedStartPoint_.y() ? selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dy / radius)))
                                                                  : selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dy / radius)));
                    break;
                case 1:
                    selectedEndPoint_.y() > selectedStartPoint_.y() ? selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dx / radius)))
                                                                  : selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dx / radius)));
                    break;
                case 2:
                    selectedEndPoint_.y() > selectedStartPoint_.y() ? selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dy / radius)))
                                                                  : selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dy / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dy / radius)));
                    break;
                case 3:
                    selectedEndPoint_.y() > selectedStartPoint_.y() ? selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 - dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 - dx / radius)))
                                                                  : selectedFigure_->setEndPoint(QPointF(selectedStartPoint_.x() + (selectedEndPoint_.x() - selectedStartPoint_.x()) * (1 + dx / radius), selectedStartPoint_.y() + (selectedEndPoint_.y() - selectedStartPoint_.y()) * (1 + dx / radius)));
                    break;
                default:
                    break;
                }
            } else {
                switch (currentSide_)
                {
                case 0:
                    selectedEndPoint_.y() > selectedStartPoint_.y() ? selectedFigure_->setStartPoint(QPointF(selectedStartPoint_.x(), selectedStartPoint_.y() + dy))
                                                                  : selectedFigure_->setEndPoint(QPointF(selectedEndPoint_.x(), selectedEndPoint_.y() + dy));
                    break;
                case 1:
                    selectedEndPoint_.x() > selectedStartPoint_.x() ? selectedFigure_->setEndPoint(QPointF(selectedEndPoint_.x() + dx, selectedEndPoint_.y()))
                                                                  : selectedFigure_->setStartPoint(QPointF(selectedStartPoint_.x() + dx, selectedStartPoint_.y()));
                    break;
                case 2:
                    selectedEndPoint_.y() > selectedStartPoint_.y() ? selectedFigure_->setEndPoint(QPointF(selectedEndPoint_.x(), selectedEndPoint_.y() + dy))
                                                                  : selectedFigure_->setStartPoint(QPointF(selectedStartPoint_.x(), selectedStartPoint_.y() + dy));
                    break;
                case 3:
                    selectedEndPoint_.x() > selectedStartPoint_.x() ? selectedFigure_->setStartPoint(QPointF(selectedStartPoint_.x() + dx, selectedStartPoint_.y()))
                                                                  : selectedFigure_->setEndPoint(QPointF(selectedEndPoint_.x() + dx, selectedEndPoint_.y()));
                    break;
                default:
                    break;
                }
            }
        }

        else if (mode_ == kMoving)
        {
            QPointF current_pos = event->scenePos();
            QPointF delta = current_pos - pressPos_;
            selectedFigure_->setPos(selectedFigure_->pos() + delta);
            pressPos_ = current_pos;
        }

        else if (mode_ == kRotating)
        {
            QPointF center = selectedFigure_->mapToScene(selectedFigure_->boundingRect().center());
            QPointF current_vector = event->scenePos()
                                     - center; // Вектор от центра до текущей позиции
            qreal current_angle = std::atan2(current_vector.y(), current_vector.x()) * 180
                                  / M_PI;             //  вычисляет угол между положительной осью X и вектором (x, y) в радианах, учитывая квадрант:
            // kурсор справа от центра: угол 0° Сверху: 90°, слева: 180°, снизу: -90°


            qreal angle_delta = current_angle - startAngle_;
            // Нормализуем угол [-180, 180]
            if (angle_delta > 180) {
                angle_delta -= 360;
            } else if (angle_delta < -180) {
                angle_delta += 360;
            }

            selectedFigure_->setTransformOriginPoint(selectedFigure_->boundingRect().center()); // Центр в локальных координатах
            selectedFigure_->setRotation(startRotation_ + angle_delta);
            selectedFigure_->update();
        }

        selectedFigure_->update();
    }
    else
    {
        if (!currentAdded_)
        {
            this->addItem(currentFigure_);
            currentAdded_ = true;
        }
        currentFigure_->setEndPoint(event->scenePos());
    }
    this->update(QRectF(0, 0, this->width(), this->height()));
}



void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected() && mode_ == kRotating && qAbs(pressPos_.x() - event->scenePos().x()) < 10 && qAbs(pressPos_.y() - event->scenePos().y()) < 10) // если маленькое перемещение это не вращение а снятие выделения
    {
        selectedFigure_->un_select();
        selectedFigure_->update();
    }
    mode_ = kDrawing;
}


Figure *Scene::figureAt(QPointF point) // возвращает указатель
{
    foreach (QGraphicsItem *item, this->items()) // список всех фигур на сцене.
    {
        QPointF local_point = item->mapFromScene(point);
        if (item->shape().contains(local_point)) { // shape() возвращает  форму фигуры, а contains проверяет попадает ли точка в эту форму
            return dynamic_cast<Figure *>(item); //  Преобразует указатель на QGraphicsItem в указатель на Figure
        }
    }
    return nullptr;
}


void Scene::deleteSelectedFigure()
{
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected())
    {
        this->removeItem(selectedFigure_);
        delete selectedFigure_;
        selectedFigure_ = nullptr;
    }
    this->update(QRectF(0, 0, this->width(), this->height()));
}

qreal Scene::getSelectedFigurePerimeter(bool &ok)
{
    ok = true;
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected()) {
        return selectedFigure_->getPerimeter();
    }
    ok = false;
    return 0;
}

qreal Scene::getSelectedFigureSquare(bool &ok)
{
    ok = true;
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected()) {
        return selectedFigure_->getSquare();
    }
    ok = false;
    return 0;
}

void Scene::removeAllItems()
{
    selectedFigure_ = nullptr;
    clear();
    update(QRectF(0, 0, width(), height()));
}

void Scene::un_select()
{
    if (selectedFigure_ != nullptr && selectedFigure_->is_selected())
    {
        selectedFigure_->un_select();
        selectedFigure_->update();
    }
}


// граничная точка
bool Scene::onRectBoard(QRectF rect, QPointF point, int &side)
{
    side = -1;
    qreal offset = 30; // Допустимое расстояние от границы в пикселях
    if ((rect.contains(point) &&
        (
            !rect.contains(QPointF(point.x() + offset, point.y())) ||
            !rect.contains(QPointF(point.x() - offset, point.y())) || //точка внутри прямоугольника, но близко к границе
            !rect.contains(QPointF(point.x(), point.y() + offset)) ||
            !rect.contains(QPointF(point.x(), point.y() - offset))
                                  )) ||
        (!rect.contains(point) &&
            (
                rect.contains(QPointF(point.x() + offset, point.y())) || // nочка снаружи прямоугольника, но близко к границе
                rect.contains(QPointF(point.x() - offset, point.y())) ||
                rect.contains(QPointF(point.x(), point.y() + offset)) ||
                rect.contains(QPointF(point.x(), point.y() - offset))))
        )
    {
        if ((rect.contains(point) && !rect.contains(QPointF(point.x(), point.y() - offset)))
            || (!rect.contains(point) && rect.contains(QPointF(point.x(), point.y() + offset)))) {
            side = 0; // вверх
            // точка внутри: смещение вверх выходит за границу
            // nочка снаружи: cvещение вниз попадает внутрь
        }
        if ((rect.contains(point) && !rect.contains(QPointF(point.x() + offset, point.y())))
            || (!rect.contains(point) && rect.contains(QPointF(point.x() - offset, point.y())))) {
            side = 1; // вправо
            // точка внутри: смещение вправо выходит за границу  точка снаружи: смещение влево попадает внутрь
        }
        if ((rect.contains(point) && !rect.contains(QPointF(point.x(), point.y() + offset)))
            || (!rect.contains(point) && rect.contains(QPointF(point.x(), point.y() - offset)))) {
            side = 2; // низ
        }
        if ((rect.contains(point) && !rect.contains(QPointF(point.x() - offset, point.y())))
            || (!rect.contains(point) && rect.contains(QPointF(point.x() + offset, point.y())))) {
            side = 3; // лево
        }
        return true;
    }
    return false;
}


// bool Scene::onRectBoard(QRectF rect, QPointF localPoint, int &side) {
//     qreal offset = 10; // Допустимое расстояние от границы
//     QRectF expandedRect = rect.adjusted(-offset, -offset, offset, offset);

//     if (!expandedRect.contains(localPoint)) return false;

//     // Определяем сторону нажатия
//     if (localPoint.y() <= rect.top() + offset) side = 0;    // Верх
//     else if (localPoint.x() >= rect.right() - offset) side = 1; // Право
//     else if (localPoint.y() >= rect.bottom() - offset) side = 2; // Низ
//     else if (localPoint.x() <= rect.left() + offset) side = 3;  // Лево

//     return true;
// }

#include "layer.h"
#include "figure.h"

Layer::Layer(const QString &name, QObject *parent)
    : QObject(parent), name_(name), visible_(true), zBase_(0)
{
}

Layer::~Layer()
{
    foreach (Figure *fig, figures_) {
        delete fig;
    }
}

QString Layer::name() const
{
    return name_;
}

void Layer::setName(const QString &name)
{
    name_ = name;
}

bool Layer::isVisible() const
{
    return visible_;
}

void Layer::setVisible(bool visible)
{
    visible_ = visible;
    foreach (Figure *fig, figures_) {
        fig->setVisible(visible_);
    }
}

QList<Figure*> Layer::figures() const
{
    return figures_;
}

void Layer::addFigure(Figure *figure)
{
    if (!figures_.contains(figure)) {
        figures_.append(figure);
        figure->setLayer(this);
        figure->setVisible(visible_);
        figure->setZValue(zBase_ + figures_.size() - 1);
    }
}

void Layer::removeFigure(Figure *figure)
{
    if (figures_.removeOne(figure)) {
        figure->setLayer(nullptr);
        for (int i = 0; i < figures_.size(); ++i) {
            figures_[i]->setZValue(zBase_ + i);
        }
    }
}

void Layer::clear()
{
    // foreach (Figure *fig, figures_) {
    //     delete fig;
    // }
    figures_.clear();
}


void Layer::setZBase(int zBase)
{
    zBase_ = zBase;
    for (int i = 0; i < figures_.size(); ++i) {
        figures_[i]->setZValue(zBase_ + i);
    }
}

int Layer::zBase() const
{
    return zBase_;
}
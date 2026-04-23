#ifndef FIGUREPLUGININTERFACE_H
#define FIGUREPLUGININTERFACE_H

#include <QtPlugin>
#include <QString>
#include <QPointF>
#include <QJsonObject>

class Figure;

class FigurePluginInterface
{
public:
    virtual ~FigurePluginInterface() = default;
    virtual QString figureTypeId() const = 0;
    virtual QString displayName() const = 0;
    virtual QString iconSymbol() const = 0;      // новый метод
    virtual Figure* createFigure(const QPointF& startPoint) const = 0;
    virtual Figure* createFromJson(const QJsonObject& obj) const = 0;
};

Q_DECLARE_INTERFACE(FigurePluginInterface, "com.paint.FigurePluginInterface/1.1")

#endif
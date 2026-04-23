#ifndef TRAPEZOIDEXPLUGIN_H
#define TRAPEZOIDEXPLUGIN_H

#include <QObject>
#include "figureplugininterface.h"
#include "trapezoidex.h"

class TrapezoidExPlugin : public QObject, public FigurePluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.paint.FigurePluginInterface" FILE "trapezoidex.json")
    Q_INTERFACES(FigurePluginInterface)

public:
    QString figureTypeId() const override { return "TrapezoidEx"; }
    QString displayName() const override { return "Trapezoid (Plugin)"; }
    QString iconSymbol() const override { return "⏢"; }
    Figure* createFigure(const QPointF& startPoint) const override {
        return new TrapezoidEx(startPoint);
    }
    Figure* createFromJson(const QJsonObject& obj) const override {
        TrapezoidEx* fig = new TrapezoidEx(QPointF());
        fig->fromJson(obj);
        return fig;
    }
};

#endif

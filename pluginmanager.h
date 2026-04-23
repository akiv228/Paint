#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <functional>
#include <QPluginLoader>
#include "figureplugininterface.h"

class Figure;
class QPointF;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    static PluginManager* instance();
    void loadPlugins();
    void registerBuiltInFigure(const QString& typeId, const QString& displayName,
                               std::function<Figure*(const QPointF&)> factory,
                               std::function<Figure*(const QJsonObject&)> jsonFactory);
    QStringList availableFigureTypes() const;
    QString displayName(const QString& typeId) const;
    Figure* createFigure(const QString& typeId, const QPointF& startPoint) const;
    Figure* createFromJson(const QJsonObject& obj) const;

    void registerExternalPlugin(const QString& typeId, const QString& displayName,
                                QObject* pluginInstance, FigurePluginInterface* interface,
                                QPluginLoader* loader);
    QString iconSymbol(const QString& typeId) const;

signals:
    void pluginsChanged();

private:
    PluginManager(QObject *parent = nullptr);
    ~PluginManager();
    struct PluginInfo {
        QObject* pluginInstance = nullptr;
        FigurePluginInterface* interface = nullptr;
        std::function<Figure*(const QPointF&)> builtInFactory;
        std::function<Figure*(const QJsonObject&)> builtInJsonFactory;
        QString displayName;
        bool isBuiltIn = false;
        QPluginLoader* loader = nullptr;
    };
    QMap<QString, PluginInfo> plugins_;
};

#endif
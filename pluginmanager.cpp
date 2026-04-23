#include "pluginmanager.h"
#include "figure.h"
#include <QDir>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QDebug>

PluginManager* PluginManager::instance()
{
    static PluginManager manager;
    return &manager;
}

PluginManager::PluginManager(QObject *parent) : QObject(parent) {}

PluginManager::~PluginManager()
{
    for (auto it = plugins_.begin(); it != plugins_.end(); ++it) {
        if (!it->isBuiltIn && it->loader) {
            it->loader->unload();
            delete it->loader;
        }
    }
}

void PluginManager::loadPlugins()
{
    QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
    pluginsDir.cd("plugins");
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cd("PlugIns");
    }
#else
    pluginsDir.cd("plugins");
#endif

    qDebug() << "=== PluginManager: searching plugins in:" << pluginsDir.absolutePath();

    if (!pluginsDir.exists()) {
        qWarning() << "Plugin directory does not exist!";
        return;
    }

    const QFileInfoList files = pluginsDir.entryInfoList(QDir::Files);
    qDebug() << "Found" << files.size() << "files:" << files;

    for (const QFileInfo& fileInfo : files) {
        const QString fileName = fileInfo.absoluteFilePath();
        QPluginLoader* loader = new QPluginLoader(fileName);
        qDebug() << "Trying to load:" << fileName;

        QObject* plugin = loader->instance();
        if (plugin) {
            FigurePluginInterface* iface = qobject_cast<FigurePluginInterface*>(plugin);
            if (iface) {
                QString typeId = iface->figureTypeId();
                qDebug() << "Successfully loaded plugin:" << typeId << "display name:" << iface->displayName();
                if (!plugins_.contains(typeId)) {
                    PluginInfo info;
                    info.pluginInstance = plugin;
                    info.interface = iface;
                    info.isBuiltIn = false;
                    info.displayName = iface->displayName();
                    info.loader = loader;   // сохраняем loader
                    plugins_[typeId] = info;
                } else {
                    loader->unload();
                    delete loader;
                }
            } else {
                qWarning() << "Plugin does not implement FigurePluginInterface:" << fileName;
                loader->unload();
                delete loader;
            }
        } else {
            qWarning() << "Failed to load plugin:" << fileName << "Error:" << loader->errorString();
            delete loader;
        }
    }

    emit pluginsChanged();
}

void PluginManager::registerBuiltInFigure(const QString& typeId, const QString& displayName,
                                          std::function<Figure*(const QPointF&)> factory,
                                          std::function<Figure*(const QJsonObject&)> jsonFactory)
{
    if (plugins_.contains(typeId)) return;
    PluginInfo info;
    info.isBuiltIn = true;
    info.displayName = displayName;
    info.builtInFactory = factory;
    info.builtInJsonFactory = jsonFactory;
    plugins_[typeId] = info;
}

void PluginManager::registerExternalPlugin(const QString& typeId, const QString& displayName,
                                           QObject* pluginInstance, FigurePluginInterface* interface,
                                           QPluginLoader* loader)
{
    if (plugins_.contains(typeId)) {
        qWarning() << "PluginManager: typeId" << typeId << "already registered";
        return;
    }
    PluginInfo info;
    info.pluginInstance = pluginInstance;
    info.interface = interface;
    info.isBuiltIn = false;
    info.displayName = displayName;
    info.loader = loader;
    plugins_[typeId] = info;
    qDebug() << "PluginManager: registered external plugin" << typeId;
    emit pluginsChanged();
}

QStringList PluginManager::availableFigureTypes() const
{
    return plugins_.keys();
}

QString PluginManager::displayName(const QString& typeId) const
{
    auto it = plugins_.find(typeId);
    return (it != plugins_.end()) ? it->displayName : QString();
}

Figure* PluginManager::createFigure(const QString& typeId, const QPointF& startPoint) const
{
    auto it = plugins_.find(typeId);
    if (it == plugins_.end()) return nullptr;
    if (it->isBuiltIn && it->builtInFactory)
        return it->builtInFactory(startPoint);
    else if (it->interface)
        return it->interface->createFigure(startPoint);
    return nullptr;
}

Figure* PluginManager::createFromJson(const QJsonObject& obj) const
{
    QString typeId = obj["typeId"].toString();
    auto it = plugins_.find(typeId);
    if (it == plugins_.end()) return nullptr;
    if (it->isBuiltIn && it->builtInJsonFactory)
        return it->builtInJsonFactory(obj);
    else if (it->interface)
        return it->interface->createFromJson(obj);
    return nullptr;
}


QString PluginManager::iconSymbol(const QString& typeId) const
{
    auto it = plugins_.find(typeId);
    if (it != plugins_.end()) {
        if (it->isBuiltIn) {
            if (typeId == "Triangle") return "△";
            if (typeId == "Circle") return "◯";
            if (typeId == "Rhombus") return "♢";
            if (typeId == "Square") return "□";
            if (typeId == "Rectangle") return "▭";
            if (typeId == "Star") return "☆";
            if (typeId == "Polygon") return "⬡";
            if (typeId == "Parallelogram") return "▱";
            if (typeId == "Line") return "—";
            if (typeId == "Polyline") return "⌇";
            if (typeId == "Ellipse") return "⬭";
            return "?";
        } else if (it->interface) {
            return it->interface->iconSymbol();
        }
    }
    return QString();
}
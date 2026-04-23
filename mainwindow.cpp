#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "pluginmanager.h"
#include "triangle.h"
#include "circle.h"
#include "rhombus.h"
#include "square.h"
#include "rectangle.h"
#include "star.h"
#include "polygon.h"
#include "parallelogram.h"
#include "line.h"
#include "polyline.h"
#include "ellipse.h"
#include "constants.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QMessageBox>
#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    this->setWindowTitle("Paint");
    this->setStyleSheet("background-color: #181818; color: white;");

    scene_ = new Scene();

    ui_->gridLayoutMenu->setAlignment(Qt::AlignLeft);
    ui_->gridLayoutMenu->setAlignment(Qt::AlignTop);

    ui_->statusGridLayout->setAlignment(Qt::AlignLeft);
    ui_->statusGridLayout->setAlignment(Qt::AlignTop);

    ui_->graphicsView->setStyleSheet("background-color: #1f1f1f; border-color: #2b2b2b; border-radius: 10px;");
    ui_->graphicsView->setScene(scene_);
    ui_->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui_->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui_->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui_->penColorPushButton->setStyleSheet("background-color: #FFA500;");

    ui_->graphicsView->setFocusPolicy(Qt::StrongFocus);
    ui_->graphicsView->setFocus();

    scene_->setPenColor(QColor(QStringLiteral("#FFA500")));
    ui_->penWidthPushButton->setText("1.0");

    timer_ = new QTimer();
    connect(timer_, &QTimer::timeout, this, &MainWindow::timerSlot);
    timer_->start(constants::kTimerIntervalMs);

    statusTimer_ = new QTimer();
    connect(statusTimer_, &QTimer::timeout, this, &MainWindow::updateStatus);
    statusTimer_->start(constants::kTimerIntervalMs);

    QToolBar *fileToolBar = addToolBar("File");
    fileToolBar->addAction("Save", this, &MainWindow::on_actionSave_triggered);
    fileToolBar->addAction("Open", this, &MainWindow::on_actionOpen_triggered);

    QMenu *pluginMenu = menuBar()->addMenu("Plugins");
    pluginMenu->addAction("Load Plugin...", this, &MainWindow::on_actionLoadPlugin_triggered);

    QToolBar *pluginToolBar = addToolBar("Plugins");
    pluginToolBar->addAction("Load Plugin", this, &MainWindow::on_actionLoadPlugin_triggered);

    PluginManager::instance()->registerBuiltInFigure("Triangle", "Triangle",
        [](const QPointF& p) -> Figure* { return new Triangle(p); },
        [](const QJsonObject& obj) -> Figure* { Triangle* t = new Triangle(QPointF()); t->fromJson(obj); return t; });
    PluginManager::instance()->registerBuiltInFigure("Circle", "Circle",
        [](const QPointF& p) -> Figure* { return new Circle(p); },
        [](const QJsonObject& obj) -> Figure* { Circle* c = new Circle(QPointF()); c->fromJson(obj); return c; });
    PluginManager::instance()->registerBuiltInFigure("Rhombus", "Rhombus",
        [](const QPointF& p) -> Figure* { return new Rhombus(p); },
        [](const QJsonObject& obj) -> Figure* { Rhombus* r = new Rhombus(QPointF()); r->fromJson(obj); return r; });
    PluginManager::instance()->registerBuiltInFigure("Square", "Square",
        [](const QPointF& p) -> Figure* { return new Square(p); },
        [](const QJsonObject& obj) -> Figure* { Square* s = new Square(QPointF()); s->fromJson(obj); return s; });
    PluginManager::instance()->registerBuiltInFigure("Rectangle", "Rectangle",
        [](const QPointF& p) -> Figure* { return new Rectangle(p); },
        [](const QJsonObject& obj) -> Figure* { Rectangle* r = new Rectangle(QPointF()); r->fromJson(obj); return r; });
    PluginManager::instance()->registerBuiltInFigure("Star", "Star",
        [](const QPointF& p) -> Figure* { return new Star(p); },
        [](const QJsonObject& obj) -> Figure* { Star* s = new Star(QPointF()); s->fromJson(obj); return s; });
    PluginManager::instance()->registerBuiltInFigure("Polygon", "Polygon",
        [](const QPointF& p) -> Figure* { return new Polygon(p); },
        [](const QJsonObject& obj) -> Figure* { Polygon* pol = new Polygon(QPointF()); pol->fromJson(obj); return pol; });
    PluginManager::instance()->registerBuiltInFigure("Parallelogram", "Parallelogram",
        [](const QPointF& p) -> Figure* { return new Parallelogram(p); },
        [](const QJsonObject& obj) -> Figure* { Parallelogram* par = new Parallelogram(QPointF()); par->fromJson(obj); return par; });
    PluginManager::instance()->registerBuiltInFigure("Line", "Line",
        [](const QPointF& p) -> Figure* { return new Line(p); },
        [](const QJsonObject& obj) -> Figure* { Line* l = new Line(QPointF()); l->fromJson(obj); return l; });
    PluginManager::instance()->registerBuiltInFigure("Ellipse", "Ellipse",
        [](const QPointF& p) -> Figure* { return new Ellipse(p); },
        [](const QJsonObject& obj) -> Figure* { Ellipse* e = new Ellipse(QPointF()); e->fromJson(obj); return e; });
    PluginManager::instance()->registerBuiltInFigure("Polyline", "Polyline",
        [](const QPointF& p) -> Figure* { return new Polyline(p); },
        [](const QJsonObject& obj) -> Figure* { Polyline* pl = new Polyline(QPointF()); pl->fromJson(obj); return pl; });

    PluginManager::instance()->loadPlugins();
    connect(PluginManager::instance(), &PluginManager::pluginsChanged, this, &MainWindow::updateFigureButtons);

    updateFigureButtons();

    connect(ui_->penColorPushButton, &QPushButton::clicked, this, &MainWindow::handlePenColorButton);
    connect(ui_->brushColorPushButton, &QPushButton::clicked, this, &MainWindow::handleBrushColorButton);
    connect(ui_->penWidthPushButton, &QPushButton::clicked, this, &MainWindow::handlePenWidthButton);
    // connect(ui_->erasePushButton, &QPushButton::clicked, this, &MainWindow::handleEraseButton);

    scaleUndoTimer_ = new QTimer(this);
    scaleUndoTimer_->setSingleShot(true);
    connect(scaleUndoTimer_, &QTimer::timeout, this, &MainWindow::resetScaleUndoFlag);

    QToolBar *layerToolBar = addToolBar("Layers");
    layerToolBar->addAction("Add Layer", this, &MainWindow::handleAddLayer);
    layerToolBar->addAction("Remove Layer", this, &MainWindow::handleRemoveLayer);
    layerToolBar->addAction("Layer Up", this, &MainWindow::handleLayerUp);
    layerToolBar->addAction("Layer Down", this, &MainWindow::handleLayerDown);

    layerCombo_ = new QComboBox();
    layerToolBar->addWidget(layerCombo_);
    connect(layerCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::setCurrentLayer);

    moveToCombo_ = new QComboBox();
    layerToolBar->addWidget(new QLabel(" Move to: "));
    layerToolBar->addWidget(moveToCombo_);
    connect(moveToCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::moveSelectedToLayer);

    updateLayerList();
    connect(scene_, &Scene::layersChanged, this, &MainWindow::updateLayerList);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!scene_->selectedFigures().isEmpty())
    {
        qreal factor = 1.0 + event->angleDelta().y() / 1200.0;
        if (factor > 0.1 && factor < 10.0) {
            if (!scaleUndoPending_) {
                scene_->pushUndoState();
                scaleUndoPending_ = true;
            }
            scene_->scaleSelectedFigures(factor);
            scaleUndoTimer_->start(200);
        }
    }
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    timer_->start(constants::kTimerIntervalMs);
    QMainWindow::resizeEvent(event);
}

void MainWindow::timerSlot()
{
    timer_->stop();
    scene_->setSceneRect(0, 0, ui_->graphicsView->width(), ui_->graphicsView->height());
}

void MainWindow::handlePenColorButton()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    ui_->penColorPushButton->setStyleSheet("background-color: " + color.name() + ";");
    scene_->setPenColor(color);
    scene_->un_select();
}

void MainWindow::handleBrushColorButton()
{
    QColorDialog color_dialog;
    color_dialog.setCustomColor(0, QColor(QStringLiteral("#1f1f1f")));
    QColor color = color_dialog.getColor(Qt::white, this);
    ui_->brushColorPushButton->setStyleSheet("background-color: " + color.name() + ";");
    scene_->setBrushColor(color);
    scene_->un_select();
}

void MainWindow::handlePenWidthButton()
{
    bool ok;
    qreal pen_width = QInputDialog::getDouble(this, "", "Pen width", scene_->getPenWidth(),
                                              constants::kMinPenWidth, constants::kMaxPenWidth,
                                              constants::kPenWidthDecimals, &ok);
    if (ok)
    {
        ui_->penWidthPushButton->setText(QString::number(pen_width));
        scene_->setPenWidth(pen_width);
    }
    scene_->un_select();
}

void MainWindow::handleEraseButton()
{
    scene_->removeAllItems();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z) {
        scene_->undo();
        event->accept();
        return;
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Y) {
        scene_->redo();
        event->accept();
        return;
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        scene_->copySelectedFigure();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Delete) {
        scene_->deleteSelectedFigure();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        scene_->un_select();
        scene_->cancelPolylineBuilding();
        scene_->update();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_C && event->modifiers() == Qt::NoModifier) {
        scene_->removeAllItems();
        event->accept();
        return;
    }

    if (!scene_->selectedFigures().isEmpty()) {
        QPointF delta(0, 0);
        if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A)
            delta = QPointF(-1, 0);
        else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D)
            delta = QPointF(1, 0);
        else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W)
            delta = QPointF(0, -1);
        else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S)
            delta = QPointF(0, 1);

        if (event->key() == Qt::Key_Q) {
            scene_->pushUndoState();
            scene_->rotateSelected(-5.0);
            event->accept();
            return;
        } else if (event->key() == Qt::Key_E) {
            scene_->pushUndoState();
            scene_->rotateSelected(5.0);
            event->accept();
            return;
        }

        if (!delta.isNull()) {
            scene_->pushUndoState();
            for (Figure *fig : scene_->selectedFigures()) {
                fig->setPos(fig->pos() + delta);
                fig->update();
            }
            scene_->update();
            event->accept();
            return;
        }
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::updateStatus()
{
    statusTimer_->start(constants::kTimerIntervalMs);
    bool perimeter_ok;
    qreal perimeter = scene_->getSelectedFigurePerimeter(perimeter_ok);
    if (perimeter_ok)
        ui_->perimterLabel->setText(QString::number(perimeter));
    else
        ui_->perimterLabel->setText("Perimeter");

    bool square_ok;
    qreal square = scene_->getSelectedFigureSquare(square_ok);
    if (square_ok)
        ui_->squareLabel->setText(QString::number(square));
    else
        ui_->squareLabel->setText("Square");

    if ((scene_->getSelectedFigure() != nullptr) && scene_->getSelectedFigure()->is_selected()) {
        QPointF center = scene_->getSelectedFigure()->getCenterOfMass();
        center = scene_->getSelectedFigure()->mapToScene(center);
        ui_->centerXLabel->setText(QString::number(center.x()));
        ui_->centerYLabel->setText(QString::number(center.y()));
    } else {
        ui_->centerXLabel->setText("X");
        ui_->centerYLabel->setText("Y");
    }
}

void MainWindow::handleAddLayer()
{
    scene_->addLayer("Layer " + QString::number(scene_->layers().size()+1));
}

void MainWindow::handleRemoveLayer()
{
    int current = layerCombo_->currentIndex();
    if (current >= 0)
        scene_->removeLayer(current);
}

void MainWindow::handleLayerUp()
{
    int current = layerCombo_->currentIndex();
    if (current > 0)
        scene_->moveLayerUp(current);
}

void MainWindow::handleLayerDown()
{
    int current = layerCombo_->currentIndex();
    if (current < scene_->layers().size() - 1)
        scene_->moveLayerDown(current);
}

void MainWindow::updateLayerList()
{
    layerCombo_->clear();
    moveToCombo_->clear();
    for (Layer *layer : scene_->layers()) {
        QString name = layer->name();
        layerCombo_->addItem(name);
        moveToCombo_->addItem(name);
    }
    int current = scene_->layers().indexOf(scene_->currentLayer());
    if (current >= 0)
        layerCombo_->setCurrentIndex(current);
}

void MainWindow::setCurrentLayer(int index)
{
    if (index >= 0)
        scene_->setCurrentLayer(index);
}

void MainWindow::moveSelectedToLayer(int index)
{
    if (index < 0) return;
    scene_->moveSelectedFiguresToLayer(index);
    scene_->update();
}

void MainWindow::resetScaleUndoFlag()
{
    scaleUndoPending_ = false;
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Drawing", "", "JSON (*.json)");
    if (!fileName.isEmpty())
        scene_->saveToFile(fileName);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Drawing", "", "JSON (*.json)");
    if (!fileName.isEmpty())
        scene_->loadFromFile(fileName);
}

void MainWindow::updateFigureButtons()
{
    // Удаляем все старые кнопки фигур (оставляем служебные кнопки: penColor, penWidth, brushColor, erase)
    QList<QPushButton*> toRemove;
    for (int i = 0; i < ui_->gridLayoutMenu->count(); ++i) {
        QLayoutItem *item = ui_->gridLayoutMenu->itemAt(i);
        if (item && item->widget()) {
            QPushButton *btn = qobject_cast<QPushButton*>(item->widget());
            if (btn && btn != ui_->penColorPushButton && btn != ui_->penWidthPushButton &&
                btn != ui_->brushColorPushButton) {
                toRemove.append(btn);
            }
        }
    }
    for (QPushButton *btn : toRemove) {
        ui_->gridLayoutMenu->removeWidget(btn);
        delete btn;
    }

    QStringList types = PluginManager::instance()->availableFigureTypes();
    int row = 1;
    int col = 0;
    const int maxCols = 12;

    for (const QString& typeId : types) {
        QString symbol = PluginManager::instance()->iconSymbol(typeId);
        if (symbol.isEmpty())
            symbol = "?";
        QString displayName = PluginManager::instance()->displayName(typeId);

        QPushButton *btn = new QPushButton(symbol);
        btn->setMaximumSize(20, 20);
        btn->setToolTip(displayName);

        connect(btn, &QPushButton::clicked, this, [this, typeId]() {
            if (typeId == "Star") {
                bool ok;
                int points_count = QInputDialog::getInt(this, "", "Points count", Star::getPointsCount(),
                                                        constants::kMinPointsCount, constants::kMaxStarPointsCount,
                                                        constants::kPointsCountStep, &ok);
                if (ok) {
                    Star::setPointsCount(points_count);
                    scene_->setFigureType(typeId);
                }
            } else if (typeId == "Polygon") {
                bool ok;
                int points_count = QInputDialog::getInt(this, "", "Points count", Polygon::getPointsCount(),
                                                        constants::kMinPointsCount, constants::kMaxPolygonPointsCount,
                                                        constants::kPointsCountStep, &ok);
                if (ok) {
                    Polygon::setPointsCount(points_count);
                    scene_->setFigureType(typeId);
                }
            } else {
                scene_->setFigureType(typeId);
            }
            scene_->un_select();
        });

        ui_->gridLayoutMenu->addWidget(btn, row, col);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    // Добавляем растягивающий элемент
    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui_->gridLayoutMenu->addItem(spacer, row, col, 1, maxCols - col);
}

void MainWindow::on_actionLoadPlugin_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Figure Plugin",
        QCoreApplication::applicationDirPath(),
        "Plugins (*.so *.dll *.dylib);;All Files (*)");

    if (fileName.isEmpty())
        return;

    QPluginLoader *loader = new QPluginLoader(fileName);
    QObject *plugin = loader->instance();
    if (!plugin) {
        QMessageBox::critical(this, "Plugin Load Error",
            "Failed to load plugin:\n" + loader->errorString());
        delete loader;
        return;
    }

    FigurePluginInterface *iface = qobject_cast<FigurePluginInterface*>(plugin);
    if (!iface) {
        QMessageBox::critical(this, "Invalid Plugin",
            "The selected file is not a valid Paint figure plugin.");
        loader->unload();
        delete loader;
        return;
    }

    QString typeId = iface->figureTypeId();
    QString displayName = iface->displayName();

    if (PluginManager::instance()->availableFigureTypes().contains(typeId)) {
        QMessageBox::information(this, "Plugin Already Loaded",
            QString("Plugin '%1' is already loaded.").arg(displayName));
        loader->unload();
        delete loader;
        return;
    }

    PluginManager::instance()->registerExternalPlugin(typeId, displayName, plugin, iface, loader);
    QMessageBox::information(this, "Plugin Loaded",
        QString("Plugin '%1' loaded successfully.").arg(displayName));
}
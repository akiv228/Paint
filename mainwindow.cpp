#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "star.h"
#include "polygon.h"
#include "QDebug"
#include "constants.h"

#include <QInputDialog>
#include <QTimer>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new ui::MainWindow) // создание объекта интерфейса
{
    ui_->setupUi(this);  // настройка из .ui-файла

    this->setWindowTitle("Paint");
    this->setStyleSheet("background-color: #181818;"
                        "color: white;");

    scene_ = new Scene();

    ui_->gridLayoutMenu->setAlignment(Qt::AlignLeft);
    ui_->gridLayoutMenu->setAlignment(Qt::AlignTop);

    ui_->statusGridLayout->setAlignment(Qt::AlignLeft);
    ui_->statusGridLayout->setAlignment(Qt::AlignTop);

    ui_->graphicsView->setStyleSheet("background-color: #1f1f1f;"
                                     "border-color: #2b2b2b;"
                                     "border-radius: 10px;");
    ui_->graphicsView->setScene(scene_);
    ui_->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui_->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui_->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui_->penColorPushButton->setStyleSheet("background-color: #FFA500;"); // цвет кнопки

    scene_->setPenColor(QColor(QStringLiteral("#FFA500")));
    ui_->penWidthPushButton->setText("1.0");

    // срабатывает только при изменении размеров окна
    timer_ = new QTimer();
    connect(timer_, &QTimer::timeout, this, &MainWindow::timerSlot);
    timer_->start(constants::kTimerIntervalMs);

    // работает постоянно, но обновляет только легковесные данные
    statusTimer_ = new QTimer();
    connect(statusTimer_, &QTimer::timeout, this, &MainWindow::updateStatus);
    statusTimer_->start(constants::kTimerIntervalMs);

    connect(ui_->trianglePushButton, &QPushButton::clicked, this, &MainWindow::handleTriangleButton);
    connect(ui_->circlePushButton, &QPushButton::clicked, this, &MainWindow::handleCircleButton);
    connect(ui_->rhombusPushButton, &QPushButton::clicked, this, &MainWindow::handleRhombusButton);
    connect(ui_->squarePushButton, &QPushButton::clicked, this, &MainWindow::handleSquareButton);
    connect(ui_->rectanglePushButton, &QPushButton::clicked, this, &MainWindow::handleRectangleButton);
    connect(ui_->starPushButton, &QPushButton::clicked, this, &MainWindow::handleStarButton);
    connect(ui_->polygonPushButton, &QPushButton::clicked, this, &MainWindow::handlePolygonButton);
    connect(ui_->trapezoidPushButton, &QPushButton::clicked, this, &MainWindow::handleTrapezoidButton);
    connect(ui_->penColorPushButton, &QPushButton::clicked, this, &MainWindow::handlePenColorButton);
    connect(ui_->brushColorPushButton, &QPushButton::clicked, this, &MainWindow::handleBrushColorButton);
    connect(ui_->penWidthPushButton, &QPushButton::clicked, this, &MainWindow::handlePenWidthButton);
    connect(ui_->erasePushButton, &QPushButton::clicked, this, &MainWindow::handleEraseButton);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

// изменять размер сцены
void MainWindow::resizeEvent(QResizeEvent *event)
{
    timer_->start(constants::kTimerIntervalMs);
    QMainWindow::resizeEvent(event); // базовое определение функции
}

void MainWindow::timerSlot() // обновим сцену после прогрузки окна (один раз)
{
    timer_->stop();
    scene_->setSceneRect(0, 0, ui_->graphicsView->width(), ui_->graphicsView->height());
}

void MainWindow::handleTriangleButton()
{
    scene_->setFigureType(FigureType::kTriangle);
    scene_->un_select();
}

void MainWindow::handleCircleButton()
{
    scene_->setFigureType(FigureType::kCircle);
    scene_->un_select();
}

void MainWindow::handleRhombusButton()
{
    scene_->setFigureType(FigureType::kRhombus);
    scene_->un_select();
}

void MainWindow::handleSquareButton()
{
    scene_->setFigureType(FigureType::kSquare);
    scene_->un_select();
}

void MainWindow::handleRectangleButton()
{
    scene_->setFigureType(FigureType::kRectangle);
    scene_->un_select();
}

void MainWindow::handleStarButton()
{
    bool ok;
    int points_count = QInputDialog::getInt(this, "", "Points count", Star::getPointsCount(),
                                            constants::kMinPointsCount, constants::kMaxStarPointsCount,
                                            constants::kPointsCountStep, &ok);
    // Star::getPointsCount() значение по умолчанию
    if (ok)
    {
        Star::setPointsCount(points_count);
        scene_->setFigureType(FigureType::kStar);
    }
    scene_->un_select();
}

void MainWindow::handlePolygonButton()
{
    bool ok;
    int points_count = QInputDialog::getInt(this, "", "Points count", Polygon::getPointsCount(),
                                            constants::kMinPointsCount, constants::kMaxPolygonPointsCount,
                                            constants::kPointsCountStep, &ok);
    if (ok)
    {
        Polygon::setPointsCount(points_count);
        scene_->setFigureType(FigureType::kPolygon);
    }
    scene_->un_select();
}

void MainWindow::handleTrapezoidButton()
{
    scene_->setFigureType(FigureType::kTrapezoid);
    scene_->un_select();
}

void MainWindow::handlePenColorButton()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    ui_->penColorPushButton->setStyleSheet("background-color: " + color.name() + ";"); // color.name() в 16-ричной форме
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
        ui_->penWidthPushButton->setText(QString::number(pen_width)); // цифра на кнопке
        scene_->setPenWidth(pen_width); // толщина пера
    }
    scene_->un_select();
}

void MainWindow::handleEraseButton()
{
    scene_->removeAllItems();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        scene_->deleteSelectedFigure();
    }
    if (event->key() == Qt::Key_Escape) {
        scene_->un_select();
        scene_->update();
    }
    if (event->key() == Qt::Key_C) {
        scene_->removeAllItems();
    }
}

void MainWindow::updateStatus()
{
    statusTimer_->start(constants::kTimerIntervalMs);
    bool perimeter_ok;
    qreal perimeter = scene_->getSelectedFigurePerimeter(perimeter_ok);
    if (perimeter_ok) {
        ui_->perimterLabel->setText(QString::number(perimeter));
    } else {
        ui_->perimterLabel->setText("Perimeter");
    }
    bool square_ok;
    qreal square = scene_->getSelectedFigureSquare(square_ok);
    if (square_ok) {
        ui_->squareLabel->setText(QString::number(square));
    } else {
        ui_->squareLabel->setText("Square");
    }
    // центр масс
    if ((scene_->getSelectedFigure() != nullptr) && scene_->getSelectedFigure()->is_selected()) {
        QPointF center = scene_->getSelectedFigure()->getCenterOfMass();
        center = scene_->getSelectedFigure()->mapToScene(center); // из локальных в координаты сцены
        ui_->centerXLabel->setText(QString::number(center.x()));
        ui_->centerYLabel->setText(QString::number(center.y()));
    } else {
        ui_->centerXLabel->setText("X");
        ui_->centerYLabel->setText("Y");
    }
}

// #include "mainwindow.h"
// #include "./ui_mainwindow.h"
// #include "star.h"
// #include "polygon.h"
// #include "QDebug"

// #include <QInputDialog>
// #include <QTimer>
// #include <QKeyEvent>

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent)
//     , ui_(new ui::MainWindow) // создание объекта интерфейса
// {
//     ui_->setupUi(this);  // настройк из .ui-файла

//     this->setWindowTitle("Paint");
//     this->setStyleSheet("background-color: #181818;"
//                         "color: white;");

//     scene_ = new Scene();

//     ui_->gridLayoutMenu->setAlignment(Qt::AlignLeft);
//     ui_->gridLayoutMenu->setAlignment(Qt::AlignTop);

//     ui_->statusGridLayout->setAlignment(Qt::AlignLeft);
//     ui_->statusGridLayout->setAlignment(Qt::AlignTop);

//     ui_->graphicsView->setStyleSheet("background-color: #1f1f1f;"
//                                     "border-color: #2b2b2b;"
//                                     "border-radius: 10px;");
//     ui_->graphicsView->setScene(scene_);
//     ui_->graphicsView->setRenderHint(QPainter::Antialiasing);
//     ui_->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     ui_->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     ui_->penColorPushButton->setStyleSheet("background-color: #FFA500;"); //цвет кнопки

//     scene_->setPenColor(QColor(QStringLiteral("#FFA500")));
//     ui_->penWidthPushButton->setText("1.0");
//     //срабатывает только при изменении размеров окна
//     timer_ = new QTimer();
//     connect(timer_, &QTimer::timeout, this, &MainWindow::timerSlot);
//     timer_->start(50);
//     //работает постоянно, но обновляет только легковесные данные
//     statusTimer_ = new QTimer();
//     connect(statusTimer_, &QTimer::timeout, this, &MainWindow::updateStatus);
//     statusTimer_->start(50);

//     connect(ui_->trianglePushButton, &QPushButton::clicked, this, &MainWindow::handleTriangleButton);
//     connect(ui_->circlePushButton, &QPushButton::clicked, this, &MainWindow::handleCircleButton);
//     connect(ui_->rhombusPushButton, &QPushButton::clicked, this, &MainWindow::handleRhombusButton);
//     connect(ui_->squarePushButton, &QPushButton::clicked, this, &MainWindow::handleSquareButton);
//     connect(ui_->rectanglePushButton, &QPushButton::clicked, this, &MainWindow::handleRectangleButton);
//     connect(ui_->starPushButton, &QPushButton::clicked, this, &MainWindow::handleStarButton);
//     connect(ui_->polygonPushButton, &QPushButton::clicked, this, &MainWindow::handlePolygonButton);
//     connect(ui_->trapezoidPushButton, &QPushButton::clicked, this, &MainWindow::handleTrapezoidButton);
//     connect(ui_->penColorPushButton, &QPushButton::clicked, this, &MainWindow::handlePenColorButton);
//     connect(ui_->brushColorPushButton, &QPushButton::clicked, this, &MainWindow::handleBrushColorButton);
//     connect(ui_->penWidthPushButton, &QPushButton::clicked, this, &MainWindow::handlePenWidthButton);
//     connect(ui_->erasePushButton, &QPushButton::clicked, this, &MainWindow::handleEraseButton);
// }

// MainWindow::~MainWindow()
// {
//     delete ui_;
// }

// // изменять размер сцены
// void MainWindow::resizeEvent(QResizeEvent *event)
// {
//     timer_->start(50);
//     QMainWindow::resizeEvent(event); // базовое определение ф-и
// }

// void MainWindow::timerSlot() //обновим сцену после прогрузки окна(один раз)
// {
//     timer_->stop();
//     scene_->setSceneRect(0, 0, ui_->graphicsView->width(), ui_->graphicsView->height());
// }

// void MainWindow::handleTriangleButton()
// {
//     scene_->setFigureType(FigureType::kTriangle);
//     scene_->un_select();
// }

// void MainWindow::handleCircleButton()
// {
//     scene_->setFigureType(FigureType::kCircle);
//     scene_->un_select();
// }

// void MainWindow::handleRhombusButton()
// {
//     scene_->setFigureType(FigureType::kRhombus);
//     scene_->un_select();
// }

// void MainWindow::handleSquareButton()
// {
//     scene_->setFigureType(FigureType::kSquare);
//     scene_->un_select();
// }

// void MainWindow::handleRectangleButton()
// {
//     scene_->setFigureType(FigureType::kRectangle);
//     scene_->un_select();
// }

// void MainWindow::handleStarButton()
// {
//     bool ok;
//     int points_count
//         = QInputDialog::getInt(this, "", "Points count", Star::getPointsCount(), 5, 1000, 1, &ok);
//     // Star::getPointsCount() значение по умолчанию
//     if (ok)
//     {
//         Star::setPointsCount(points_count);
//         scene_->setFigureType(FigureType::kStar);
//     }
//     scene_->un_select();
// }

// void MainWindow::handlePolygonButton()
// {
//     bool ok;
//     int points_count = QInputDialog::getInt(this,
//                                             "",
//                                             "Points count",
//                                             Polygon::getPointsCount(),
//                                             5,
//                                             10000,
//                                             1,
//                                             &ok);
//     if (ok)
//     {
//         Polygon::setPointsCount(points_count);
//         scene_->setFigureType(FigureType::kPolygon);
//     }
//     scene_->un_select();
// }

// void MainWindow::handleTrapezoidButton()
// {
//     scene_->setFigureType(FigureType::kTrapezoid);
//     scene_->un_select();
// }

// void MainWindow::handlePenColorButton()
// {
//     QColor color = QColorDialog::getColor(Qt::white, this);
//     ui_->penColorPushButton->setStyleSheet("background-color: " + color.name() + ";"); // color.name() в 16ричной форме
//     scene_->setPenColor(color);
//     scene_->un_select();
// }

// void MainWindow::handleBrushColorButton()
// {
//     QColorDialog color_dialog;
//     color_dialog.setCustomColor(0, QColor(QStringLiteral("#1f1f1f")));
//     QColor color = color_dialog.getColor(Qt::white, this);
//     ui_->brushColorPushButton->setStyleSheet("background-color: " + color.name() + ";");
//     scene_->setBrushColor(color);
//     scene_->un_select();
// }

// void MainWindow::handlePenWidthButton()
// {
//     bool ok;
//     qreal pen_width
//         = QInputDialog::getDouble(this, "", "Pen width", scene_->getPenWidth(), 0.1, 999, 1, &ok);
//     if (ok)
//     {
//         ui_->penWidthPushButton->setText(QString::number(pen_width)); // цифра накнопке
//         scene_->setPenWidth(pen_width); //толщина ручки
//     }
//     scene_->un_select();
// }

// void MainWindow::handleEraseButton()
// {
//     scene_->removeAllItems();
// }

// void MainWindow::keyPressEvent(QKeyEvent* event) {
//     if (event->key() == Qt::Key_Delete) {
//         scene_->deleteSelectedFigure();
//     }
//     if (event->key() == Qt::Key_Escape) {
//         scene_->un_select();
//         scene_->update();
//     }
//     if (event->key() == Qt::Key_C) {
//         scene_->removeAllItems();
//     }
// }
// void MainWindow::updateStatus()
// {
//     statusTimer_->start(50);
//     bool perimeter_ok;
//     qreal perimeter = scene_->getSelectedFigurePerimeter(perimeter_ok);
//     if (perimeter_ok) {
//         ui_->perimterLabel->setText(QString::number(perimeter));
//     } else {
//         ui_->perimterLabel->setText("Perimeter");
//     }
//     bool square_ok;
//     qreal square = scene_->getSelectedFigureSquare(square_ok);
//     if (square_ok) {
//         ui_->squareLabel->setText(QString::number(square));
//     } else {
//         ui_->squareLabel->setText("Square");
//     }
//     // центр масс
//     if ((scene_->getSelectedFigure() != nullptr) && scene_->getSelectedFigure()->is_selected()) {
//         QPointF center = scene_->getSelectedFigure()->getCenterOfMass();
//         center = scene_->getSelectedFigure()->mapToScene(center); // из локальных в координаты сцены
//         ui_->centerXLabel->setText(QString::number(center.x()));
//         ui_->centerYLabel->setText(QString::number(center.y()));
//     } else {
//         ui_->centerXLabel->setText("X");
//         ui_->centerYLabel->setText("Y");
//     }
// }


// void MainWindow::updateStatus()
// {
//     statusTimer_->start(50);
//     bool perimeter_ok;
//     qreal perimeter = scene_->getSelectedFigurePerimeter(perimeter_ok);
//     if (perimeter_ok) {
//         ui_->perimterLabel->setText(QString::number(perimeter));
//     } else {
//         ui_->perimterLabel->setText("Perimeter");
//     }
//     bool square_ok;
//     qreal square = scene_->getSelectedFigureSquare(square_ok);
//     if (square_ok) {
//         ui_->squareLabel->setText(QString::number(square));
//     } else {
//         ui_->squareLabel->setText("Square");
//     }
// }




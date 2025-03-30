#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QMainWindow>
#include "scene.h"

QT_BEGIN_NAMESPACE
namespace ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(); //деструктор

private:
    ui::MainWindow *ui_;
    Scene *scene_;
    QTimer *timer_;
    QTimer *statusTimer_;

    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void timerSlot();
    void handleTriangleButton();
    void handleCircleButton();
    void handleRhombusButton();
    void handleSquareButton();
    void handleRectangleButton();
    void handleStarButton();
    void handlePolygonButton();
    void handleTrapezoidButton();
    void handlePenColorButton();
    void handleBrushColorButton();
    void handlePenWidthButton();
    void handleEraseButton();
    void updateStatus();
};

#endif // MAINWINDOW_H

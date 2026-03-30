#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QMainWindow>
#include <QComboBox>
#include "scene.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    Ui::MainWindow *ui_;
    Scene *scene_;
    QTimer *timer_;
    QTimer *statusTimer_;

    QComboBox *layerCombo_;
    QComboBox *moveToCombo_;

    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QTimer* scaleUndoTimer_;
    bool scaleUndoPending_ = false;

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
    void handleLineButton();
    void handlePolylineButton();
    void handleEllipseButton();
    void undo();
    void redo();
    void resetScaleUndoFlag(); 
    void handleAddLayer();
    void handleRemoveLayer();
    void handleLayerUp();
    void handleLayerDown();
    void updateLayerList();
    void setCurrentLayer(int index);
    void moveSelectedToLayer(int index);
    
};

#endif 
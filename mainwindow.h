#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
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
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_actionLoadPlugin_triggered();
    void timerSlot();
    void updateStatus();
    void handlePenColorButton();
    void handleBrushColorButton();
    void handlePenWidthButton();
    void handleEraseButton();
    void handleAddLayer();
    void handleRemoveLayer();
    void handleLayerUp();
    void handleLayerDown();
    void updateLayerList();
    void setCurrentLayer(int index);
    void moveSelectedToLayer(int index);
    void resetScaleUndoFlag();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
    void updateFigureButtons();

private:
    Ui::MainWindow *ui_;
    Scene *scene_;
    QTimer *timer_;
    QTimer *statusTimer_;
    QTimer *scaleUndoTimer_;
    bool scaleUndoPending_ = false;
    QComboBox *layerCombo_;
    QComboBox *moveToCombo_;
    QVBoxLayout *figureButtonsLayout_;
};

#endif // MAINWINDOW_H
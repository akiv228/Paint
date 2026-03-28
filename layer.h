#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include <QList>
#include <QString>

class Figure;

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(const QString &name = "Layer", QObject *parent = nullptr);
    ~Layer();

    QString name() const;
    void setName(const QString &name);
    bool isVisible() const;
    void setVisible(bool visible);
    QList<Figure*> figures() const;
    void addFigure(Figure *figure);
    void removeFigure(Figure *figure);
    void clear();

    void setZBase(int zBase);
    int zBase() const;

private:
    QString name_;
    bool visible_;
    QList<Figure*> figures_;
    int zBase_;
};

#endif // LAYER_H
#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QRectF>
#include <QPainter>
#include <QTextStream>
#include <QMouseEvent>

#include "algorithm.h"
#include <iostream>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void setAlgo(Algorithm* a);
signals:
    
public slots:
private:
    QPointF PointInRect(QRectF r, QPointF p);
    QPointF RPointInRect(QRectF r, QPointF p);

    QRectF _rect;
    Algorithm* _algo;
};

#endif // DRAWWIDGET_H

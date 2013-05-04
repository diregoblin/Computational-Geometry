#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPointF>

#include "algorithm.h"
#include "drawwidget.h"
#include <iostream>
#include <vector>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    
signals:
    
public slots:
    void BuildAlgo();
    void OpenFile();
    void SaveToFile();
    void PrevClicked();
    void NextClicked();
    void FirstClicked();
    void LastClicked();
    void AddPointsClicked();
private:
    void ShowExtraButtons(bool f);
    Algorithm _algo;
    QGridLayout _mainL, _buttonL, _pnL;
    QPushButton _open, _exec, _exit;
    QPushButton _addPoints, _save;
    QPushButton _next, _prev;
    QPushButton _toFirst, _toLast;
    DrawWidget _drawer;
    
};

#endif // MAINWINDOW_H

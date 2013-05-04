#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QString>

#include <vector>
#include <algorithm>
#include <iostream>

enum Operation {
    STEPARATOR,
    SET_I,
    ADD,
    REMOVE
};

struct Edge {
    Edge(QPointF a, QPointF b);
    qreal Length() const;
    void rot();
    qreal intersect(Edge g);
    QPointF a;
    QPointF b;
};

struct Action{
    Action(Operation a, std::vector<Edge> *vec, Edge e);
    Action(Operation a);
    Operation _oper;
    qint32 _stepnum;
    std::vector<Edge> *_vec;
    Edge _e;
};

enum AlgoState {
    INIT,
    BUILD,
    STEP
};

class Algorithm : public QObject
{
    Q_OBJECT
public:
    Algorithm();

    int AddPoint(QPointF point, bool a = false);
    int AddPoint(qreal x, qreal y, bool a = false);
    int Init();
    int Build();
    void Reset();

    AlgoState GetState();
    void ResetToAdd();
    QPointF GetBotleft();
    QRectF GetRect();
    qint32 GetStepNum();
    qint32 GetStepCount();

    std::vector<QPointF>* GetVectorR();
    std::vector<Edge>* GetFrontier();
    std::vector<Edge>* GetGrid();
    std::vector<Edge>* GetSkeleton();
signals:
//    void error(QString message);
public slots:
    void SkipForward();
    void SkipBack();
    int StepForward();
    int StepBack();

private:
    double Turn(QPointF a, QPointF b, QPointF c);
    bool Mate(Edge e, QPointF &p);
    void updateVector(std::vector<qint32> &data, qint32 oldval, qint32 newval);
    std::vector<QPointF> _data;
    std::vector<Edge> _frontier;
    std::vector<Edge> _grid;
    std::vector<Edge> _skeleton;
    QPointF _botleft;
    QRectF _rect;
    qint32 n;

    AlgoState _state; //next action
    std::vector<Action> _flow;
    qint32 _flowit;
};

#endif // ALGORITHM_H

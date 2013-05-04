#include "algorithm.h"

bool PointCompare(QPointF a, QPointF b)
{
    if (a.x() * b.y() - a.y() * b.x() == 0)
        return a.manhattanLength() < b.manhattanLength();
    return a.x() * b.y() - a.y() * b.x() > 0;
}

Algorithm::Algorithm()
    : _botleft(100000, 10000)
    , _state(INIT)
{
}

int Algorithm::AddPoint(QPointF point, bool a)
{
    if (_state != INIT)
        return (int)_state;
    if (a)
        for (int i = 0; i < _data.size(); ++i)
            if (point == _data[i])
                return -1;
    if (_data.size() == 0)
    {
        _rect = QRectF(point - QPointF(1, 1), point + QPointF(1, 1));
        _botleft = point;
    }
    _rect |= QRectF(point - QPointF(1, 1), point + QPointF(1, 1));
    _data.push_back(point);
    if (_botleft.y() > point.y() || (_botleft.y() == point.y() && _botleft.x() > point.x()))
        _botleft = point;
}

int Algorithm::AddPoint(qreal x, qreal y, bool a)
{
    return AddPoint(QPointF(x, y), a);
}

int Algorithm::Init()
{
    if (_state != INIT)
        return 1;
    if (_data.size() < 3)
        return 2;
    n=_data.size();
    for (int i = 0; i < n; ++i)
        _data[i] -= _botleft;
    std::sort(_data.begin(), _data.end(), PointCompare);
    int a = 0, b = 1;
    while (b < _data.size())
    {
        if (_data[a] != _data[b])
        {
            ++a;
            std::swap(_data[a], _data[b]);
        }
        ++b;
    }
    ++a;
    _data.resize(a);
    n=_data.size();
    _state = BUILD;
    return 0;
}

int Algorithm::Build()
{
    if (_state != BUILD)
        return 1;
    //DELONE
    _flow.push_back(Action(STEPARATOR));
    _frontier.push_back(Edge(_data[0], _data[1]));
    _flow.push_back(Action(ADD, &_frontier, Edge(_data[0], _data[1])));
    _grid.push_back(Edge(_data[0], _data[1]));
    _flow.push_back(Action(ADD, &_grid, Edge(_data[0], _data[1])));
    _flow.push_back(Action(STEPARATOR));
    while (!_frontier.empty())
    {
        QPointF p;
        Edge e = _frontier.back();
        _frontier.pop_back();
        _flow.push_back(Action(REMOVE, &_frontier, e));
        if(Mate(e, p))
        {
            Edge rev(p, e.a);
            std::vector<Edge>::iterator revloc =
                    std::find(_frontier.begin(), _frontier.end(), rev);
            if (revloc != _frontier.end())
            {
                _flow.push_back(Action(REMOVE, &_frontier, *revloc));
                *revloc = _frontier.back();
                _frontier.pop_back();
            }
            else
            {
                _frontier.push_back(Edge(e.a, p));
                _flow.push_back(Action(ADD, &_frontier, Edge(e.a, p)));
                _grid.push_back(Edge(e.a, p));
                _flow.push_back(Action(ADD, &_grid, Edge(e.a, p)));
            }
            rev = Edge(e.b, p);
            revloc = std::find(_frontier.begin(), _frontier.end(), rev);
            if (revloc != _frontier.end())
            {
                _flow.push_back(Action(REMOVE, &_frontier, *revloc));
                *revloc = _frontier.back();
                _frontier.pop_back();
            }
            else
            {
                _frontier.push_back(Edge(p, e.b));
                _flow.push_back(Action(ADD, &_frontier, Edge(p, e.b)));
                _grid.push_back(Edge(p, e.b));
                _flow.push_back(Action(ADD, &_grid, Edge(p, e.b)));
            }
        }
        _flow.push_back(Action(STEPARATOR));
    }
    //DELONEEND
    //KRUSKAL
    std::vector<Edge> unlinked = _grid;
    std::sort(unlinked.begin(), unlinked.end());
    std::reverse(unlinked.begin(), unlinked.end());
    std::vector<QPointF> linked;
    std::vector<qint32> indexes;
    while (!unlinked.empty())
    {
        Edge e = unlinked.back();
        unlinked.pop_back();
        qint32 loca, locb;
        for(loca = 0; loca < linked.size(); loca++)
        {
            if(linked[loca] == e.a)
                break;
        }
        for(locb = 0; locb < linked.size(); locb++)
        {
            if(linked[locb] == e.b)
                break;
        }
        if((loca == linked.size()) && (locb == linked.size()))
        {
            _skeleton.push_back(e);
            indexes.push_back(loca);
            indexes.push_back(loca);
            linked.push_back(e.a);
            linked.push_back(e.b);
            _flow.push_back(Action(ADD, &_skeleton, e));
            _flow.push_back(Action(STEPARATOR));
        }
        else
        {
            if((loca == linked.size()) || (locb == linked.size()))
            {
                _skeleton.push_back(e);
                if(loca == linked.size())
                {
                    linked.push_back(e.a);
                    indexes.push_back(indexes[locb]);
                }
                else
                {
                    linked.push_back(e.b);
                    indexes.push_back(indexes[loca]);
                }
                _flow.push_back(Action(ADD, &_skeleton, e));
                _flow.push_back(Action(STEPARATOR));
            }

            else
            {
                if(indexes[loca] != indexes[locb])
                {
                    _skeleton.push_back(e);
                    updateVector(indexes, indexes[locb], indexes[loca]);
                    _flow.push_back(Action(ADD, &_skeleton, e));
                    _flow.push_back(Action(STEPARATOR));
                }
            }
        }
    }
    //KRUSKALEND
    _flowit = _flow.size() - 1;
    _state = STEP;
    return 0;
}

void Algorithm::SkipForward()
{
    while (!StepForward());
}

void Algorithm::SkipBack()
{
    while (!StepBack());
}

void Algorithm::Reset()
{
    _state = INIT;
    _data.clear();
    _frontier.clear();
    _grid.clear();
    _skeleton.clear();
    _flow.push_back(Action(SET_I));
    _flow.clear();
}

int Algorithm::StepForward()
{
    if (_flow[_flowit]._oper != STEPARATOR)
        return 1;
    if (_flow.size() <= _flowit + 1)
        return 2;
    ++_flowit;
    while(_flow[_flowit]._oper != STEPARATOR)
    {
        switch(_flow[_flowit]._oper)
        {
        case ADD:
            _flow[_flowit]._vec->push_back(_flow[_flowit]._e);
            break;
        case REMOVE:
            std::vector<Edge>::iterator loc =
                    std::find(_flow[_flowit]._vec->begin(),
                              _flow[_flowit]._vec->end(),
                              _flow[_flowit]._e);
            if (loc != _flow[_flowit]._vec->end())
            {
                *loc = _flow[_flowit]._vec->back();
                _flow[_flowit]._vec->pop_back();
            }
            break;
        }
        ++_flowit;
    }
    return 0;
}

int Algorithm::StepBack()
{
    if (_flow[_flowit]._oper != STEPARATOR)
        return 1;
    if (_flowit <= 0)
        return 2;

    --_flowit;
    while(_flow[_flowit]._oper != STEPARATOR)
    {
        switch(_flow[_flowit]._oper)
        {
        case REMOVE:
            _flow[_flowit]._vec->push_back(_flow[_flowit]._e);
            break;
        case ADD:
            std::vector<Edge>::iterator loc =
                    std::find(_flow[_flowit]._vec->begin(),
                              _flow[_flowit]._vec->end(),
                              _flow[_flowit]._e);
            if (loc != _flow[_flowit]._vec->end())
            {
                *loc = _flow[_flowit]._vec->back();
                _flow[_flowit]._vec->pop_back();
            }
            break;
        }
        --_flowit;
    }
    return 0;
}

AlgoState Algorithm::GetState()
{
    return _state;
}

void Algorithm::ResetToAdd()
{
    _flow.push_back(Action(SET_I));
    _flow.clear();
    _frontier.clear();
    _grid.clear();
    _skeleton.clear();
    if (_state > INIT)
        for (int i = 0;i < _data.size(); ++i)
            _data[i] += _botleft;
    _state = INIT;
}

void Edge::rot()
{
    QPointF mid = (a + b) / 2;
    QPointF st(-(a.y() - mid.y()) + mid.x(),
               (a.x() - mid.x()) + mid.y());
    QPointF fin(-(b.y() - mid.y()) + mid.x(),
               (b.x() - mid.x()) + mid.y());
    a = st;
    b = fin;
}

qreal Edge::intersect(Edge g)
{
    QPointF mid = (a + b) / 2;
    qreal ua = ((g.b.x() - g.a.x())*(a.y() - g.a.y()) -
                (g.b.y() - g.a.y())*(a.x() - g.a.x()))
             / ((g.b.y() - g.a.y())*(b.x() - a.x()) -
                (g.b.x() - g.a.x())*(b.y() - a.y()));
    QPointF ip(a.x() + ua * (b.x() - a.x()),
               a.y() + ua * (b.y() - a.y()));
    if ((ip - a).manhattanLength() < (ip - b).manhattanLength())
        return -(ip - mid).manhattanLength();
    else
        return (ip - mid).manhattanLength();
}

bool Algorithm::Mate(Edge e, QPointF &p)
{
    QPointF *bestp = NULL;
    qreal  t, bestt = 100000;
    Edge f = e;
    f.rot();      // f - перпендикуляр в середине отрезка е
    for (qint32 i = 0; i < n; i++)
        if ((Turn(e.a, e.b, _data[i]) > 0) &&
                (_data[i] != e.a) && (_data[i] != e.b)){
            Edge g(e.b, _data[i]);
            g.rot();
            t = f.intersect(g);
            if (t < bestt) {
                bestp = &_data[i];
                bestt = t;
            }
        }
    if (bestp) {
      p = *bestp;
      return true;
    }
    return false;
}

void Algorithm::updateVector(std::vector<qint32> &data, qint32 oldval, qint32 newval)
{
    for(qint32 it = 0; it < data.size(); it++)
    {
        if(data[it] == oldval)
            data[it] = newval;
    }
}

QPointF Algorithm::GetBotleft()
{
    return _botleft;
}

QRectF Algorithm::GetRect()
{
    return _rect;
}

qint32 Algorithm::GetStepNum()
{
    return _flow[_flowit]._stepnum;
}

qint32 Algorithm::GetStepCount()
{
    return _flow.back()._stepnum;
}

std::vector<QPointF>* Algorithm::GetVectorR()
{
    return &_data;
}

std::vector<Edge>* Algorithm::GetFrontier()
{
    return &_frontier;
}

std::vector<Edge>* Algorithm::GetSkeleton()
{
    return &_skeleton;
}

std::vector<Edge>* Algorithm::GetGrid()
{
    return &_grid;
}

double Algorithm::Turn(QPointF a, QPointF b, QPointF c)
{
    return (b.x() - a.x()) * (c.y() - a.y()) -
            (b.y() - a.y()) * (c.x() - a.x()) ;
}

Action::Action(Operation a)
    : _oper(a)
    , _stepnum(0)
    , _vec(NULL)
    , _e(QPoint(0,0),QPoint(0,0))
{
    static qint32 i(0);
    if (_oper == STEPARATOR)
        _stepnum = i++;
    if (_oper == SET_I)
        i = 0;
}

Action::Action(Operation a, std::vector<Edge> *vec, Edge e)
    : _oper(a)
    , _vec(vec)
    , _e(e)
{
}

Edge::Edge(QPointF a, QPointF b)
    : a(a)
    , b(b)
{
}

qreal Edge::Length() const
{
    return (a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y());
}

inline bool operator==(const Edge &f, const Edge &g)
{
    if((f.a == g.a) && (f.b == g.b))
        return true;
    else
        return false;
}

inline bool operator<(const Edge &f, const Edge &g)
{
    if(f.Length() < g.Length())
        return true;
    else
        return false;
}

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , _open("Open...")
    , _exec("Build")
    , _exit("Exit")
    , _addPoints("Add points...")
    , _save("Save...")
    , _next(">")
    , _prev("<")
    , _toFirst("<<")
    , _toLast(">>")
{
    this->setLayout(&_mainL);
    _mainL.addLayout(&_buttonL, 0, 1, 2, 1);
    _mainL.addLayout(&_pnL, 1, 0, 2, 1);
    _mainL.addWidget(&_drawer, 0, 0);
    _buttonL.addWidget(&_exec, 0, 0);
    _buttonL.addWidget(&_addPoints, 1, 0);
    _buttonL.addWidget(&_open, 2, 0);
    _buttonL.addWidget(&_save, 3, 0);
    _buttonL.addWidget(new QWidget(), 9, 0);
    _buttonL.addWidget(&_exit, 10, 0);

    _pnL.addWidget(new QWidget(), 0, 0);
    _pnL.addWidget(&_toFirst, 0, 1);
    _pnL.addWidget(&_prev, 0, 2);
    //_pnL.addWidget(new QWidget(), 0, 3);
    _pnL.addWidget(&_next, 0, 4);
    _pnL.addWidget(&_toLast, 0, 5);
    _pnL.addWidget(new QWidget(), 0, 6);
    ShowExtraButtons(false);

    QObject::connect(&_exit, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(&_exec, SIGNAL(clicked()), this, SLOT(BuildAlgo()));
    QObject::connect(&_open, SIGNAL(clicked()), this, SLOT(OpenFile()));
    QObject::connect(&_save, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    QObject::connect(&_addPoints, SIGNAL(clicked()), this, SLOT(AddPointsClicked()));

    QObject::connect(&_next, SIGNAL(clicked()), this, SLOT(NextClicked()));
    QObject::connect(&_prev, SIGNAL(clicked()), this, SLOT(PrevClicked()));
    QObject::connect(&_toLast, SIGNAL(clicked()), this, SLOT(LastClicked()));
    QObject::connect(&_toFirst, SIGNAL(clicked()), this, SLOT(FirstClicked()));

    _drawer.setAlgo(&_algo);
}

void MainWindow::BuildAlgo()
{
    if (!_algo.Init() && !_algo.Build())
    {
        ShowExtraButtons(true);
    }
    _drawer.repaint();
}

void MainWindow::OpenFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    _algo.Reset();

    QTextStream in(&file);
    int n;
    in >> n;
    for (int i = 0; i < n; ++i)
    {
        qreal x,y;
        in >> x >> y;
        _algo.AddPoint(x, y);
    }
}

void MainWindow::SaveToFile()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save to File"));
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    _algo.ResetToAdd();
    QTextStream out(&file);
    std::vector<QPointF> *vec = _algo.GetVectorR();
    out << vec->size() << "/n";
    for (int i = 0; i < vec->size(); ++i)
    {
        out << (*vec)[i].x() << " " << (*vec)[i].y() << "/n";
    }
    out.flush();
    _algo.Build();
    _drawer.repaint();
}

void MainWindow::PrevClicked()
{
    _algo.StepBack();
    _drawer.repaint();
}

void MainWindow::NextClicked()
{
    _algo.StepForward();
    _drawer.repaint();

}

void MainWindow::FirstClicked()
{
    _algo.SkipBack();
    _drawer.repaint();
}

void MainWindow::LastClicked()
{
    _algo.SkipForward();
    _drawer.repaint();
}

void MainWindow::AddPointsClicked()
{
    _algo.ResetToAdd();
    ShowExtraButtons(false);
    _drawer.repaint();
}

void MainWindow::ShowExtraButtons(bool f)
{
    if (f)
    {
        _prev.show();
        _next.show();
        _toLast.show();
        _toFirst.show();
    }
    else
    {
        _prev.hide();
        _toFirst.hide();
        _next.hide();
        _toLast.hide();
    }
}

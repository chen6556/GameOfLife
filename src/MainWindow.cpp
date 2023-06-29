#include "MainWindow.hpp"
#include "./ui_mainwindow.h"
#include "File.hpp"
#include <QPainter>
#include <QBrush>
#include <QFileDialog>
#include <Qstring>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _info;
}



void MainWindow::init()
{
    connect(&_timer, &QTimer::timeout, this, &MainWindow::revolution);
    ui->canvas->installEventFilter(this);

    ui->status_bar->setSizeGripEnabled(false);
    _info = new QLabel(this);
    ui->status_bar->addPermanentWidget(_info);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Paint && obj == ui->canvas)
    {
        QPainter painter(ui->canvas);
        QPen pen;
        pen.setColor(QColor(34, 34, 34));
        pen.setWidth(0);
        painter.setPen(pen);

        const int w = ui->canvas->width(), h = ui->canvas->height();
        for (int i = 0; i <= w; i += 8)
        {
            painter.drawLine(QPoint(i, 0), QPoint(i, h));
        }
        for (int i = 0; i <= h; i += 8)
        {
            painter.drawLine(QPoint(0, i), QPoint(w, i));
        }

        QBrush brush;
        brush.setColor(QColor(51, 102, 0));
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);
        for (int i = 0, row = _checkerboard.size(), col = _checkerboard.back().size(); i < row; ++i)
        {
            for (int j = 0; j < col; ++j)
            {
                if (_checkerboard[i][j])
                {
                    painter.drawRect(8 * j, 8 * i, 8, 8);
                }
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::read()
{
    QFileDialog dialog;
    dialog.setModal(true);
    QString path = dialog.getOpenFileName(this, QString(), QString(), "Files: *.txt");
    if (!path.isEmpty())
    {
        const int w = width() - ui->canvas->width(), h = height() - ui->canvas->height();
        _checkerboard = File::read(path.toStdString());
        resize(_checkerboard.back().size() * 8 + w, _checkerboard.size() * 8 + h);
    }
}

void MainWindow::write()
{
    QFileDialog dialog;
    dialog.setModal(true);
    QString path = dialog.getSaveFileName(this, QString(), QString(), "Files: *.txt");
    if (!path.isEmpty())
    {
        File::write(path.toStdString(), _checkerboard);
    }
}



void MainWindow::play()
{
    _playing = !_playing;
    if (_playing)
    {
        _timer.start(500);
        _info->clear();
    }
    else
    {
        _timer.stop();
        _info->setText("Pause");
    }
}

void MainWindow::clear()
{
    _timer.stop();
    _playing = false;
    _count = 0;
    _checkerboard.assign(ui->canvas->height() / 8, std::vector<bool>(ui->canvas->width() / 8, false));
    _info->clear();
    update();
}

void MainWindow::protect()
{
    _paintable = !ui->action_protect->isChecked();
}

void MainWindow::revolution()
{
    int count;
    std::vector<std::vector<bool>> checkerboard(_checkerboard);
    for (int i = 0, row = _checkerboard.size(), col = _checkerboard.back().size(); i < row; ++i)
    {   
        for (int j = 0; j < col; ++j)
        {
            count = 0;
            if (i > 0)
            {
                if (j > 0 && _checkerboard[i - 1][j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _checkerboard[i - 1].back())
                {
                    ++count;
                }
                if (_checkerboard[i - 1][j])
                {
                    ++count;
                }
                if (j < col - 1 && _checkerboard[i - 1][j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _checkerboard[i - 1].front())
                {
                    ++count;
                }
            }
            else
            {
                if (j > 0 && _checkerboard.back()[j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _checkerboard.back().back())
                {
                    ++count;
                }
                if (_checkerboard.back()[j])
                {
                    ++count;
                }
                if (j < col - 1 && _checkerboard.back()[j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _checkerboard.back().front())
                {
                    ++count;
                }
            }
            if (j > 0 && _checkerboard[i][j - 1])
            {
                ++count;
            }
            else if (j == 0 && _checkerboard[i].back())
            {
                ++count;
            }
            if (j < col - 1 && _checkerboard[i][j + 1])
            {
                ++count;
            }
            else if (j == col - 1 && _checkerboard[i].front())
            {
                ++count;
            }
            if (i < row - 1)
            {
                if (j > 0 && _checkerboard[i + 1][j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _checkerboard[i + 1].back())
                {
                    ++count;
                }
                if (_checkerboard[i + 1][j])
                {
                    ++count;
                }
                if (j < col - 1 && _checkerboard[i + 1][j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _checkerboard[i + 1].front())
                {
                    ++count;
                }
            }
            else
            {
                if (j > 0 && _checkerboard.front()[j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _checkerboard.front().back())
                {
                    ++count;
                }
                if (_checkerboard.front()[j])
                {
                    ++count;
                }
                if (j < col - 1 && _checkerboard.front()[j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _checkerboard.front().front())
                {
                    ++count;
                }
            }

            if (_checkerboard[i][j] && (count <= 1 || count >= 4))
            {
                checkerboard[i][j] = false;
            }
            else if (!_checkerboard[i][j] && count == 3)
            {
                checkerboard[i][j] = true;
            }
        }
    }

    _checkerboard.assign(checkerboard.cbegin(), checkerboard.cend());
    ui->status_bar->showMessage(QString::fromStdString(std::string("Revolution times : ").append(std::to_string(++_count))));
    update();

    count = 0;
    for (const std::vector<bool> &row : _checkerboard)
    {
        count += std::count(row.cbegin(), row.cend(), true);
    }
    if (count == 0)
    {
        _timer.stop();
        _info->setText("Game over : all cells died.");
    }
}



void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!_paintable)
    {
        return QMainWindow::mousePressEvent(event);
    }
    switch (event->button())
    {
    case Qt::LeftButton:
        _paint = 1;
        break;
    case Qt::RightButton:
        _paint = 2;
        break;
    default:
        _paint = 0;
        return QMainWindow::mousePressEvent(event);
    }
    
    const QPointF pos(ui->canvas->mapFrom(this, event->position()));

    const int row = pos.y() / 8, col = pos.x() / 8;
    if (row < 0 || row >= _checkerboard.size() || col < 0 || col >= _checkerboard.back().size())
    {
        return QMainWindow::mousePressEvent(event);
    }

    _checkerboard[row][col] = _paint == 1;

    update();
    
    return QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {   
        _paint = 0;
    }

    return QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (_paint != 1 && _paint != 2)
    {
        return QMainWindow::mouseMoveEvent(event);
    }
    
    const QPointF pos(ui->canvas->mapFrom(this, event->position()));

    const int row = pos.y() / 8, col = pos.x() / 8;
    if (row < 0 || row >= _checkerboard.size() || col < 0 || col >= _checkerboard.back().size())
    {
        return QMainWindow::mouseMoveEvent(event);
    }

    _checkerboard[row][col] = _paint == 1;

    update();

    return QMainWindow::mouseMoveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (_checkerboard.empty())
    {
        _checkerboard.assign((this->height() - 62) / 8 , std::vector<bool>((this->width() - 18) / 8, false));
        return QMainWindow::resizeEvent(event);
    }
    
    const int w = ui->canvas->width(), h = ui->canvas->height();
    int cols = _checkerboard.empty() ? w / 8 : _checkerboard.back().size();
    if (_checkerboard.size() > h / 8)
    {
        for (int i = _checkerboard.size(); i > h / 8; --i)
        {
            if (i % 2 == 0)
            {
                _checkerboard.pop_back();
            }
            else
            {
                _checkerboard.erase(_checkerboard.begin());
            }
        }
    }
    else
    {
        for (int i = _checkerboard.size(); i < h / 8; ++i)
        {
            if (i % 2 == 0)
            {
                _checkerboard.push_back(std::vector<bool>(cols, false));
            }
            else
            {
                _checkerboard.insert(_checkerboard.begin(), std::vector<bool>(cols, false));
            }
        }
    }

    if (_checkerboard.back().size() > w / 8)
    {
        for (int i = _checkerboard.back().size(); i > w / 8; --i)
        {
            if (i % 2 == 0)
            {
                for (std::vector<bool> &row : _checkerboard)
                {
                    row.pop_back();
                }
            }
            else
            {
                for (std::vector<bool> &row : _checkerboard)
                {
                    row.erase(row.begin());
                }
            }
        }
    }
    else
    {
        for (int i = _checkerboard.back().size(); i < w / 8; ++i)
        {
            if (i % 2 == 0)
            {
                for (std::vector<bool> &row : _checkerboard)
                {
                    row.push_back(false);
                }
            }
            else
            {
                for (std::vector<bool> &row : _checkerboard)
                {
                    row.insert(row.begin(), false);
                }
            }
        }
    }

    return QMainWindow::resizeEvent(event);
}



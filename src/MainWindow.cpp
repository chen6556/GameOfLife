#include "MainWindow.hpp"
#include "./ui_mainwindow.h"
#include "File.hpp"
#include <QPainter>
#include <QBrush>
#include <QFileDialog>
#include <Qstring>
#include <random>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    if (_refresh_thread != nullptr)
    {
        _refresh_cache = false;
        _mutex.lock();
        delete _refresh_thread;
        _mutex.unlock();
    }
    delete ui;
    delete _info;
}



void MainWindow::init()
{
    connect(&_timer, &QTimer::timeout, this, &MainWindow::revolution);
    ui->canvas->installEventFilter(this);

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

void MainWindow::change_speed(const int value)
{
    _speed = value;
    ui->label_speed->setText(std::to_string(value).c_str());
    if (_timer.isActive())
    {
        _timer.start(1000 / _speed);
    }
}

void MainWindow::fill()
{
    std::default_random_engine e;
    std::bernoulli_distribution u;
    for (int i = 0, rows = _checkerboard.size(), cols = _checkerboard.back().size(); i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            _checkerboard[i][j] = u(e);
        }
    }
    ui->canvas->update();

    ui->status_bar->showMessage("Fill finished.");
}



void MainWindow::play()
{
    _playing = !_playing;
    if (_playing)
    {
        _timer.start(1000 / _speed);
        _info->clear();
        _refresh_cache = true;
        _refresh_thread = new std::thread(std::mem_fn(&MainWindow::refresh_cache), this);
        _refresh_thread->detach();
    }
    else
    {
        _timer.stop();
        _info->setText("Pause");
    }
}

void MainWindow::clear()
{
    _refresh_cache = false;
    _timer.stop();
    _playing = false;
    _count = 0;
    _checkerboard.assign(ui->canvas->height() / 8, std::vector<bool>(ui->canvas->width() / 8, false));
    _info->clear();
    ui->canvas->update();
    _cache.clear();
    if (_refresh_thread != nullptr)
    {
        _mutex.lock();
        delete _refresh_thread;
        _mutex.unlock();
        _refresh_thread = nullptr;
    }
}

void MainWindow::protect()
{
    _paintable = !ui->action_protect->isChecked();
}

void MainWindow::refresh_cache_subfunc(std::tuple<std::vector<std::vector<bool>> *, const int, const int> args)
{
    int count;
    for (int i = std::get<1>(args), row = std::get<2>(args), col = _checkerboard.back().size(); i < row; ++i)
    {   
        for (int j = 0; j < col; ++j)
        {
            count = 0;
            if (i > 0)
            {
                if (j > 0 && _cache.back()[i - 1][j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _cache.back()[i - 1].back())
                {
                    ++count;
                }
                if (_cache.back()[i - 1][j])
                {
                    ++count;
                }
                if (j < col - 1 && _cache.back()[i - 1][j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _cache.back()[i - 1].front())
                {
                    ++count;
                }
            }
            else
            {
                if (j > 0 && _cache.back().back()[j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _cache.back().back().back())
                {
                    ++count;
                }
                if (_cache.back().back()[j])
                {
                    ++count;
                }
                if (j < col - 1 && _cache.back().back()[j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _cache.back().back().front())
                {
                    ++count;
                }
            }
            if (j > 0 && _cache.back()[i][j - 1])
            {
                ++count;
            }
            else if (j == 0 && _cache.back()[i].back())
            {
                ++count;
            }
            if (j < col - 1 && _cache.back()[i][j + 1])
            {
                ++count;
            }
            else if (j == col - 1 && _cache.back()[i].front())
            {
                ++count;
            }
            if (i < row - 1)
            {
                if (j > 0 && _cache.back()[i + 1][j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _cache.back()[i + 1].back())
                {
                    ++count;
                }
                if (_cache.back()[i + 1][j])
                {
                    ++count;
                }
                if (j < col - 1 && _cache.back()[i + 1][j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _cache.back()[i + 1].front())
                {
                    ++count;
                }
            }
            else
            {
                if (j > 0 && _cache.back().front()[j - 1])
                {
                    ++count;
                }
                else if (j == 0 && _cache.back().front().back())
                {
                    ++count;
                }
                if (_cache.back().front()[j])
                {
                    ++count;
                }
                if (j < col - 1 && _cache.back().front()[j + 1])
                {
                    ++count;
                }
                else if (j == col - 1 && _cache.back().front().front())
                {
                    ++count;
                }
            }

            if (_cache.back()[i][j] && (count <= 1 || count >= 4))
            {
                (*std::get<0>(args))[i][j] = false;
            }
            else if (!_cache.back()[i][j] && count == 3)
            {
                (*std::get<0>(args))[i][j] = true;
            }
        }
    }
}

void MainWindow::refresh_cache()
{
    bool flag;
    std::vector<std::thread> threads;
    const int cpus = std::thread::hardware_concurrency();
    const int rows = _checkerboard.size();
    const int step = rows / cpus;
    while (_refresh_cache)
    {
        if (_cache.size() > 30)
        {
            continue;
        }
        std::vector<std::vector<bool>> checkerboard(_checkerboard);

        _mutex.lock();
        if (_cache.empty())
        {
            _cache.push_back(_checkerboard);
            flag = true;
        }
        else
        {
            flag = false;
        }
        
        for (int i = 1; i < cpus; ++i)
        {
            threads.push_back(std::thread(std::mem_fn(&MainWindow::refresh_cache_subfunc), this,
                std::make_tuple(&checkerboard, step * (i - 1), step * i)));
        }
        threads.push_back(std::thread(std::mem_fn(&MainWindow::refresh_cache_subfunc), this,
                std::make_tuple(&checkerboard, step * (cpus - 1), rows)));
        for (std::thread &t : threads)
        {
            t.join();
        }
        
        if (flag)
        {
            _cache.pop_front();
        }

        _cache.push_back(checkerboard);
        _mutex.unlock();

        threads.clear();
    }
}

void MainWindow::revolution()
{
    if (_cache.empty())
    {
        return;
    }

    _checkerboard.assign(_cache.front().cbegin(), _cache.front().cend());
    _mutex.lock();
    _cache.pop_front();
    _mutex.unlock();
    ui->status_bar->showMessage(QString::fromStdString(std::string("Revolution times : ").append(std::to_string(++_count))));
    ui->canvas->update();

    int count = 0;
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

    ui->canvas->update();
    
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

    ui->canvas->update();

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
    const int base_w = _checkerboard.back().size(), base_h = _checkerboard.size();
    int cols = _checkerboard.empty() ? w / 8 : _checkerboard.back().size();
    if (_checkerboard.size() > h / 8)
    {
        _checkerboard.erase(_checkerboard.end() - (base_h - (h / 8)) / 2, _checkerboard.end());
        _checkerboard.erase(_checkerboard.begin(), _checkerboard.begin() + (base_h - (h / 8)) / 2);
    }
    else
    {
        _checkerboard.insert(_checkerboard.begin(), ((h / 8) - base_h) / 2, std::vector<bool>(cols, false));
        _checkerboard.insert(_checkerboard.end(), ((h / 8) - base_h) / 2, std::vector<bool>(cols, false));
    }

    if (_checkerboard.back().size() > w / 8)
    {
        for (std::vector<bool> &row : _checkerboard)
        {
            row.erase(row.begin(), row.begin() + (base_w - (w / 8)) / 2);
            row.erase(row.end() - (base_w - (w / 8)) / 2, row.end());
        }
    }
    else
    {
        for (std::vector<bool> &row : _checkerboard)
        {
            row.insert(row.begin(), ((w / 8) - base_w) / 2, false);
            row.insert(row.end(), ((w / 8) - base_w) / 2, false);
        }
    }

    return QMainWindow::resizeEvent(event);
}



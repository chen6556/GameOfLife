#pragma once
#include <QMainWindow>
#include <QTimer>
#include <vector>
#include <QMouseEvent>
#include <QLabel>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui = nullptr;
    bool _playing = false;
    bool _paintable = true;
    int _paint = 0; // 0:None 1:Paint 2:Erase
    std::vector<std::vector<bool>> _checkerboard;

    QTimer _timer;
    size_t _count = 0;
    int _speed = 2;
    QLabel *_info = nullptr;

private:
    void init();

    bool eventFilter(QObject *obj, QEvent *event);

    void revolution_subfunc(std::tuple<std::vector<std::vector<bool>> *, const int, const int> args);

private slots:
    void play();

    void clear();

    void protect();

    void revolution();

    void read();

    void write();

    void change_speed(const int value);

    void fill();

protected:
    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void resizeEvent(QResizeEvent *event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
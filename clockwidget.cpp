#include "clockwidget.h"
#include "ui_clockwidget.h"

//#include <math.h>
#include <QtMath>
#include <QPainter>
#include <QTime>

ClockWidget::ClockWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClockWidget)
{
    ui->setupUi(this);

    // 构造定时器，设置超时为 1 秒
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    setWindowTitle("时钟");
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
}

ClockWidget::~ClockWidget()
{
    delete ui;
}

QRectF ClockWidget::textRectF(double radius, int pointSize, double angle)
{
    QRectF rectF;
    rectF.setX(radius*qCos(angle*M_PI/180.0) - pointSize*2);
    rectF.setY(radius*qSin(angle*M_PI/180.0) - pointSize/2.0);
    rectF.setWidth(pointSize*4);
    rectF.setHeight(pointSize);
    return rectF;
}

void ClockWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        is_drag = true;
        //获得鼠标的初始位置
        mouse_start_point = event->globalPos();
        //获得窗口的初始位置
        window_start_point = this->frameGeometry().topLeft();
    }
}

void ClockWidget::mouseMoveEvent(QMouseEvent *event)
{
    //判断是否在拖拽移动
    if (is_drag)
    {
        //获得鼠标移动的距离
        QPoint move_distance = event->globalPos() - mouse_start_point;
        //改变窗口的位置
        this->move(window_start_point + move_distance);
    }
}

void ClockWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //放下左键即停止移动
    if (event->button() == Qt::LeftButton)
    {
        is_drag = false;
    }
}

void ClockWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    static const int point_radius = 10;
    // 时针、分针、秒针位置 - 多边形
    static const QPoint hourHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -30)
    };
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -65)
    };
    static const QPoint secondHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -80)
    };

    // 时针、分针、秒针颜色
    QColor hourColor(200, 100, 0, 200);
    QColor minuteColor(0, 127, 127, 150);
    QColor secondColor(0, 160, 230, 150);

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 平移坐标系原点至中心点
    painter.translate(width() / 2, height() / 2);
    // 缩放
    painter.scale(side / 200.0, side / 200.0);

    // 绘制时针
    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);

    painter.save();
    // 每圈360° = 12h 即：旋转角度 = 小时数 * 30°
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(hourHand, 3);
    painter.restore();

    painter.setPen(hourColor);

    // 绘制小时线 （360度 / 12 = 30度）
    for (int i = 0; i < 12; ++i) {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(30.0);
    }

    int radius = 100;
    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);
    int pointSize = font.pointSize();

    // 绘制小时文本
    int nHour = 0;
    for (int i = 0; i < 12; ++i) {
        nHour = i + 3;
        if (nHour > 12)
            nHour -= 12;
        painter.drawText(textRectF(radius*0.8, pointSize, i * 30), Qt::AlignCenter, QString::number(nHour));
    }

    // 绘制分针
    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);
    painter.save();
    // 每圈360° = 60m 即：旋转角度 = 分钟数 * 6°
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();

    painter.setPen(minuteColor);

    // 绘制分钟线 （360度 / 60 = 6度）
    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter.drawLine(92, 0, 96, 0);
        painter.rotate(6.0);
    }

    // 绘制秒针
    painter.setPen(Qt::NoPen);
    painter.setBrush(secondColor);

    painter.save();
    // 每圈360° = 60s 即：旋转角度 = 秒数 * 6°
    painter.rotate(6.0 * time.second());
    painter.drawConvexPolygon(secondHand, 3);
    painter.restore();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::darkBlue));
    painter.drawEllipse(-point_radius/2, -point_radius/2, point_radius, point_radius);
}

void ClockWidget::on_pushButton_clicked()
{
    this->close();
}

void ClockWidget::on_pushButton_2_clicked()
{
    //隐藏程序主窗口
    this->hide();

    //新建QSystemTrayIcon对象
    if (trayIcon == NULL)
    {
        trayIcon = new QSystemTrayIcon(this);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

        //新建托盘要显示的icon
        QIcon icon = QIcon(":/images/clock.png");
        //将icon设到QSystemTrayIcon对象中
        trayIcon->setIcon(icon);

        //当鼠标移动到托盘上的图标时，会显示此处设置的内容
        trayIcon->setToolTip(QString("辰光融信 耗材写入工具"));
    }

    //在系统托盘显示此对象
    trayIcon->show();
}

void ClockWidget::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;

    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        this->show();
        trayIcon->hide();
        break;

    default:
        break;
    }
}

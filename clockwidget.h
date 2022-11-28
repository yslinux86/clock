#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class ClockWidget; }
QT_END_NAMESPACE

class ClockWidget : public QWidget
{
    Q_OBJECT

public:
    ClockWidget(QWidget *parent = nullptr);
    ~ClockWidget();

private:
    Ui::ClockWidget *ui;
    QSystemTrayIcon *trayIcon = NULL;
    bool is_drag = false;
    QPoint mouse_start_point;
    QPoint window_start_point;

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

private:
    QRectF textRectF(double radius, int pointSize, double angle);
};
#endif // CLOCKWIDGET_H

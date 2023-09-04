#ifndef RADARWIDGET_H
#define RADARWIDGET_H

#include <QWidget>
#include <QUdpSocket>
#include <QTimer>
#include <QString>
#include <QRandomGenerator>
#include <QDebug>
#include "QVector"
#include <QStringList>
#include <QPainter>

extern bool flag;              // 用于确定每一批数量
extern bool erase;             // 用于确定是否消除航迹
extern QString temp ;          // 暂存每次拿到的结果
extern int jishu ;             // 画图时拿到的一批数量
extern int checkBox_state;     // 记录点击状态
extern QString PiHao ;         // 记录批号

namespace Ui {
class RadarWidget;
}

class RadarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);
    ~RadarWidget();
    void drawPath(QPainter& painter);
    void drawPoint(QPainter& painter);
    void drawAxisMark(QPainter& painter, int x, int y, const QString& text);
    void drawCoordinateAxes(QPainter& painter);
    void drawRadar(QPainter& painter);
    void updatePoint();
    void paintEvent(QPaintEvent* event);


private slots:
    void on_checkBox_clicked();
    void on_exitButton_clicked();

private:
    Ui::RadarWidget *ui;
    QUdpSocket *socket_receive;
    QTimer* timer;
    int centerX;
    int centerY;
    int radius;
    int angle;
    QPointF currentPoint;
    std::vector<QPointF> path;
    QPainter painte;
};
#endif // RADARWIDGET_H

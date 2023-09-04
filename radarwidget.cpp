#include "radarwidget.h"
#include "ui_radarwidget.h"

bool flag = false;          // 用于确定每一批数量
bool erase = false;         // 用于确定是否消除航迹
QString temp = nullptr;     // 暂存每次拿到的结果
int jishu = 1;              // 画图时拿到的一批数量
int checkBox_state = 0;     // 记录点击状态
QString PiHao = nullptr;    // 记录批号
//int count = 1;            //记录一次数量

RadarWidget::RadarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadarWidget)
{
    ui->setupUi(this);
    ui->checkBox->setCheckState(Qt::Unchecked);  //设置初始状态
    checkBox_state = Qt::Checked;            //checkBox_state 是在类中声明的，类型为int型
    setMinimumSize(800, 800);
//    srand(static_cast<unsigned int>(time(nullptr)));

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RadarWidget::updatePoint);
    timer->start(1000); // 每秒更新一次点

    centerX = width() / 2;
    centerY = height() / 2;
    radius = std::min(centerX, centerY)-40;

    currentPoint.setX(centerX);
    currentPoint.setY(centerY);

    angle = 0;

    socket_receive=new QUdpSocket(this);

    //组播形式,且需要运用IPV4
    socket_receive->bind(QHostAddress::AnyIPv4,9000,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    //加入组播,对应着D类地址
    socket_receive->joinMulticastGroup(QHostAddress("224.0.0.10"));

}

RadarWidget::~RadarWidget()
{
    delete ui;
    delete timer;
    delete socket_receive;
}

void RadarWidget::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawRadar(painter);
    drawCoordinateAxes(painter);
    drawPoint(painter);
    drawPath(painter);

//    painter.restore();
}


void RadarWidget::updatePoint() {

    //当接收端接收到数据时，就会发送readRead信号
    connect(socket_receive, &QUdpSocket::readyRead, [this](){
        while(socket_receive->hasPendingDatagrams()){
            QByteArray dataGram;
            //读取的数据报大小
            dataGram.resize(socket_receive->pendingDatagramSize());
            socket_receive->readDatagram(dataGram.data(),dataGram.size());
            QString re = QString(dataGram);
            QStringList ql = re.split(" ");

            double newX = (centerX + ql[2].toDouble()/10);
            double newY = (centerY + ql[3].toDouble()/10);
            qDebug() << centerX <<endl;
            qDebug() << centerY <<endl;
            qDebug() << ql[2].toDouble() <<endl;
            qDebug() << ql[3].toDouble()<<endl;
            qDebug() << newX<<endl;
            qDebug() << newY<<endl;
//            qDebug() << "***********************"<<endl;

            // 更新新点的坐标
            currentPoint.setX(newX);
            currentPoint.setY(newY);

            // 输出新点的坐标
            qDebug() << "新点坐标：(" << newX << ", " << newY << ")";
            // 将新点添加到轨迹路径中

            QString everytime = ql[0];
            if(everytime != temp)
            {
                PiHao = ql[0];
                temp = ql[0];
                jishu = 1;
            }
            else {
                jishu++;
            }

            path.push_back(currentPoint);

//            qDebug() << kaiguan<<endl;
//            qDebug() << temp<<endl;

            qDebug() << jishu << endl;
            qDebug() << PiHao << endl;
            qDebug() << "***********************"<< endl;

            update(); // 更新绘图
        }
    });
//    // 生成随机的新点
//    double newX = centerX + (rand() % 201 - 100); // 在-100到100之间随机
//    double newY = centerY + (rand() % 201 - 100);


}

void RadarWidget::drawRadar(QPainter& painter) {
    painter.setPen(QPen(Qt::black, 3));
    int numCircles = 5;
    for (int i = 1; i <= numCircles; ++i) {
        painter.drawEllipse(centerX - radius * i / numCircles, centerY - radius * i / numCircles,
                            radius * 2 * i / numCircles, radius * 2 * i / numCircles);
    }
}

void RadarWidget::drawCoordinateAxes(QPainter& painter) {
    painter.setPen(QPen(Qt::black, 3));

    // 绘制x轴
    painter.drawLine(0, centerY, width(), centerY);
    painter.drawText(width() - 20, centerY - 10, "X");

    // 绘制y轴
    painter.drawLine(centerX, 0, centerX, height());
    painter.drawText(centerX + 10, 20, "Y");

    // 绘制交点上的标注
    painter.setFont(QFont("宋体",15,1)); //QFont(字体，大小，线宽)
    drawAxisMark(painter, centerX, 0, "0");
    drawAxisMark(painter, centerX + radius, centerY, "90");
    drawAxisMark(painter, centerX, centerY + radius, "180");
    drawAxisMark(painter, centerX - radius, centerY, "270");
}

void RadarWidget::drawAxisMark(QPainter& painter, int x, int y, const QString& text) {
    painter.drawText(x - 10, y + 20, text);
    painter.drawLine(x - 5, y, x + 5, y);
    painter.drawLine(x, y - 5, x, y + 5);
}

void RadarWidget::drawPoint(QPainter& painter) {
    painter.setPen(QPen(Qt::red, 3));
    painter.drawPoint(currentPoint);
}

void RadarWidget::drawPath(QPainter& painter) {

    if (path.size() < 2) {
        return;
    }

    painter.setPen(QPen(Qt::darkGreen, 2));

    if(!erase){
        painter.drawText(path[path.size()-1], PiHao);

        for (size_t i = (path.size() - jishu) + 1; i < path.size(); ++i) {
            painter.drawLine(path[i - 1], path[i]);
        }
    }

    qDebug() << QString::number(path.size()) << endl;
    qDebug() << QString::number(jishu) << endl;
    qDebug() << "#####################################" << endl;
}

void RadarWidget::on_checkBox_clicked()
{
    if (checkBox_state == Qt::Checked)      // "选中"
    {
        erase = true;
        checkBox_state = Qt::Unchecked;
    }
    else if(checkBox_state == Qt::Unchecked)// 未选中
    {
        erase = false;
        checkBox_state = Qt::Checked;
    }

}

void RadarWidget::on_exitButton_clicked()
{
    this->close();
}



#ifndef ADMIN_H
#define ADMIN_H

#include <QWidget>
#include "widget.h"
#include <QtCharts>
#include <QOpcUaType.h>
#include <QOpcUaProvider>
#include <QWidget>
#include <QOpcUaClient.h>
#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QComboBox>
#include <QTimer>
#include <qopcuanode.h>
#include <QQueue>
#include <qopcuanodeids.h>
#include <QTextEdit>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QThread>
#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>
#include <QMetaObject>
#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <string>
#include <string.h>// 引入所有QtCharts绘制的头文件，也可以单独引入某一个头文件
#include <QDateTimeAxis>
#include <QDateTime>
namespace Ui {
class admin;
}

class admin : public QWidget
{
    Q_OBJECT

public:
    //QChartView *chartView = new QChartView(this);
    explicit admin(QWidget *parent = nullptr);
    ~admin();
    void paintEvent(QPaintEvent *e);//重绘图函数
    QString strValue=NULL;
    void readNodeVlue();
    Widget *wo = new Widget;
    QOpcUaClient *client = nullptr;
    void connectserver();
    QString temper = NULL;
    //QChart *chart = new QChart();
    //QLineSeries *series0 = new QLineSeries;
    bool stop = false;
    QString nodeid = "";
    void updateProgressBar(int completed, int total);
    int shuchu = 0;
    int shengchan = 0;
    int wancheng = 0;
    int wancheng1 = 0;
    int wancheng2 = 0;
    int wancheng3 = 0;
    //前轮机器人节点
    QString qlltrobstart = "ns=4;i=29";
    QString qlltrobstop = "ns=4;i=30";
    QString qlltrobhome = "ns=4;i=31";
    QString qlltrobemstop = "ns=4;i=51";
    //前轮螺栓机器人节点
    QString qllsrobstart = "ns=4;i=41";
    QString qllsrobstop = "ns=4;i=42";
    QString qllsrobhome = "ns=4;i=43";
    QString qllsrobemstop = "ns=4;i=55";
    //前轮螺母机器人节点
    QString qllmrobstart = "ns=4;i=35";
    QString qllmrobstop = "ns=4;i=36";
    QString qllmrobhome = "ns=4;i=37";
    QString qllmrobemstop = "ns=4;i=53";
    //后轮机器人节点
    QString hlltrobstart = "ns=4;i=26";
    QString hlltrobstop = "ns=4;i=27";
    QString hlltrobhome = "ns=4;i=28";
    QString hlltrobemstop = "ns=4;i=50";
    //后轮螺栓机器人节点
    QString hllsrobstart = "ns=4;i=38";
    QString hllsrobstop = "ns=4;i=39";
    QString hllsrobhome ="ns=4;i=40";
    QString hllsrobemstop ="ns=4;i=54";
    //后轮螺母机器人节点
    QString hllmrobstart = "ns=4;i=32";
    QString hllmrobstop = "ns=4;i=33";
    QString hllmrobhome ="ns=4;i=34";
    QString hllmrobemstop ="ns=4;i=50";
    //座椅安装机器人
    QString zyazrobstart = "ns=4;i=44";
    QString zyazrobstop = "ns=4;i=45";
    QString zyazrobhome = "ns=4;i=46";
    QString zyazrobemstop = "ns=4;i=56";
    //座椅螺丝刀机器人
    QString zylsrobstart ="ns=4;i=47";
    QString zylsrobstop = "ns=4;i=48";
    QString zylsrobhome ="ns=4;i=49";
    QString zylsrobemstop ="ns=4;i=57";
    //产线节点
    QString allstart = "ns=4;i=59";
    QString allstop = "ns=4;i=64";
    QString allemstop = "ns=4;i=58";
    QString allhome = "ns=4;i=62";
    //传送带节点
    QString conveyorstart ="ns=4;i=60";
    QString conveyorstop ="ns=4;i=61";
    //生产数量
    QString productnumber = "ns=4;i=25";
    bool startb =false;
    bool allstopb = false;
    int p =0;
    void decrementRemainingTime();
    QTimer *m_timer;
    int remainingTime = 0;
    int leftTime = 0;
    int leftTime2 = 0;
    QTimer *time = new QTimer(this);

signals:
    void showmain();
private:

    Ui::admin *ui;
    int i =0;
    QChartView *chartView = new QChartView(this);//图表函数
    QChart *chart = new QChart();
    QLineSeries *series0 = new QLineSeries;
    void writeNodeValueWithDelay(const QString &nodeId, const QVariant &value, int delayMillis);

public slots:
    void showmead();//显示页面函数
    void handleReadNodeAttributesFinished(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult);//读取温度节点函数
    void handleReadNodeAttributesFinished1(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult);//处理节点返回值（测试）
    void writeNodeValue(const QString &nodeId, const QVariant &value);//修改节点函数
    void handleWriteNodeAttributesFinished(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult);//修改节点槽函数
private slots:
    void on_ui_back_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_ui_startall_clicked();//产线启动生产
    void on_ui_stopall_clicked();//产线停止生产
    void on_ui_allhome_clicked();//产线复位
    void on_ui_allemstop_clicked();//产线急停
    void on_ui_comfrimproduct_clicked();//生产数量确认
    void ijiajia();
};

#endif // ADMIN_H
